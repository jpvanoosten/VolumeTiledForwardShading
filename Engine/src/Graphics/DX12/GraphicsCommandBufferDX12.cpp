#include <EnginePCH.h>

#include <Graphics/DX12/GraphicsCommandBufferDX12.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/FenceDX12.h>
#include <Graphics/DX12/GraphicsCommandQueueDX12.h>
#include <Graphics/DX12/GraphicsPipelineStateDX12.h>
#include <Graphics/DX12/ComputePipelineStateDX12.h>
#include <Graphics/DX12/ShaderSignatureDX12.h>
#include <Graphics/DX12/ResourceDX12.h>
#include <Graphics/DX12/ConstantBufferDX12.h>
#include <Graphics/DX12/ByteAddressBufferDX12.h>
#include <Graphics/DX12/StructuredBufferDX12.h>
#include <Graphics/DX12/VertexBufferDX12.h>
#include <Graphics/DX12/IndexBufferDX12.h>
#include <Graphics/DX12/TextureDX12.h>
#include <Graphics/DX12/SamplerDX12.h>
#include <Graphics/DXGI/TextureFormatDXGI.h>
#include <Graphics/DX12/RenderTargetDX12.h>
#include <Graphics/DX12/HeapAllocatorDX12.h>
#include <Graphics/DX12/DynamicDescriptorHeapDX12.h>
#include <Graphics/DX12/QueryDX12.h>
#include <Graphics/DX12/IndirectCommandSignatureDX12.h>

#include <Graphics/Profiler.h>
#include <Graphics/ShaderParameter.h>

#include <LogManager.h>

using namespace Graphics;
using namespace Microsoft::WRL;

