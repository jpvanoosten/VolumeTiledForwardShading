#include <EnginePCH.h>

#include <Graphics/DX12/GraphicsPipelineStateDX12.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>
#include <Graphics/DX12/ShaderDX12.h>
#include <Graphics/DX12/RenderTargetDX12.h>
#include <Graphics/DX12/ShaderSignatureDX12.h>

using namespace Graphics;
using namespace Microsoft::WRL;

D3D12_PRIMITIVE_TOPOLOGY_TYPE TranslatePrimitiveToplogyType( PrimitiveTopology primTopology )
{
    D3D12_PRIMITIVE_TOPOLOGY_TYPE d3d12PrimType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;

    switch( primTopology )
    {
    case PrimitiveTopology::PointList:
        d3d12PrimType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        break;
    case PrimitiveTopology::LineList:
    case PrimitiveTopology::LineStrip:
    case PrimitiveTopology::LineListAdj:
    case PrimitiveTopology::LineStripAdj:
        d3d12PrimType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        break;
    case PrimitiveTopology::TriangleList:
    case PrimitiveTopology::TriangleStrip:
    case PrimitiveTopology::TriangleListAdj:
    case PrimitiveTopology::TriangleStripAdj:
        d3d12PrimType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        break;
    case PrimitiveTopology::PatchList:
        d3d12PrimType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
        break;
    }

    return d3d12PrimType;
}

D3D12_PRIMITIVE_TOPOLOGY TranslatePrimitiveToplogy( PrimitiveTopology primToplogy, uint32_t patchControlPoints )
{
    D3D_PRIMITIVE_TOPOLOGY d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;

    switch ( primToplogy )
    {
    case PrimitiveTopology::PointList:
        d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        break;
    case PrimitiveTopology::LineList:
        d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
    case PrimitiveTopology::LineStrip:
        d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        break;
    case PrimitiveTopology::TriangleList:
        d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
    case PrimitiveTopology::TriangleStrip:
        d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        break;
    case PrimitiveTopology::LineListAdj:
        d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
        break;
    case PrimitiveTopology::LineStripAdj:
        d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
        break;
    case PrimitiveTopology::TriangleListAdj:
        d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
        break;
    case PrimitiveTopology::TriangleStripAdj:
        d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
        break;
    case PrimitiveTopology::PatchList:
        d3dPrimitiveTopology = static_cast<D3D_PRIMITIVE_TOPOLOGY>( D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST + ( patchControlPoints - 1 ) );
        break;
    }

    return d3dPrimitiveTopology;
}

D3D12_INDEX_BUFFER_STRIP_CUT_VALUE TranslatePrimitiveRestart( PrimitiveRestart primRestart )
{
    D3D12_INDEX_BUFFER_STRIP_CUT_VALUE d3d12StripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

    switch ( primRestart )
    {
    case PrimitiveRestart::Disabled:
        d3d12StripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
        break;
    case PrimitiveRestart::Enabled16Bit:
        d3d12StripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF;
        break;
    case PrimitiveRestart::Enabled32Bit:
        d3d12StripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF;
        break;
    }

    return d3d12StripCutValue;
}

GraphicsPipelineStateDX12::GraphicsPipelineStateDX12( std::shared_ptr<DeviceDX12> device )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
    , m_PrimitiveTopology( PrimitiveTopology::TriangleList )
    , m_PatchControlPoints( 1 ) // Not using patch primitives so this value doesn't mater.
    , m_PrimitiveRestart( PrimitiveRestart::Disabled )
    , m_IsDirty( true )
{

}

GraphicsPipelineStateDX12::~GraphicsPipelineStateDX12()
{}

void GraphicsPipelineStateDX12::SetShader( ShaderType type, std::shared_ptr<Shader> shader )
{
    m_Shaders[type] = shader;

    if ( shader )
    {
        m_Connections.push_back( shader->FileChanged += boost::bind( &GraphicsPipelineStateDX12::OnFileChanged, this, _1 ) );

        if ( !m_ShaderSignature )
        {
            m_ShaderSignature = std::dynamic_pointer_cast<ShaderSignatureDX12>( shader->GetShaderSignature() );
        }
    }

    m_IsDirty = true;
}

std::shared_ptr<Shader> GraphicsPipelineStateDX12::GetShader( ShaderType type ) const
{
    ShaderMap::const_iterator iter = m_Shaders.find( type );
    if ( iter != m_Shaders.end() )
    {
        return iter->second;
    }

    return nullptr;
}

