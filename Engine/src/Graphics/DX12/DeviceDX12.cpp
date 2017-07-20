#include <EnginePCH.h>

#include <Common.h>
#include <Application.h>
#include <Graphics/DXGI/AdapterDXGI.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/RenderTargetDX12.h>
#include <Graphics/DX12/GraphicsCommandQueueDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>
#include <Graphics/DX12/GraphicsPipelineStateDX12.h>
#include <Graphics/DX12/ComputePipelineStateDX12.h>
#include <Graphics/DX12/IndirectCommandSignatureDX12.h>
#include <Graphics/DX12/DescriptorAllocatorDX12.h>
#include <Graphics/DX12/ConstantBufferDX12.h>
#include <Graphics/DX12/ByteAddressBufferDX12.h>
#include <Graphics/DX12/StructuredBufferDX12.h>
#include <Graphics/DX12/VertexBufferDX12.h>
#include <Graphics/DX12/IndexBufferDX12.h>
#include <Graphics/DX12/ReadbackBufferDX12.h>
#include <Graphics/DX12/TextureDX12.h>
#include <Graphics/DX12/SamplerDX12.h>
#include <Graphics/DX12/ShaderDX12.h>
#include <Graphics/DX12/ShaderSignatureDX12.h>
#include <Graphics/DX12/SceneDX12.h>
#include <Graphics/DX12/QueueSemaphoreDX12.h>
#include <Graphics/DX12/QueryDX12.h>
#include <Graphics/Mesh.h>
#include <Graphics/Material.h>

#include <LogManager.h>

using namespace Graphics;
using namespace Microsoft::WRL;

DeviceDX12::DeviceDX12( Microsoft::WRL::ComPtr<ID3D12Device> device, std::shared_ptr<AdapterDXGI> adapter )
    : m_d3d12Device( device )
    , m_Adapter( adapter )
    , m_NodeCount( m_d3d12Device->GetNodeCount() )
{
    std::wstring deviceName( L"ID3D12Device" );
    if ( adapter )
    {
        deviceName += L"(" + adapter->GetDescription() + L")";
    }

    m_d3d12Device->SetName( deviceName.c_str() );

    m_d3d12Device->CheckFeatureSupport( D3D12_FEATURE_D3D12_OPTIONS, &m_FeatureOptions, sizeof( D3D12_FEATURE_DATA_D3D12_OPTIONS ) );

    for ( int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i )
    {
        m_DescriptorAllocators[i] = std::make_unique<DescriptorAllocatorDX12>( m_d3d12Device, static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>( i ) );
    }
}

DeviceDX12::~DeviceDX12()
{
    IdleGPU();
}

void DeviceDX12::IdleGPU()
{
    m_CopyQueue->Idle();
    m_ComputeQueue->Idle();
    m_GraphicsQueue->Idle();
}

std::shared_ptr<Adapter> DeviceDX12::GetAdapter() const
{
    return m_Adapter.lock();
}

std::shared_ptr<GraphicsCommandQueue> DeviceDX12::GetGraphicsQueue() const
{
    return m_GraphicsQueue;
}

std::shared_ptr<ComputeCommandQueue> DeviceDX12::GetComputeQueue() const
{
    return m_ComputeQueue;
}

std::shared_ptr<CopyCommandQueue> DeviceDX12::GetCopyQueue() const
{
    return m_CopyQueue;
}

Microsoft::WRL::ComPtr<ID3D12Device> DeviceDX12::GetD3D12Device() const
{
    return m_d3d12Device;
}

std::shared_ptr<QueueSemaphore> DeviceDX12::CreateQueueSemaphore()
{
    return std::make_shared<QueueSemaphoreDX12>( shared_from_this() );
}

std::shared_ptr<Mesh> DeviceDX12::CreateMesh()
{
    return std::make_shared<Mesh>( shared_from_this() );
}

std::shared_ptr<Material> DeviceDX12::CreateMaterial()
{
    return std::make_shared<Material>( shared_from_this() );
}

std::shared_ptr<Scene> DeviceDX12::CreateScene()
{
    return std::make_shared<SceneDX12>( shared_from_this() );
}

std::shared_ptr<Scene> DeviceDX12::CreateSphere( std::shared_ptr<ComputeCommandBuffer> commandBuffer, float radius, float tesselation )
{
    std::shared_ptr<Scene> scene = CreateScene();
    std::stringstream ss;
    // Create a white diffuse material for the sphere.
    // f red green blue Kd Ks Shine transmittance indexOfRefraction
    ss << "f 1 1 1 1 0 0 0 0" << std::endl;

    // tess tesselation
    ss << "tess " << tesselation << std::endl;
    // s x y z radius
    ss << "s 0 0 0 " << radius << std::endl;

    if ( scene->LoadFromString( commandBuffer, ss.str(), "nff" ) )
    {
        return scene;
    }

    return nullptr;
}