D3D12_RESOURCE_STATES ConvertResourceState( ResourceState state )
{
    D3D12_RESOURCE_STATES d3d12ResourceState = static_cast<D3D12_RESOURCE_STATES>( 0 );

    if ( ( state & ResourceState::VertexBuffer ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    }
    if ( ( state & ResourceState::ConstantBuffer ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    }
    if ( ( state & ResourceState::IndexBuffer ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
    }
    if ( ( state & ResourceState::RenderTarget ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_RENDER_TARGET;
    }
    if ( ( state & ResourceState::UAV ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    }
    if ( ( state & ResourceState::DepthWrite ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
    }
    if ( ( state & ResourceState::DepthRead ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_DEPTH_READ;
    }
    if ( ( state & ResourceState::NonPixelShader ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    }
    if ( ( state & ResourceState::PixelShader ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    }
    if ( ( state & ResourceState::StreamOut ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_STREAM_OUT;
    }
    if ( ( state & ResourceState::IndirectArgument ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
    }
    if ( ( state & ResourceState::CopyDest ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_COPY_DEST;
    }
    if ( ( state & ResourceState::CopySrc ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_COPY_SOURCE;
    }
    if ( ( state & ResourceState::ResolveDest ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
    }
    if ( ( state & ResourceState::ResolveSrc ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
    }
    if ( ( state & ResourceState::Present ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_PRESENT;
    }
    if ( ( state & ResourceState::Predication ) != 0 )
    {
        d3d12ResourceState |= D3D12_RESOURCE_STATE_PREDICATION;
    }

    return d3d12ResourceState;
}

static uint32_t gs_NumCopyCommandBuffers = 0;
static uint32_t gs_NumComputeCommandBuffers = 0;
static uint32_t gs_NumDirectCommandBuffers = 0;
static uint32_t gs_NumBundleCommandBuffers = 0;

GraphicsCommandBufferDX12::GraphicsCommandBufferDX12( std::shared_ptr<DeviceDX12> device,
                                                      std::shared_ptr<GraphicsCommandQueueDX12> queue,
                                                      D3D12_COMMAND_LIST_TYPE type,
                                                      UINT nodeMask )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
    , m_Queue( queue )
    , m_d3d12CommandListType( type )
    , m_NodeMask( nodeMask )
{
    ComPtr<ID3D12Device> d3d12Device = device->GetD3D12Device();
    assert( d3d12Device );

    if ( FAILED( d3d12Device->CreateCommandAllocator( m_d3d12CommandListType, IID_PPV_ARGS( &m_d3d12CommandAllocator ) ) ) )
    {
        LOG_ERROR( "Failed to create command allocator." );
    } 
    else
    {
        if ( FAILED( d3d12Device->CreateCommandList( m_NodeMask, m_d3d12CommandListType, m_d3d12CommandAllocator.Get(), nullptr, IID_PPV_ARGS( &m_d3d12CommandList ) ) ) )
        {
            LOG_ERROR( "Failed to create command list." );
        }
        else
        {
            std::wstring namePostfix;
            switch ( m_d3d12CommandListType )
            {
            case D3D12_COMMAND_LIST_TYPE_COPY:
                namePostfix = L"(Copy)[" + std::to_wstring( gs_NumCopyCommandBuffers++ ) + L"]";
                break;
            case D3D12_COMMAND_LIST_TYPE_COMPUTE:
                namePostfix = L"(Compute)[" + std::to_wstring( gs_NumComputeCommandBuffers++ ) + L"]";
                break;
            case D3D12_COMMAND_LIST_TYPE_DIRECT:
                namePostfix = L"(Direct)[" + std::to_wstring( gs_NumDirectCommandBuffers++ ) + L"]";
                break;
            case D3D12_COMMAND_LIST_TYPE_BUNDLE:
                namePostfix = L"(Bundle)[" + std::to_wstring( gs_NumBundleCommandBuffers++ ) + L"]";
                break;
            }

            m_d3d12CommandList->SetName( ( std::wstring( L"ID3D12CommandList" ) + namePostfix ).c_str() );
            m_d3d12CommandAllocator->SetName( ( std::wstring( L"ID3D12CommandAllocator" ) + namePostfix ).c_str() );
        }
    }

    m_UploadHeap = std::make_shared<HeapAllocatorDX12>( device, D3D12_HEAP_TYPE_UPLOAD );

    for ( int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i )
    {
        m_DynamicDescriptorHeap[i] = std::make_unique<DynamicDescriptorHeapDX12>( m_d3d12Device, static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>( i ) );
    }
    
    // The command list is expected to be closed before 
    // it is reset. Calling End() will ensure the command list is closed.
    End();
}

GraphicsCommandBufferDX12::~GraphicsCommandBufferDX12()
{
}

std::shared_ptr<CommandQueue> GraphicsCommandBufferDX12::GetCommandQueue() const
{
    return m_Queue.lock();
}

#define VALID_COMPUTE_QUEUE_RESOURCE_STATES \
    ( D3D12_RESOURCE_STATE_UNORDERED_ACCESS \
    | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE \
    | D3D12_RESOURCE_STATE_COPY_DEST \
    | D3D12_RESOURCE_STATE_COPY_SOURCE )

void GraphicsCommandBufferDX12::TransitionResoure( std::shared_ptr<ResourceDX12> resourceDX12, ResourceState state, bool flushBarriers )
{
    D3D12_RESOURCE_STATES stateBefore = resourceDX12->m_d3d12ResourceState;
    D3D12_RESOURCE_STATES stateAfter = ConvertResourceState( state );

    if ( m_d3d12CommandListType == D3D12_COMMAND_LIST_TYPE_COMPUTE )
    {
        // Check that the before and after resource states are valid for 
        // compute command lists.
        assert( ( stateBefore & VALID_COMPUTE_QUEUE_RESOURCE_STATES ) == stateBefore );
        assert( ( stateAfter & VALID_COMPUTE_QUEUE_RESOURCE_STATES ) == stateAfter );
    }

    ID3D12Resource* pResource = resourceDX12 ? resourceDX12->m_d3d12Resource.Get() : nullptr;

    if ( pResource )
    {
        if ( stateBefore != stateAfter )
        {
            m_ResourceBarriers.push_back( CD3DX12_RESOURCE_BARRIER::Transition( pResource, stateBefore, stateAfter ) );
            resourceDX12->m_d3d12ResourceState = stateAfter;
        }
        else if ( stateAfter == D3D12_RESOURCE_STATE_UNORDERED_ACCESS )
        {
            AddResourceBarrier( resourceDX12, resourceDX12, ResourceBarrier::UAV, flushBarriers );
        }
    }

    if ( flushBarriers )
    {
        FlushResourceBarriers();
    }
}

void GraphicsCommandBufferDX12::TransitionResoure( std::shared_ptr<Resource> resource, ResourceState state, bool flushBarriers )
{
    std::shared_ptr<ResourceDX12> resourceDX12 = std::dynamic_pointer_cast<ResourceDX12>( resource );
    TransitionResoure( resourceDX12, state, flushBarriers );
}

void GraphicsCommandBufferDX12::AddResourceBarrier( std::shared_ptr<ResourceDX12> resourceBeforeDX12, std::shared_ptr<ResourceDX12> resourceAfterDX12, ResourceBarrier barrier, bool flushBarriers )
{
    ID3D12Resource* pResourceBefore = resourceBeforeDX12 ? resourceBeforeDX12->m_d3d12Resource.Get() : nullptr;
    ID3D12Resource* pResourceAfter = resourceAfterDX12 ? resourceAfterDX12->m_d3d12Resource.Get() : nullptr;

    switch ( barrier )
    {
    case ResourceBarrier::UAV:
        m_ResourceBarriers.push_back( CD3DX12_RESOURCE_BARRIER::UAV( pResourceBefore ) );
        break;
    case ResourceBarrier::Aliasing:
        m_ResourceBarriers.push_back( CD3DX12_RESOURCE_BARRIER::Aliasing( pResourceBefore, pResourceAfter ) );
        break;
    }

    if ( flushBarriers )
    {
        FlushResourceBarriers();
    }
}

void GraphicsCommandBufferDX12::AddResourceBarrier( std::shared_ptr<Resource> resourceBefore, std::shared_ptr<Resource> resourceAfter, ResourceBarrier barrier, bool flushBarriers )
{
    std::shared_ptr<ResourceDX12> resourceBeforeDX12 = std::dynamic_pointer_cast<ResourceDX12>( resourceBefore );
    std::shared_ptr<ResourceDX12> resourceAfterDX12 = std::dynamic_pointer_cast<ResourceDX12>( resourceAfter );
    AddResourceBarrier( resourceBeforeDX12, resourceAfterDX12, barrier, flushBarriers );
}

void GraphicsCommandBufferDX12::AddUAVBarrier( std::shared_ptr<Resource> uavResource, bool flushBarriers )
{
    std::shared_ptr<ResourceDX12> uavResourceDX12 = std::dynamic_pointer_cast<ResourceDX12>( uavResource );
    AddResourceBarrier( uavResourceDX12, nullptr, ResourceBarrier::UAV, flushBarriers );
}


void GraphicsCommandBufferDX12::BeginQuery( std::shared_ptr<Query> query, uint32_t index )
{
    std::shared_ptr<QueryDX12> queryDX12 = std::dynamic_pointer_cast<QueryDX12>( query );
    assert( queryDX12 );
    assert( index < query->GetQueryCount() );

    ID3D12QueryHeap* queryHeap = queryDX12->GetD3D12QueryHeap().Get();
    D3D12_QUERY_TYPE queryType = queryDX12->GetD3D12QueryType();

    switch ( queryType )
    {
    case D3D12_QUERY_TYPE_TIMESTAMP:
        m_d3d12CommandList->EndQuery( queryHeap, queryType, index * 2 );
        break;
    case D3D12_QUERY_TYPE_OCCLUSION:
    case D3D12_QUERY_TYPE_BINARY_OCCLUSION:
        m_d3d12CommandList->BeginQuery( queryHeap, queryType, index );
        break;
    }
}

void GraphicsCommandBufferDX12::EndQuery( std::shared_ptr<Query> query, uint32_t index )
{
    std::shared_ptr<QueryDX12> queryDX12 = std::dynamic_pointer_cast<QueryDX12>( query );
    assert( queryDX12 );
    assert( index < query->GetQueryCount() );

    ID3D12QueryHeap* queryHeap = queryDX12->GetD3D12QueryHeap().Get();
    ID3D12Resource* queryResource = queryDX12->GetD3D12QueryResource().Get();
    D3D12_QUERY_TYPE queryType = queryDX12->GetD3D12QueryType();

    m_ReferencedObjects.push_back( queryHeap );
    m_ReferencedObjects.push_back( queryResource );

    switch ( queryType )
    {
    case D3D12_QUERY_TYPE_TIMESTAMP:
        m_d3d12CommandList->EndQuery( queryHeap, queryType, index * 2 + 1 );
        m_d3d12CommandList->ResolveQueryData( queryHeap, queryType, index * 2, 2, queryResource, index * 2 * sizeof(uint64_t) );
        break;
    case D3D12_QUERY_TYPE_OCCLUSION:
    case D3D12_QUERY_TYPE_BINARY_OCCLUSION:
        m_d3d12CommandList->EndQuery( queryHeap, queryType, index );
        m_d3d12CommandList->ResolveQueryData( queryHeap, queryType, index, 1, queryResource, index * sizeof(uint64_t) );
        break;
    }
}

void GraphicsCommandBufferDX12::BeginProfilingEvent( const std::wstring& name )
{
#if defined(USE_PIX)
    ::PIXBeginEvent( m_d3d12CommandList.Get(), 0, name.c_str() );
#endif
}

void GraphicsCommandBufferDX12::EndProfilingEvent( const std::wstring& name )
{
#if defined(USE_PIX)
    ::PIXEndEvent( m_d3d12CommandList.Get() );
#endif
}

void GraphicsCommandBufferDX12::FlushResourceBarriers()
{
    // Commit the resource barriers
    if ( !m_ResourceBarriers.empty() )
    {
        m_d3d12CommandList->ResourceBarrier( static_cast<UINT>( m_ResourceBarriers.size() ), m_ResourceBarriers.data() );
        m_ResourceBarriers.clear();
    }
}

void GraphicsCommandBufferDX12::ReleaseReferences()
{
    m_ReferencedObjects.clear();
}

void GraphicsCommandBufferDX12::Begin()
{
    // This should not fail if the command allocator is not in use.
    if ( FAILED( m_d3d12CommandAllocator->Reset() ) )
    {
        LOG_ERROR( "Failed to reset command allocator" );
    }
    else
    {
        // It is always safe to reset the command list after it has been submitted
        // to the command queue.
        if ( FAILED( m_d3d12CommandList->Reset( m_d3d12CommandAllocator.Get(), nullptr ) ) )
        {
            LOG_ERROR( "Failed to reset command list." );
        }
    }

    ReleaseReferences();

    m_UploadHeap->Free();

    for ( int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i )
    {
        m_DescriptorHeaps[i] = nullptr;
        m_DynamicDescriptorHeap[i]->Free();
    }

    m_pCurrentComputeRootSignature = nullptr;
    m_pCurrentGraphicsRootSignature = nullptr;

    m_pCurrentComputeShaderSignature = nullptr;
    m_pCurrentGraphicsShaderSignature = nullptr;
}

void GraphicsCommandBufferDX12::End()
{
    FlushResourceBarriers();

    // Close the command list.
    m_d3d12CommandList->Close();
}

void GraphicsCommandBufferDX12::ClearResourceFloat( std::shared_ptr<Resource> resource, const glm::vec4& clearValues )
{
    std::shared_ptr<ResourceDX12> resourceDX12 = std::dynamic_pointer_cast<ResourceDX12>( resource );
    
    if ( resourceDX12 && resourceDX12->GetD3D12Resource() )
    {
        ComPtr<ID3D12Resource> d3d12Resource = resourceDX12->GetD3D12Resource();

        TransitionResoure( resourceDX12, ResourceState::UAV, true );

        D3D12_CPU_DESCRIPTOR_HANDLE hCPU = resourceDX12->GetUnorderedAccessView( nullptr );
        D3D12_GPU_DESCRIPTOR_HANDLE hGPU = m_DynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->CopyDescriptor( shared_from_this(), hCPU );

        m_d3d12CommandList->ClearUnorderedAccessViewFloat( hGPU, hCPU, d3d12Resource.Get(), glm::value_ptr(clearValues), 0, nullptr );
    }
}

void GraphicsCommandBufferDX12::ClearResourceUInt( std::shared_ptr<Resource> resource, const glm::uvec4& clearValues )
{
    std::shared_ptr<ResourceDX12> resourceDX12 = std::dynamic_pointer_cast<ResourceDX12>( resource );

    if ( resourceDX12 && resourceDX12->GetD3D12Resource() )
    {
        ComPtr<ID3D12Resource> d3d12Resource = resourceDX12->GetD3D12Resource();

        TransitionResoure( resourceDX12, ResourceState::UAV, true );

        D3D12_CPU_DESCRIPTOR_HANDLE hCPU = resourceDX12->GetUnorderedAccessView( nullptr );
        D3D12_GPU_DESCRIPTOR_HANDLE hGPU = m_DynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->CopyDescriptor( shared_from_this(), hCPU );

        m_d3d12CommandList->ClearUnorderedAccessViewUint( hGPU, hCPU, d3d12Resource.Get(), glm::value_ptr(clearValues), 0, nullptr );
    }
}

void GraphicsCommandBufferDX12::ClearTexture( std::shared_ptr<Texture> texture, const ClearColor& color )
{
    std::shared_ptr<TextureDX12> textureDX12 = std::dynamic_pointer_cast<TextureDX12>( texture );

    TransitionResoure( texture, ResourceState::RenderTarget, true );

    m_d3d12CommandList->ClearRenderTargetView( textureDX12->GetRenderTargetView(), color.RGBA, 0, nullptr );

    m_ReferencedObjects.push_back( textureDX12->m_d3d12Resource );
}

void GraphicsCommandBufferDX12::ClearDepthStencilTexture( std::shared_ptr<Texture> texture, ClearFlags clearFlags, float depth, uint8_t stencil )
{
    std::shared_ptr<TextureDX12> textureDX12 = std::dynamic_pointer_cast<TextureDX12>( texture );

    D3D12_CLEAR_FLAGS d3d12ClearFlags = static_cast<D3D12_CLEAR_FLAGS>(0);
    if ( ( clearFlags & ClearFlags::Depth ) != 0 )
    {
        d3d12ClearFlags |= D3D12_CLEAR_FLAG_DEPTH;
    }
    if ( ( clearFlags & ClearFlags::Stencil ) != 0 )
    {
        d3d12ClearFlags |= D3D12_CLEAR_FLAG_STENCIL;
    }

    TransitionResoure( texture, ResourceState::DepthWrite, true );

    m_d3d12CommandList->ClearDepthStencilView( textureDX12->GetDepthStencilView(), d3d12ClearFlags, depth, stencil, 0, nullptr );

    m_ReferencedObjects.push_back( textureDX12->m_d3d12Resource );
}

void GraphicsCommandBufferDX12::ClearRenderTarget( std::shared_ptr<RenderTarget> renderTarget, ClearFlags clearFlags, const ClearColor& color, float depth, uint8_t stencil )
{
    std::shared_ptr<RenderTargetDX12> renderTargetDX12 = std::dynamic_pointer_cast<RenderTargetDX12>( renderTarget );
    const std::vector< std::shared_ptr<TextureDX12> >& textures = renderTargetDX12->GetTextures();

    if ( ( clearFlags & ClearFlags::Color ) != 0 )
    {
        for ( size_t i = 0; i < 8; ++i )
        {
            auto texture = textures[i];
            if ( texture )
            {
                ClearTexture( texture, color );
            }
        }
    }

    if ( ( clearFlags & ClearFlags::Depth ) != 0 )
    {
        auto texture = textures[static_cast<size_t>( AttachmentPoint::Depth )];
        if ( texture )
        {
            ClearDepthStencilTexture( texture, ClearFlags::Depth, depth );
        }
    }

    if ( (clearFlags & ClearFlags::DepthStencil ) != 0 )
    {
        auto texture = textures[static_cast<size_t>( AttachmentPoint::DepthStencil )];
        if ( texture )
        {
            ClearDepthStencilTexture( texture, clearFlags, depth, stencil );
        }
    }
}

void GraphicsCommandBufferDX12::CopyResource( std::shared_ptr<Resource> dstResource, std::shared_ptr<Resource> srcResource )
{
    std::shared_ptr<ResourceDX12> dstResourceDX12 = std::dynamic_pointer_cast<ResourceDX12>( dstResource );
    std::shared_ptr<ResourceDX12> srcResourceDX12 = std::dynamic_pointer_cast<ResourceDX12>( srcResource );

    TransitionResoure( dstResource, ResourceState::CopyDest );
    TransitionResoure( srcResource, ResourceState::CopySrc );
    FlushResourceBarriers();

    ComPtr<ID3D12Resource> d3d12DstResource = dstResourceDX12->GetD3D12Resource();
    ComPtr<ID3D12Resource> d3d12SrcResource = srcResourceDX12->GetD3D12Resource();

    m_d3d12CommandList->CopyResource( d3d12DstResource.Get(), d3d12SrcResource.Get() );

    m_ReferencedObjects.push_back( d3d12DstResource );
    m_ReferencedObjects.push_back( d3d12SrcResource );
}

void GraphicsCommandBufferDX12::ResolveMultisampleTexture( std::shared_ptr<Texture> dstTexture, uint32_t mip, uint32_t arraySlice, std::shared_ptr<Texture> srcTexture )
{
    TransitionResoure( dstTexture, ResourceState::ResolveDest );
    TransitionResoure( srcTexture, ResourceState::ResolveSrc );
    FlushResourceBarriers();

    std::shared_ptr<TextureDX12> dstTextureDX12 = std::dynamic_pointer_cast<TextureDX12>( dstTexture );
    std::shared_ptr<TextureDX12> srcTextureDX12 = std::dynamic_pointer_cast<TextureDX12>( srcTexture );

    ComPtr<ID3D12Resource> d3d12DstResource = dstTextureDX12->GetD3D12Resource();
    ComPtr<ID3D12Resource> d3d12SrcResource = srcTextureDX12->GetD3D12Resource();

    UINT dstSubresource = D3D12CalcSubresource( mip, arraySlice, 0, dstTextureDX12->GetMipLevels(), dstTextureDX12->GetDepthOrArraySize() );
    UINT srcSubresource = D3D12CalcSubresource( mip, arraySlice, 0, srcTextureDX12->GetMipLevels(), srcTextureDX12->GetDepthOrArraySize() );

    m_d3d12CommandList->ResolveSubresource( d3d12DstResource.Get(), dstSubresource, d3d12SrcResource.Get(), srcSubresource, srcTextureDX12->GetResourceFormat() );

    m_ReferencedObjects.push_back( d3d12DstResource );
    m_ReferencedObjects.push_back( d3d12SrcResource );
}

void GraphicsCommandBufferDX12::BindGraphicsPipelineState( std::shared_ptr<GraphicsPipelineState> graphicsPipelineState )
{
    std::shared_ptr<GraphicsPipelineStateDX12> graphicsPipelineStateDX12 = std::dynamic_pointer_cast<GraphicsPipelineStateDX12>( graphicsPipelineState );
    assert( graphicsPipelineStateDX12 );

    graphicsPipelineStateDX12->Bind( shared_from_this() );

    ComPtr<ID3D12PipelineState> d3d12PipelineState = graphicsPipelineStateDX12->GetD3D12PipelineState();
    m_ReferencedObjects.push_back( d3d12PipelineState );
}

void GraphicsCommandBufferDX12::BindComputePipelineState( std::shared_ptr<ComputePipelineState> pipelineState )
{
    std::shared_ptr<ComputePipelineStateDX12> computePipelineState = std::dynamic_pointer_cast<ComputePipelineStateDX12>( pipelineState );
    assert( computePipelineState );

    computePipelineState->Bind( shared_from_this() );

    ComPtr<ID3D12PipelineState> d3d12PipelineState = computePipelineState->GetD3D12PipelineState();

    m_ReferencedObjects.push_back( d3d12PipelineState );
}


void GraphicsCommandBufferDX12::BindComputeShaderSignature( std::shared_ptr<ShaderSignature> shaderSignature )
{
    std::shared_ptr<ShaderSignatureDX12> shaderSignatureDX12 = std::dynamic_pointer_cast<ShaderSignatureDX12>( shaderSignature );
    assert( shaderSignatureDX12 );

    if ( m_pCurrentComputeRootSignature != shaderSignatureDX12->GetD3D12RootSignature() )
    {
        m_pCurrentComputeShaderSignature = shaderSignatureDX12;
        m_pCurrentComputeRootSignature = shaderSignatureDX12->GetD3D12RootSignature();
        m_d3d12CommandList->SetComputeRootSignature( m_pCurrentComputeRootSignature.Get() );
        m_ReferencedObjects.push_back( m_pCurrentComputeRootSignature );
        for ( uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i )
        {
            m_DynamicDescriptorHeap[i]->SetShaderSignature( Pipeline::Compute, shaderSignatureDX12 );
        }
    }
}

void GraphicsCommandBufferDX12::BindGraphicsShaderSignature( std::shared_ptr<ShaderSignature> shaderSignature )
{
    std::shared_ptr<ShaderSignatureDX12> shaderSignatureDX12 = std::dynamic_pointer_cast<ShaderSignatureDX12>( shaderSignature );
    assert( shaderSignatureDX12 );

    if ( m_pCurrentGraphicsRootSignature != shaderSignatureDX12->GetD3D12RootSignature() )
    {
        m_pCurrentGraphicsShaderSignature = shaderSignatureDX12;
        m_pCurrentGraphicsRootSignature = shaderSignatureDX12->GetD3D12RootSignature();
        m_d3d12CommandList->SetGraphicsRootSignature( m_pCurrentGraphicsRootSignature.Get() );
        m_ReferencedObjects.push_back( m_pCurrentGraphicsRootSignature );
        for ( uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i )
        {
            m_DynamicDescriptorHeap[i]->SetShaderSignature( Pipeline::Graphics, shaderSignatureDX12 );
        }
    }
}


void GraphicsCommandBufferDX12::BindRenderTarget( std::shared_ptr<RenderTarget> renderTarget )
{
    std::shared_ptr<RenderTargetDX12> renderTargetDX12 = std::dynamic_pointer_cast<RenderTargetDX12>( renderTarget );
    renderTargetDX12->Bind( shared_from_this() );

    for ( auto texture : renderTargetDX12->GetTextures() )
    {
        if ( texture )
        {
            ComPtr<ID3D12Resource> d3d12Resource = texture->GetD3D12Resource();
            m_ReferencedObjects.push_back( d3d12Resource );
        }
    }
}

void GraphicsCommandBufferDX12::BindComputeShaderArguments( uint32_t slotID, uint32_t offset, const ShaderArguments& shaderArguments )
{
    m_DynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors( shared_from_this(), Pipeline::Compute, slotID, offset, shaderArguments );
}

void GraphicsCommandBufferDX12::BindGraphicsShaderArgument( uint32_t slotID, std::shared_ptr<Resource> shaderArgument )
{
    std::shared_ptr<ResourceDX12> resourceDX12 = std::dynamic_pointer_cast<ResourceDX12>( shaderArgument );
    if ( resourceDX12 && m_pCurrentGraphicsShaderSignature )
    {
        const ShaderParameter& shaderParameter = m_pCurrentGraphicsShaderSignature->GetParameter( slotID );
        switch ( shaderParameter.GetType() )
        {
        case ParameterType::Buffer:
            TransitionResoure( resourceDX12, ResourceState::GenericRead );
            m_d3d12CommandList->SetGraphicsRootShaderResourceView( slotID, resourceDX12->GetD3D12GPUVirtualAddress() );
            break;
        case ParameterType::RWBuffer:
            TransitionResoure( resourceDX12, ResourceState::UAV );
            m_d3d12CommandList->SetGraphicsRootUnorderedAccessView( slotID, resourceDX12->GetD3D12GPUVirtualAddress() );
            break;
        default:
            LOG_ERROR( "Only buffer types can be bound inline. Other parameter types must be bound using a descriptor table. Use BindShaderArguments instead." );
            break;
        }

        m_ReferencedObjects.push_back( resourceDX12->GetD3D12Resource() );
    }
}

void GraphicsCommandBufferDX12::BindComputeShaderArgument( uint32_t slotID, std::shared_ptr<Resource> argument )
{
    std::shared_ptr<ResourceDX12> resourceDX12 = std::dynamic_pointer_cast<ResourceDX12>( argument );
    if ( resourceDX12 && m_pCurrentComputeShaderSignature )
    {
        const ShaderParameter& shaderParameter = m_pCurrentComputeShaderSignature->GetParameter( slotID );
        switch ( shaderParameter.GetType() )
        {
        case ParameterType::Buffer:
            TransitionResoure( resourceDX12, ResourceState::GenericRead );
            m_d3d12CommandList->SetComputeRootShaderResourceView( slotID, resourceDX12->GetD3D12GPUVirtualAddress() );
            break;
        case ParameterType::RWBuffer:
            TransitionResoure( resourceDX12, ResourceState::UAV );
            m_d3d12CommandList->SetComputeRootUnorderedAccessView( slotID, resourceDX12->GetD3D12GPUVirtualAddress() );
            break;
        default: 
            LOG_ERROR( "Only buffer types can be bound inline. Other parameter types must be bound using a descriptor table. Use BindShaderArguments instead." );
            break;
        }

        m_ReferencedObjects.push_back( resourceDX12->GetD3D12Resource() );
    }
}

void GraphicsCommandBufferDX12::BindGraphicsShaderArguments( uint32_t slotID, uint32_t offset, const ShaderArguments& shaderArguments )
{
    m_DynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors( shared_from_this(), Pipeline::Graphics, slotID, offset, shaderArguments );
}

void GraphicsCommandBufferDX12::BindCompute32BitConstants( uint32_t slotID, uint32_t numConstants, const void* constants )
{
    m_d3d12CommandList->SetComputeRoot32BitConstants( slotID, numConstants, constants, 0 );
}

void GraphicsCommandBufferDX12::BindGraphics32BitConstants( uint32_t slotID, uint32_t numConstants, const void* constants )
{
    m_d3d12CommandList->SetGraphicsRoot32BitConstants( slotID, numConstants, constants, 0 );
}

void GraphicsCommandBufferDX12::BindComputeDynamicConstantBuffer( uint32_t slotID, size_t bufferSizeInBytes, const void* bufferData )
{
    HeapAllocation heapAllocation = m_UploadHeap->Allocate( bufferSizeInBytes, 256 );
    memcpy( heapAllocation.CpuPtr, bufferData, bufferSizeInBytes );

    m_d3d12CommandList->SetComputeRootConstantBufferView( slotID, heapAllocation.GpuAddress );
}

void GraphicsCommandBufferDX12::BindGraphicsDynamicConstantBuffer( uint32_t slotID, size_t bufferSizeInBytes, const void* bufferData )
{
    HeapAllocation heapAllocation = m_UploadHeap->Allocate( bufferSizeInBytes, 256 );
    memcpy( heapAllocation.CpuPtr, bufferData, bufferSizeInBytes );

    m_d3d12CommandList->SetGraphicsRootConstantBufferView( slotID, heapAllocation.GpuAddress );
}

void GraphicsCommandBufferDX12::BindComputeDynamicStructuredBuffer( uint32_t slotID, size_t numElements, size_t elementSize, const void* bufferData )
{
    size_t bufferSize = numElements * elementSize;
    HeapAllocation heapAllocation = m_UploadHeap->Allocate( bufferSize, elementSize );
    memcpy( heapAllocation.CpuPtr, bufferData, bufferSize );

    m_d3d12CommandList->SetComputeRootShaderResourceView( slotID, heapAllocation.GpuAddress );
}

void GraphicsCommandBufferDX12::BindGraphicsDynamicStructuredBuffer( uint32_t slotID, size_t numElements, size_t elementSize, const void* bufferData )
{
    size_t bufferSize = numElements * elementSize;
    HeapAllocation heapAllocation = m_UploadHeap->Allocate( bufferSize, elementSize );
    memcpy( heapAllocation.CpuPtr, bufferData, bufferSize );

    m_d3d12CommandList->SetGraphicsRootShaderResourceView( slotID, heapAllocation.GpuAddress );
}

void Graphics::GraphicsCommandBufferDX12::SetBuffer( std::shared_ptr<ResourceDX12> resourceDX12, size_t numElements, size_t elementSize, const void* bufferData, D3D12_RESOURCE_FLAGS flags )
{
    size_t bufferSize = numElements * elementSize;

    ComPtr<ID3D12Resource> d3d12Resource;
    if ( bufferSize == 0 )
    {
//        LOG_WARNING( "Creating a 0 size buffer will result in a null resource." );
    }
    else
    {
        if ( FAILED( m_d3d12Device->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT ),
                                                             D3D12_HEAP_FLAG_NONE,
                                                             &CD3DX12_RESOURCE_DESC::Buffer( bufferSize, flags ),
                                                             D3D12_RESOURCE_STATE_COMMON,
                                                             nullptr,
                                                             IID_PPV_ARGS( &d3d12Resource ) ) ) )
        {
            LOG_ERROR( "Failed to create a committed resource." );
        }
        else
        {
            if ( bufferData != nullptr )
            {
                // Create an upload resource to use as an intermediate buffer to copy the buffer resource 
                ComPtr<ID3D12Resource> uploadResource;
                if ( FAILED( m_d3d12Device->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD ),
                                                                     D3D12_HEAP_FLAG_NONE,
                                                                     &CD3DX12_RESOURCE_DESC::Buffer( bufferSize ),
                                                                     D3D12_RESOURCE_STATE_GENERIC_READ,
                                                                     nullptr,
                                                                     IID_PPV_ARGS( &uploadResource ) ) ) )
                {
                    LOG_ERROR( "Failed to create committed upload resouce." );
                }
                else
                {
                    D3D12_SUBRESOURCE_DATA subresourceData = {};
                    subresourceData.pData = bufferData;
                    subresourceData.RowPitch = bufferSize;
                    subresourceData.SlicePitch = subresourceData.RowPitch;

                    UpdateSubresources( m_d3d12CommandList.Get(), d3d12Resource.Get(), uploadResource.Get(), 0, 0, 1, &subresourceData );

                    // Add references to resources so they stay in scope until the command list is reset.
                    m_ReferencedObjects.push_back( d3d12Resource );
                    m_ReferencedObjects.push_back( uploadResource );
                }
            }
        }
    }

    resourceDX12->SetD3D12Resource( d3d12Resource, D3D12_RESOURCE_STATE_COPY_DEST );
    resourceDX12->CreateViews( numElements, elementSize );
}

void GraphicsCommandBufferDX12::SetVertexBuffer( std::shared_ptr<VertexBuffer> vertexBuffer, size_t numVertices, size_t vertexStride, const void* vertexData )
{
    std::shared_ptr<VertexBufferDX12> vertexBufferDX12 = std::dynamic_pointer_cast<VertexBufferDX12>( vertexBuffer );
    SetBuffer( vertexBufferDX12, numVertices, vertexStride, vertexData );
}

void GraphicsCommandBufferDX12::SetIndexBuffer( std::shared_ptr<IndexBuffer> indexBuffer, size_t numIndicies, size_t indexSizeInBytes, const void * indexData )
{
    std::shared_ptr<IndexBufferDX12> indexBufferDX12 = std::dynamic_pointer_cast<IndexBufferDX12>( indexBuffer );
    SetBuffer( indexBufferDX12, numIndicies, indexSizeInBytes, indexData );
}

void GraphicsCommandBufferDX12::SetConstantBuffer( std::shared_ptr<ConstantBuffer> constantBuffer, size_t sizeInBytes, const void* bufferData )
{
    std::shared_ptr<ConstantBufferDX12> constantBufferDX12 = std::dynamic_pointer_cast<ConstantBufferDX12>( constantBuffer );
    size_t sizeInBytes256 = Math::AlignUp( sizeInBytes, 256 ); // Constant buffer size is required to be 256-byte aligned.
    SetBuffer( constantBufferDX12, 1, sizeInBytes256, bufferData );
}

void GraphicsCommandBufferDX12::SetByteAddressBuffer( std::shared_ptr<ByteAddressBuffer> byteAddressBuffer, size_t sizeInBytes, const void* bufferData )
{
    std::shared_ptr<ByteAddressBufferDX12> byteAddressBufferDX12 = std::dynamic_pointer_cast<ByteAddressBufferDX12>( byteAddressBuffer );
    SetBuffer( byteAddressBufferDX12, 1, sizeInBytes, bufferData, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS );
}

void GraphicsCommandBufferDX12::SetStructuredBuffer( std::shared_ptr<StructuredBuffer> structuredBuffer, size_t numElements, size_t elementSize, const void* bufferData )
{
    std::shared_ptr<StructuredBufferDX12> structuredBufferDX12 = std::dynamic_pointer_cast<StructuredBufferDX12>( structuredBuffer );
    SetBuffer( structuredBufferDX12, numElements, elementSize, bufferData, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS );
}

void GraphicsCommandBufferDX12::SetTextureSubresource( std::shared_ptr<Texture> texture, uint32_t mip, uint32_t arraySlice, const void* pTextureData )
{
    assert( pTextureData );

    std::shared_ptr<TextureDX12> textureDX12 = std::dynamic_pointer_cast<TextureDX12>( texture );
    assert( textureDX12 );

    UINT firstSubresource = D3D12CalcSubresource( mip, arraySlice, 0, textureDX12->GetMipLevels(), textureDX12->GetTextureDimension() == TextureDimension::Texture3D ? 1 : textureDX12->GetDepthOrArraySize() );
    UINT64 uploadBufferSize = GetRequiredIntermediateSize( textureDX12->m_d3d12Resource.Get(), firstSubresource, 1 );

    ComPtr<ID3D12Resource> uploadResource;
    if ( FAILED( m_d3d12Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD ),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer( uploadBufferSize ),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS( &uploadResource ) ) ) )
    {
        LOG_ERROR( "Failed to create committed upload resource." );
    }
    else
    {
        size_t w = std::max<size_t>( 1, textureDX12->GetWidth() >> mip );
        size_t h = std::max<size_t>( 1, textureDX12->GetHeight() >> mip );
        DXGI_FORMAT format = textureDX12->GetResourceFormat();
        
        size_t rowPitch, slicePitch;
        GetSurfaceInfo( w, h, format, &slicePitch, &rowPitch, nullptr );

        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData = pTextureData;
        subresourceData.RowPitch = rowPitch;
        subresourceData.SlicePitch = slicePitch;
        
        TransitionResoure( texture, ResourceState::CopyDest, true );
        UpdateSubresources( m_d3d12CommandList.Get(), textureDX12->m_d3d12Resource.Get(), uploadResource.Get(), 0, firstSubresource, 1, &subresourceData );

        // Add references to the resources so they stay in memory until the copy operation completes.
        m_ReferencedObjects.push_back( textureDX12->m_d3d12Resource );
        m_ReferencedObjects.push_back( uploadResource );
    }

}

void GraphicsCommandBufferDX12::BindIndexBuffer( std::shared_ptr<IndexBuffer> indexBuffer, size_t offset )
{
    std::shared_ptr<IndexBufferDX12> indexBufferDX12 = std::dynamic_pointer_cast<IndexBufferDX12>( indexBuffer );
    assert( indexBufferDX12 );

    TransitionResoure( indexBuffer, ResourceState::IndexBuffer );

    D3D12_INDEX_BUFFER_VIEW ibView = indexBufferDX12->GetIndexBufferView();
    m_d3d12CommandList->IASetIndexBuffer( &ibView );

    m_ReferencedObjects.push_back( indexBufferDX12->m_d3d12Resource );
}

void GraphicsCommandBufferDX12::BindDynamicIndexBuffer( size_t numIndicies, size_t indexSizeInBytes, const void* indexData )
{
    assert( indexSizeInBytes == 2 || indexSizeInBytes == 4 );

    size_t bufferSize = numIndicies * indexSizeInBytes;

    HeapAllocation heapAllocation = m_UploadHeap->Allocate( bufferSize, indexSizeInBytes );

    memcpy( heapAllocation.CpuPtr, indexData, bufferSize );

    D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
    indexBufferView.BufferLocation = heapAllocation.GpuAddress;
    indexBufferView.SizeInBytes = static_cast<UINT>( bufferSize );
    indexBufferView.Format = ( indexSizeInBytes == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT );

    m_d3d12CommandList->IASetIndexBuffer( &indexBufferView );
}


void GraphicsCommandBufferDX12::BindVertexBuffer( uint32_t slotID, std::shared_ptr<VertexBuffer> vertexBuffer )
{
    std::shared_ptr<VertexBufferDX12> vertexBufferDX12 = std::dynamic_pointer_cast<VertexBufferDX12>( vertexBuffer );
    assert( vertexBufferDX12 );

    TransitionResoure( vertexBuffer, ResourceState::VertexBuffer );

    D3D12_VERTEX_BUFFER_VIEW vbView = vertexBufferDX12->GetVertexBufferView();
    m_d3d12CommandList->IASetVertexBuffers( slotID, 1, &vbView );

    m_ReferencedObjects.push_back( vertexBufferDX12->m_d3d12Resource );
}

void GraphicsCommandBufferDX12::BindDynamicVertexBuffer( uint32_t slotID, size_t numVertices, size_t vertexSizeInBytes, const void* vertexData )
{
    size_t bufferSize = numVertices * vertexSizeInBytes;

    HeapAllocation heapAllocation = m_UploadHeap->Allocate( bufferSize, vertexSizeInBytes );

    memcpy( heapAllocation.CpuPtr, vertexData, bufferSize );

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
    vertexBufferView.BufferLocation = heapAllocation.GpuAddress;
    vertexBufferView.SizeInBytes = static_cast<UINT>( bufferSize );
    vertexBufferView.StrideInBytes = static_cast<UINT>( vertexSizeInBytes );

    m_d3d12CommandList->IASetVertexBuffers( slotID, 1, &vertexBufferView );
}

void GraphicsCommandBufferDX12::Draw( uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount, uint32_t firstInstance )
{
    FlushResourceBarriers();
    CopyAndBindAllStagedDescriptors( Pipeline::Graphics );
    m_d3d12CommandList->DrawInstanced( vertexCount, instanceCount, firstVertex, firstInstance );
}

void GraphicsCommandBufferDX12::DrawIndexed( uint32_t indexCount, uint32_t firstIndex, int32_t baseVertex, uint32_t instanceCount, uint32_t firstInstance )
{
    FlushResourceBarriers();
    CopyAndBindAllStagedDescriptors( Pipeline::Graphics );
    m_d3d12CommandList->DrawIndexedInstanced( indexCount, instanceCount, firstIndex, baseVertex, firstInstance );
}

void GraphicsCommandBufferDX12::Dispatch( uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ )
{
    FlushResourceBarriers();
    CopyAndBindAllStagedDescriptors( Pipeline::Compute );
    m_d3d12CommandList->Dispatch( numGroupsX, numGroupsY, numGroupsZ );
}

void GraphicsCommandBufferDX12::ExecuteIndirect( std::shared_ptr<IndirectCommandSignature> commandSignature,
                                                 std::shared_ptr<Buffer> indirectArguments, size_t byteOffset,
                                                 std::shared_ptr<Buffer> commandCountBuffer, size_t countBufferOffset )
{
    std::shared_ptr<IndirectCommandSignatureDX12> commandSignatureDX12 = std::dynamic_pointer_cast<IndirectCommandSignatureDX12>( commandSignature );
    std::shared_ptr<ResourceDX12> indirectArgumentsDX12 = std::dynamic_pointer_cast<ResourceDX12>( indirectArguments );

    TransitionResoure( indirectArgumentsDX12, ResourceState::IndirectArgument );

    UINT maxCommandCount = static_cast<UINT>( commandSignature->GetCommandArguments().size() );
    ComPtr<ID3D12CommandSignature> d3d12CommandSignature = commandSignatureDX12->GetD3D12CommandSignature();
    ComPtr<ID3D12Resource> pArgumentBuffer = indirectArgumentsDX12->GetD3D12Resource();
    ComPtr<ID3D12Resource> pCountBuffer;
    
    if ( commandCountBuffer )
    {
        std::shared_ptr<ResourceDX12> countBufferDX12 = std::dynamic_pointer_cast<ResourceDX12>( commandCountBuffer );
        pCountBuffer = countBufferDX12->GetD3D12Resource();

        m_ReferencedObjects.push_back( pCountBuffer );
    }

    FlushResourceBarriers();
    if ( m_d3d12CommandListType == D3D12_COMMAND_LIST_TYPE_DIRECT || m_d3d12CommandListType == D3D12_COMMAND_LIST_TYPE_BUNDLE )
    {
        CopyAndBindAllStagedDescriptors( Pipeline::Graphics );
    }
    CopyAndBindAllStagedDescriptors( Pipeline::Compute );

    m_d3d12CommandList->ExecuteIndirect( d3d12CommandSignature.Get(), maxCommandCount, pArgumentBuffer.Get(), byteOffset, pCountBuffer.Get(), countBufferOffset );

    m_ReferencedObjects.push_back( d3d12CommandSignature );
    m_ReferencedObjects.push_back( pArgumentBuffer );
}


void GraphicsCommandBufferDX12::GenerateMips( std::shared_ptr<Texture> texture )
{
    std::shared_ptr<TextureDX12> textureDX12 = std::dynamic_pointer_cast<TextureDX12>( texture );

    uint8_t numMipLevels = textureDX12 ? textureDX12->GetMipLevels() : 0;

    // Check if the texture actually has any lower mip levels.
    if ( numMipLevels <= 1 ) return;

    alignas(16) struct GenerateMipsCB
    {
        uint32_t SrcMipLevel;	// Texture level of source mip
        uint32_t NumMipLevels;	// Number of OutMips to write: [1-4]
        glm::vec2 TexelSize;	// 1.0 / OutMip1.Dimensions
    } generateMipsCB;

    if ( std::shared_ptr<DeviceDX12> device = m_Device.lock() )
    {
        //Core::LogManager::LogInfo( "Generating mipmaps for ", texture->GetFileName() );

        uint32_t textureWidth = textureDX12->GetWidth();
        uint32_t textureHeight = textureDX12->GetHeight();
        
        for ( uint8_t srcMip = 0; srcMip < numMipLevels; )
        {
            uint32_t srcWidth = textureWidth >> srcMip;
            uint32_t srcHeight = textureHeight >> srcMip;
            uint32_t dstWidth = srcWidth >> 1;
            uint32_t dstHeight = srcHeight >> 1;

            // Determine the compute shader to use based on the dimension of the 
            // source texture.
            // 0b00(0): Both width and height are even.
            // 0b01(1): Width is odd, height is even.
            // 0b10(2): Width is even, height is odd.
            // 0b11(3): Both width and height are odd.
            uint32_t generateMipsVariation = ( srcHeight & 1 ) << 1 | ( srcWidth & 1 );
            std::shared_ptr<ComputePipelineStateDX12> generateMipsPSO = device->GetMipsPipelineState( generateMipsVariation );

            BindComputePipelineState( generateMipsPSO );

            // How many mip map levels to compute this pass (max 4 mips per pass)
            DWORD mipCount;
            // The number of times we can half the size of the texture and get exactly a 50% reduction in size.
            // A 1 bit in the width or height indicates an odd dimension.
            // The case where either the width or the height is exactly 1 is handled as a special case (as the dimension does not require reduction).
            _BitScanForward( &mipCount, ( dstWidth == 1 ? dstHeight : dstWidth ) | ( dstHeight == 1 ? dstWidth : dstHeight ) );
            // Maximum number of mips to generate is 4.
            mipCount = std::min<DWORD>( 4, mipCount + 1 );
            // Clamp to total number of mips left over.
            mipCount = ( srcMip + mipCount ) > numMipLevels ? numMipLevels - srcMip : mipCount;

            // Dimensions should not reduce to 0.
            // This can happen if the width and height are not the same.
            dstWidth = std::max<DWORD>( 1, dstWidth );
            dstHeight = std::max<DWORD>( 1, dstHeight );

            generateMipsCB.SrcMipLevel = srcMip;
            generateMipsCB.NumMipLevels = mipCount;
            generateMipsCB.TexelSize = 1.0f / glm::vec2( dstWidth, dstHeight );

            m_DynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors( Pipeline::Compute, 1, 0, 1, textureDX12->GetShaderResourceView( shared_from_this() ) );
            m_DynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors( Pipeline::Compute, 2, 0, 4, textureDX12->GetUnorderedAccessView( shared_from_this(), srcMip + 1 ) );

            ComputeCommandBuffer::BindCompute32BitConstants( 0, generateMipsCB );

            Dispatch( dstWidth, dstHeight );

            srcMip += static_cast<uint8_t>(mipCount);
        }

        TransitionResoure( texture, ResourceState::Common );

        m_ReferencedObjects.push_back( textureDX12->GetD3D12Resource() );
    }
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GraphicsCommandBufferDX12::GetD3D12CommandList() const
{
    return m_d3d12CommandList;
}

D3D12_COMMAND_LIST_TYPE GraphicsCommandBufferDX12::GetD3D12CommandListType() const
{
    return m_d3d12CommandListType;
}

void GraphicsCommandBufferDX12::SetDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE heapType, ID3D12DescriptorHeap* heap )
{
    if ( m_DescriptorHeaps[heapType] != heap )
    {
        m_DescriptorHeaps[heapType] = heap;
        BindDescriptorHeaps();
    }
}

void GraphicsCommandBufferDX12::BindDescriptorHeaps()
{
    UINT numDescriptorHeaps = 0;
    ID3D12DescriptorHeap* descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

    for ( uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i )
    {
        ID3D12DescriptorHeap* descriptorHeap = m_DescriptorHeaps[i];
        if ( descriptorHeap )
        {
            descriptorHeaps[numDescriptorHeaps++] = descriptorHeap;
        }
    }

    m_d3d12CommandList->SetDescriptorHeaps( numDescriptorHeaps, descriptorHeaps );
}

void GraphicsCommandBufferDX12::CopyAndBindAllStagedDescriptors( Pipeline pipeline )
{
    for ( uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i )
    {
        m_DynamicDescriptorHeap[i]->CopyAndBindStagedDescriptors( pipeline, shared_from_this() );
    }
}