const ShaderMap& GraphicsPipelineStateDX12::GetShaders() const
{
    return m_Shaders;
}

void GraphicsPipelineStateDX12::SetShaderSignature( std::shared_ptr<ShaderSignature> shaderSignature )
{
    m_ShaderSignature = std::dynamic_pointer_cast<ShaderSignatureDX12>( shaderSignature );

    m_IsDirty = true;
}

std::shared_ptr<ShaderSignature> GraphicsPipelineStateDX12::GetShaderSignature() const
{
    return m_ShaderSignature;
}

void GraphicsPipelineStateDX12::SetRasterizerState( const RasterizerState& rasterizerState )
{
    m_RasterizerState = dynamic_cast<const RasterizerStateDX12&>( rasterizerState );

    m_IsDirty = true;
}

RasterizerState& GraphicsPipelineStateDX12::GetRasterizerState()
{
    return m_RasterizerState;
}

void GraphicsPipelineStateDX12::SetDepthStencilState( const DepthStencilState& depthStencilState )
{
    m_DepthStencilState = dynamic_cast<const DepthStencilStateDX12&>( depthStencilState );

    m_IsDirty = true;
}

DepthStencilState& GraphicsPipelineStateDX12::GetDepthStencilState() 
{
    return m_DepthStencilState;
}

void GraphicsPipelineStateDX12::SetBlendState( const BlendState& blendState )
{
    m_BlendState = dynamic_cast<const BlendStateDX12&>( blendState );

    m_IsDirty = true;
}

BlendState& GraphicsPipelineStateDX12::GetBlendState()
{
    return m_BlendState;
}

void GraphicsPipelineStateDX12::SetRenderTarget( std::shared_ptr<RenderTarget> renderTarget )
{
    m_RenderTarget = std::dynamic_pointer_cast<RenderTargetDX12>( renderTarget );

    if ( m_RenderTarget )
    {
        m_Connections.push_back( m_RenderTarget->AttachmentChanged += boost::bind( &GraphicsPipelineStateDX12::OnAttachmentChanged, this, _1 ) );
    }
    
    m_IsDirty = true;
}

std::shared_ptr<RenderTarget> GraphicsPipelineStateDX12::GetRenderTarget()
{
    return m_RenderTarget;
}

void GraphicsPipelineStateDX12::SetPrimitiveTopology( PrimitiveTopology primTopology )
{
    m_PrimitiveTopology = primTopology;

    m_IsDirty = true;
}

PrimitiveTopology GraphicsPipelineStateDX12::GetPrimitiveTopology() const
{
    return m_PrimitiveTopology;
}

void GraphicsPipelineStateDX12::SetPatchControlPoints( uint32_t patchControlPoints )
{
    m_PatchControlPoints = std::max( patchControlPoints, 1u );

    m_IsDirty = true;
}

uint32_t GraphicsPipelineStateDX12::GetPatchControlPoints() const
{
    return m_PatchControlPoints;
}

void GraphicsPipelineStateDX12::SetPrimitiveRestart( PrimitiveRestart primitiveRestart )
{
    m_PrimitiveRestart = primitiveRestart;

    m_IsDirty = true;
}