std::shared_ptr<Scene> DeviceDX12::CreateCylinder( std::shared_ptr<ComputeCommandBuffer> commandBuffer, float baseRadius, float apexRadius, float height, const glm::vec3& axis )
{
    std::shared_ptr<Scene> scene = CreateScene();
    std::stringstream ss;

    // Create a white diffuse material for the cylinder.
    // f red green blue Kd Ks Shine transmittance indexOfRefraction
    ss << "f 1 1 1 1 0 0 0 0" << std::endl;

    ss << "c" << std::endl;
    // base.x base.y base.z baseRadius
    ss << "0 0 0 " << baseRadius << std::endl;

    glm::vec3 apex = axis * height;
    // apex.x apex.y apex.z apexRadius
    ss << apex.x << " " << apex.y << " " << apex.z << " " << apexRadius << std::endl;

    if ( scene->LoadFromString( commandBuffer, ss.str(), "nff" ) )
    {
        return scene;
    }

    // An error occurred while loading the scene.
    return nullptr;
}

std::shared_ptr<Scene> DeviceDX12::CreateScreenQuad( std::shared_ptr<ComputeCommandBuffer> commandBuffer, float left, float right, float bottom, float top, float z )
{
    glm::vec3 p[4]; // Vertex position
    glm::vec3 n[4]; // Vertex normal (required for texture patch polygons)
    glm::vec2 t[4]; // Texture coordinates
                    // Winding order is assumed to be right-handed. Front-facing polygons have
                    // a counter-clockwise winding order.
                    // Assimp flips the winding order of vertices.. Don't ask me why. To account for this,
                    // the vertices are loaded in reverse order :)
    p[0] = glm::vec3( right, bottom, z );   n[0] = glm::vec3( 0, 0, 1 );    t[0] = glm::vec2( 1, 0 );
    p[1] = glm::vec3( left, bottom, z );    n[1] = glm::vec3( 0, 0, 1 );    t[1] = glm::vec2( 0, 0 );
    p[2] = glm::vec3( left, top, z );       n[2] = glm::vec3( 0, 0, 1 );    t[2] = glm::vec2( 0, 1 );
    p[3] = glm::vec3( right, top, z );      n[3] = glm::vec3( 0, 0, 1 );    t[3] = glm::vec2( 1, 1 );

    // Now create the quad.
    std::shared_ptr<Scene> scene = CreateScene();

    std::stringstream ss;

    // Create a white diffuse material for the quad.
    // f red green blue Kd Ks Shine transmittance indexOfRefraction
    ss << "f 1 1 1 1 0 0 0 0" << std::endl;

    // Create a 4-point textured polygon patch
    ss << "tpp 4" << std::endl;
    for ( int i = 0; i < 4; i++ )
    {
        // px py pz nx ny nz tu tv
        ss << p[i].x << " " << p[i].y << " " << p[i].z << " " << n[i].x << " " << n[i].y << " " << n[i].z << " " << t[i].x << " " << t[i].y << std::endl;
    }

    if ( scene->LoadFromString( commandBuffer, ss.str(), "nff" ) )
    {
        return scene;
    }

    return nullptr;
}

std::shared_ptr<ShaderSignature> DeviceDX12::CreateShaderSignature()
{
    return std::make_shared<ShaderSignatureDX12>( shared_from_this() );
}

std::shared_ptr<ConstantBuffer> DeviceDX12::CreateConstantBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, size_t bufferSize, const void* bufferData )
{
    std::shared_ptr<ConstantBuffer> constantBuffer = std::make_shared<ConstantBufferDX12>( shared_from_this() );
    copyCommandBuffer->SetConstantBuffer( constantBuffer, bufferSize, bufferData );

    return constantBuffer;
}

std::shared_ptr<StructuredBuffer> DeviceDX12::CreateStructuredBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, size_t numElements, size_t elementSize, const void* bufferData )
{
    std::shared_ptr<StructuredBuffer> structuredBuffer = std::make_shared<StructuredBufferDX12>( shared_from_this(), copyCommandBuffer );
    copyCommandBuffer->SetStructuredBuffer( structuredBuffer, numElements, elementSize, bufferData );

    return structuredBuffer;
}

