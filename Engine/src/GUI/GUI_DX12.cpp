#include <EnginePCH.h>

#include <GUI/GUI.h>

#include <Common.h>
#include <Graphics/Device.h>
#include <Graphics/Window.h>
#include <Graphics/Profiler.h>
#include <Graphics/DX12/ShaderDX12.h>
#include <Graphics/ShaderParameter.h>
#include <Graphics/Texture.h>
#include <Graphics/GraphicsCommandQueue.h>
#include <Graphics/GraphicsCommandBuffer.h>
#include <Graphics/GraphicsPipelineState.h>
#include <Graphics/BlendState.h>
#include <Graphics/DepthStencilState.h>
#include <Graphics/RasterizerState.h>
#include <Graphics/Fence.h>

using namespace Graphics;

std::shared_ptr<GraphicsPipelineState> g_PipelineState;
std::shared_ptr<Texture> g_FontTexture;

void BindDevice( std::shared_ptr<Graphics::Device> device, std::shared_ptr<Graphics::Window> window )
{
    if ( !device || !window ) return;

    ImGuiIO& io = ImGui::GetIO();

    // Upload font texture
    {
        std::shared_ptr<ComputeCommandQueue> commandQueue = device->GetComputeQueue();
        std::shared_ptr<ComputeCommandBuffer> commandBuffer = commandQueue->GetComputeCommandBuffer();
        
        uint8_t* pixelData;
        int width, height;
        
        io.Fonts->GetTexDataAsRGBA32( &pixelData, &width, &height );
        g_FontTexture = device->CreateTexture2D( width, height, 1, TextureFormat::R8G8B8A8_UNORM );
        g_FontTexture->SetName( L"ImGui Default Font Texture" );

        commandBuffer->SetTextureSubresource( g_FontTexture, 0, 0, pixelData );
        commandBuffer->GenerateMips( g_FontTexture );

        auto fence = commandQueue->Submit( commandBuffer );
        fence->WaitFor();
    }

    std::string guiVSSource = Core::GetStringResource( GUI_VS, "Shader" );
    std::string guiPSSource = Core::GetStringResource( GUI_PS, "Shader" );

    // We need the DX12 version of the shader so we can override the input layout.
    auto vertexShader = std::dynamic_pointer_cast<ShaderDX12>( device->CreateShader() );
    vertexShader->LoadShaderFromString( ShaderType::Vertex, guiVSSource, L"GUI_VS.hlsl", "VS_gui" );

    // ImGui uses a different input layout than what is specified in the shader.
    // The vertex shader will try to determine the input layout for the shader using shader reflection.
    // The vertex data from ImGui doesn't match the input layout that is specified in the shader so it needs to be overridden.
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, offsetof(ImDrawVert, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, offsetof(ImDrawVert, uv),  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offsetof(ImDrawVert, col), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    vertexShader->SetInputLayout( inputLayout );

    auto pixelShader = device->CreateShader();
    pixelShader->LoadShaderFromString( ShaderType::Pixel, guiPSSource, L"GUI_PS.hlsl", "PS_gui" );

    g_PipelineState = device->CreateGraphicsPipelineState();

    g_PipelineState->SetShader( ShaderType::Vertex, vertexShader );
    g_PipelineState->SetShader( ShaderType::Pixel, pixelShader );
    g_PipelineState->SetRenderTarget( window->GetRenderTarget() );

    BlendMode alphaBlend( true, false,
            BlendFactor::SrcAlpha,
            BlendFactor::OneMinusSrcAlpha,
            BlendOperation::Add,
            BlendFactor::OneMinusSrcAlpha,
            BlendFactor::Zero,
            BlendOperation::Add
        );

    DepthMode disableDepth( false );

    g_PipelineState->GetBlendState().SetBlendMode( alphaBlend );
    g_PipelineState->GetDepthStencilState().SetDepthMode( disableDepth );
    g_PipelineState->GetRasterizerState().SetCullMode( CullMode::None );
    g_PipelineState->GetRasterizerState().SetScissorEnabled( true );
    g_PipelineState->GetRasterizerState().SetMultisampleEnabled( true );
}

void GUI::RenderGUI( std::shared_ptr<GraphicsCommandBuffer> commandBuffer )
{
    if ( !commandBuffer ) return;

    GPU_MARKER( __FUNCTION__, commandBuffer );

    ImGui::Render();

    ImGuiIO& io = ImGui::GetIO();

    Viewport vp( 0, 0, io.DisplaySize.x, io.DisplaySize.y );
    g_PipelineState->GetRasterizerState().SetViewport( vp );

    commandBuffer->BindGraphicsShaderSignature( g_PipelineState->GetShaderSignature() );

    // Setup projection matrix.
    {
        glm::mat4 projectionMatrix = glm::ortho<float>( 0, io.DisplaySize.x, io.DisplaySize.y, 0 );
        commandBuffer->BindGraphicsDynamicConstantBuffer( 0, projectionMatrix );
    }

    // Bind the font texture to slot 1 of the root signature.
    commandBuffer->BindGraphicsShaderArguments( 1, 0, { g_FontTexture } );

    ImDrawData* drawData = ImGui::GetDrawData();
    for ( int i = 0; i < drawData->CmdListsCount; ++i )
    {
        const ImDrawList* drawList = drawData->CmdLists[i];

        commandBuffer->BindDynamicVertexBuffer( 0, drawList->VtxBuffer.size(), sizeof( ImDrawVert ), drawList->VtxBuffer.Data );
        commandBuffer->BindDynamicIndexBuffer( drawList->IdxBuffer.size(), sizeof( ImDrawIdx ), drawList->IdxBuffer.Data );

        int indexOffset = 0;

        for ( int j = 0; j < drawList->CmdBuffer.size(); ++j )
        {
            const ImDrawCmd& drawCmd = drawList->CmdBuffer[j];
            if ( drawCmd.UserCallback )
            {
                drawCmd.UserCallback( drawList, &drawCmd );
            }
            else
            {
                Rect clipRect( drawCmd.ClipRect.x, drawCmd.ClipRect.y,
                                drawCmd.ClipRect.z - drawCmd.ClipRect.x, drawCmd.ClipRect.w - drawCmd.ClipRect.y );

                if ( clipRect.Width > 1.0f && clipRect.Height > 1.0f )  // Clipping rectangle must be at least 1 px
                {
                    g_PipelineState->GetRasterizerState().SetScissorRect( clipRect );

                    commandBuffer->BindGraphicsPipelineState( g_PipelineState );
                    commandBuffer->DrawIndexed( drawCmd.ElemCount, indexOffset );
                }
            }
            indexOffset += drawCmd.ElemCount;
        }
    }
}

void GUI::Shutdown()
{
    g_FontTexture.reset();
    g_PipelineState.reset();
}