PrimitiveRestart GraphicsPipelineStateDX12::GetPrimitiveRestart() const
{
    return m_PrimitiveRestart;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> GraphicsPipelineStateDX12::GetD3D12PipelineState() const
{
    return m_d3d12PipelineState;
}

void GraphicsPipelineStateDX12::Bind( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer )
{
    ComPtr<ID3D12GraphicsCommandList> d3d12CommandList = commandBuffer->GetD3D12CommandList();

    if ( m_IsDirty || m_RasterizerState.IsDirty() || m_DepthStencilState.IsDirty() || m_BlendState.IsDirty() )
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d12PipelineStateDesc = {};

        for ( auto shader : m_Shaders )
        {
            std::shared_ptr<ShaderDX12> shaderDX12 = std::dynamic_pointer_cast<ShaderDX12>( shader.second );
            if ( shaderDX12 )
            {
                switch ( shader.first )
                {
                case ShaderType::Vertex:
                {
                    d3d12PipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE( shaderDX12->GetD3DShaderBlob().Get() );

                    if ( !m_ShaderSignature )
                    {
                        // Use the root signature defined in the vertex shader if no signature has been 
                        // assigned to the pipeline state.
                        m_ShaderSignature = std::dynamic_pointer_cast<ShaderSignatureDX12>( shaderDX12->GetShaderSignature() );
                    }

                    if ( m_ShaderSignature && ( m_ShaderSignature->GetD3D12RootSignatureFlags() & D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT ) != 0 )
                    {
                        const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout = shaderDX12->GetInputLayout();
                        d3d12PipelineStateDesc.InputLayout = { inputLayout.data(), static_cast<UINT>( inputLayout.size() ) };
                    }
                }
                break;
                case ShaderType::TessellationControl:
                    d3d12PipelineStateDesc.HS = CD3DX12_SHADER_BYTECODE( shaderDX12->GetD3DShaderBlob().Get() );
                    break;
                case ShaderType::TessellationEvaluation:
                    d3d12PipelineStateDesc.DS = CD3DX12_SHADER_BYTECODE( shaderDX12->GetD3DShaderBlob().Get() );
                    break;
                case ShaderType::Geometry:
                    d3d12PipelineStateDesc.GS = CD3DX12_SHADER_BYTECODE( shaderDX12->GetD3DShaderBlob().Get() );
                    break;
                case ShaderType::Pixel:
                    d3d12PipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE( shaderDX12->GetD3DShaderBlob().Get() );
                    break;
                }
            }
        }

        d3d12PipelineStateDesc.pRootSignature = m_ShaderSignature ? m_ShaderSignature->GetD3D12RootSignature().Get() : nullptr;
        d3d12PipelineStateDesc.RasterizerState = m_RasterizerState.GetD3D12RasterizerDescription();
        d3d12PipelineStateDesc.DepthStencilState = m_DepthStencilState.GetD3D12DepthStencilDesc();
        d3d12PipelineStateDesc.BlendState = m_BlendState.GetD3D12BlendDesc();
        d3d12PipelineStateDesc.IBStripCutValue = TranslatePrimitiveRestart( m_PrimitiveRestart );
        d3d12PipelineStateDesc.PrimitiveTopologyType = TranslatePrimitiveToplogyType( m_PrimitiveTopology );
        d3d12PipelineStateDesc.SampleMask = m_BlendState.GetSampleMask();
        d3d12PipelineStateDesc.SampleDesc = { 1, 0 };
        
        if ( m_RenderTarget )
        {
            d3d12PipelineStateDesc.SampleDesc = m_RenderTarget->GetSampleDesc();

            std::vector<DXGI_FORMAT> rtvFormats = m_RenderTarget->GetRTVFormats();
            d3d12PipelineStateDesc.NumRenderTargets = static_cast<UINT>( rtvFormats.size() );

            size_t i = 0;
            for ( auto dxgiFormat : rtvFormats )
            {
                d3d12PipelineStateDesc.RTVFormats[i++] = dxgiFormat;
            }

            d3d12PipelineStateDesc.DSVFormat = m_RenderTarget->GetDSVFormat();
        }

        m_d3d12Device->CreateGraphicsPipelineState( &d3d12PipelineStateDesc, IID_PPV_ARGS( &m_d3d12PipelineState ) );

        m_IsDirty = false;
    }

    d3d12CommandList->SetPipelineState( m_d3d12PipelineState.Get() );
    if ( m_ShaderSignature )
    {
        commandBuffer->BindGraphicsShaderSignature( m_ShaderSignature );
    }

    if ( m_RenderTarget )
    {
        commandBuffer->BindRenderTarget( m_RenderTarget );
    }

    D3D12_PRIMITIVE_TOPOLOGY d3d12PrimitiveToplogy = TranslatePrimitiveToplogy( m_PrimitiveTopology, m_PatchControlPoints );
    d3d12CommandList->IASetPrimitiveTopology( d3d12PrimitiveToplogy );

    m_DepthStencilState.Bind( commandBuffer );
    m_RasterizerState.Bind( commandBuffer );
    m_BlendState.Bind( commandBuffer );
}

void GraphicsPipelineStateDX12::OnFileChanged( Core::FileChangeEventArgs& e )
{
    // Recreate the PSO if one of the dependent shaders has been modified.
    m_IsDirty = true;
}

void GraphicsPipelineStateDX12::OnAttachmentChanged( Core::EventArgs& e )
{
    // If any of the attachments of the render target change,
    // then the PSO needs to be recreated.
    m_IsDirty = true;
}