std::shared_ptr<ByteAddressBuffer> DeviceDX12::CreateByteAddressBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, size_t bufferSize, const void* bufferData )
{
    std::shared_ptr<ByteAddressBuffer> byteAddressBuffer = std::make_shared<ByteAddressBufferDX12>( shared_from_this() );
    copyCommandBuffer->SetByteAddressBuffer( byteAddressBuffer, bufferSize, bufferData );

    return byteAddressBuffer;
}

std::shared_ptr<VertexBuffer> DeviceDX12::CreateVertexBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, size_t numVertices, size_t vertexStride, const void* vertexData )
{
    std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBufferDX12>( shared_from_this() );
    copyCommandBuffer->SetVertexBuffer( vertexBuffer, numVertices, vertexStride, vertexData );

    return vertexBuffer;
}

std::shared_ptr<IndexBuffer> DeviceDX12::CreateIndexBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, size_t numIndicies, size_t indexSizeInBytes, const void* indexData )
{
    std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBufferDX12>( shared_from_this() );
    copyCommandBuffer->SetIndexBuffer( indexBuffer, numIndicies, indexSizeInBytes, indexData );

    return indexBuffer;
}

std::shared_ptr<ReadbackBuffer> DeviceDX12::CreateReadbackBuffer( size_t bufferSize )
{
    std::shared_ptr<ReadbackBuffer> readbackBuffer = std::make_shared<ReadbackBufferDX12>( shared_from_this(), bufferSize );
    return readbackBuffer;
}

std::shared_ptr<Sampler> DeviceDX12::CreateSampler()
{
    std::shared_ptr<Sampler> sampler = std::make_shared<SamplerDX12>( shared_from_this() );
    return sampler;
}

std::shared_ptr<Texture> DeviceDX12::CreateTexture( std::shared_ptr<ComputeCommandBuffer> computeCommandBuffer, const std::wstring& fileName )
{
    TextureMap::const_iterator iter = m_TextureMap.find( fileName );
    if ( iter != m_TextureMap.end() )
    {
        return iter->second;
    }

    Core::Application::Get().SetLoadingMessage( fileName );

    std::shared_ptr<Texture> texture = std::make_shared<TextureDX12>( shared_from_this() );
    if ( texture->LoadTexture2D( computeCommandBuffer, fileName ) )
    {
        // Generate mipmaps for textures.
        computeCommandBuffer->GenerateMips( texture );
    }

    m_TextureMap.emplace( fileName, texture );

    return texture;
}

std::shared_ptr<Texture> DeviceDX12::CreateTexture2D( uint16_t width, uint16_t height, uint16_t slices, const TextureFormat& format )
{
    std::shared_ptr<Texture> texture = std::make_shared<TextureDX12>( shared_from_this(), width, height, slices, format );
    return texture;
}


std::shared_ptr<Shader> DeviceDX12::CreateShader()
{
    std::shared_ptr<Shader> shader = std::make_shared<ShaderDX12>( shared_from_this() );
    return shader;
}

std::shared_ptr<Query> DeviceDX12::CreateQuery( QueryType queryType, uint32_t numQueries )
{
    return std::make_shared<QueryDX12>( shared_from_this(), queryType, numQueries );
}

D3D12_CPU_DESCRIPTOR_HANDLE DeviceDX12::AllocateDescriptors( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors )
{
    return m_DescriptorAllocators[type]->Allocate( numDescriptors );
}

std::shared_ptr<GraphicsPipelineState> DeviceDX12::CreateGraphicsPipelineState()
{
    std::shared_ptr<GraphicsPipelineState> graphicsPipelineState = std::make_shared<GraphicsPipelineStateDX12>( shared_from_this() );
    return graphicsPipelineState;
}

std::shared_ptr<ComputePipelineState> DeviceDX12::CreateComputePipelineState()
{
    std::shared_ptr<ComputePipelineState> computePipelineState = std::make_shared<ComputePipelineStateDX12>( shared_from_this() );
    return computePipelineState;
}

std::shared_ptr<IndirectCommandSignature> DeviceDX12::CreateIndirectCommandSignature()
{
    std::shared_ptr<IndirectCommandSignature> indirectCommandSignature = std::make_shared<IndirectCommandSignatureDX12>( shared_from_this() );

    return indirectCommandSignature;
}

std::shared_ptr<RenderTarget> DeviceDX12::CreateRenderTarget()
{
    std::shared_ptr<RenderTarget> renderTarget = std::make_shared<RenderTargetDX12>( shared_from_this() );
    return renderTarget;
}

void DeviceDX12::Init()
{
    m_GraphicsQueue = std::make_shared<GraphicsCommandQueueDX12>( shared_from_this(), D3D12_COMMAND_LIST_TYPE_DIRECT );
    m_ComputeQueue = std::make_shared<GraphicsCommandQueueDX12>( shared_from_this(), D3D12_COMMAND_LIST_TYPE_COMPUTE );
    m_CopyQueue = std::make_shared<GraphicsCommandQueueDX12>( shared_from_this(), D3D12_COMMAND_LIST_TYPE_COPY );

    // Load the variations of the GenerateMips compute shader.
    std::string generateMipsShader = Core::GetStringResource( GENERATEMIPS_CS, "Shader" );

    ShaderMacros macros;
    macros["TEXTURE_DIMENSION"] = "WIDTH_HEIGHT_EVEN";
    auto generateMipsCS = CreateShader();
    generateMipsCS->LoadShaderFromString( ShaderType::Compute, generateMipsShader, L"GenerateMipsCS.hlsl", "main", macros );

    m_GenerateMipsPSO[GenerateMips::WidthHeightEven] = std::dynamic_pointer_cast<ComputePipelineStateDX12>( CreateComputePipelineState() );
    m_GenerateMipsPSO[GenerateMips::WidthHeightEven]->SetShader( generateMipsCS );

    // Width is odd, height is even.
    macros["TEXTURE_DIMENSION"] = "WIDTH_ODD_HEIGHT_EVEN";
    generateMipsCS = CreateShader();
    generateMipsCS->LoadShaderFromString( ShaderType::Compute, generateMipsShader, L"GenerateMipsCS.hlsl", "main", macros );

    m_GenerateMipsPSO[GenerateMips::WidthOddHeightEven] = std::dynamic_pointer_cast<ComputePipelineStateDX12>( CreateComputePipelineState() );
    m_GenerateMipsPSO[GenerateMips::WidthOddHeightEven]->SetShader( generateMipsCS );

    // Width is even, height is odd.
    macros["TEXTURE_DIMENSION"] = "WIDTH_EVEN_HEIGHT_ODD";
    generateMipsCS = CreateShader();
    generateMipsCS->LoadShaderFromString( ShaderType::Compute, generateMipsShader, L"GenerateMipsCS.hlsl", "main", macros );

    m_GenerateMipsPSO[GenerateMips::WidthEvenHeightOdd] = std::dynamic_pointer_cast<ComputePipelineStateDX12>( CreateComputePipelineState() );
    m_GenerateMipsPSO[GenerateMips::WidthEvenHeightOdd]->SetShader( generateMipsCS );

    // Both width and height are odd.
    macros["TEXTURE_DIMENSION"] = "WIDTH_HEIGHT_ODD";
    generateMipsCS = CreateShader();
    generateMipsCS->LoadShaderFromString( ShaderType::Compute, generateMipsShader, L"GenerateMipsCS.hlsl", "main", macros );

    m_GenerateMipsPSO[GenerateMips::WidthHeightOdd] = std::dynamic_pointer_cast<ComputePipelineStateDX12>( CreateComputePipelineState() );
    m_GenerateMipsPSO[GenerateMips::WidthHeightOdd]->SetShader( generateMipsCS );
}

std::shared_ptr<ComputePipelineStateDX12> DeviceDX12::GetMipsPipelineState( uint32_t variation ) const
{
    assert( variation >= 0 && variation < GenerateMips::NumVariations );

    return m_GenerateMipsPSO[variation];
}

DXGI_SAMPLE_DESC DeviceDX12::GetMultisampleQualityLevels( DXGI_FORMAT format, UINT numSamples, D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags ) const
{
    DXGI_SAMPLE_DESC sampleDesc = { 1, 0 };

    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels;
    qualityLevels.Format = format;
    qualityLevels.SampleCount = 1;
    qualityLevels.Flags = flags;
    qualityLevels.NumQualityLevels = 0;

    while ( qualityLevels.SampleCount <= numSamples && SUCCEEDED( m_d3d12Device->CheckFeatureSupport( D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels, sizeof( D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS ) ) ) && qualityLevels.NumQualityLevels > 0 )
    {
        // That works...
        sampleDesc.Count = qualityLevels.SampleCount;
        sampleDesc.Quality = qualityLevels.NumQualityLevels - 1;

        // But can we do better?
        qualityLevels.SampleCount *= 2;
    }

    return sampleDesc;
}
