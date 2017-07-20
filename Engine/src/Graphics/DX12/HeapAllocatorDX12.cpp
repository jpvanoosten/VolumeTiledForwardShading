#include <EnginePCH.h>

#include <Graphics/DX12/HeapAllocatorDX12.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/DynamicBufferDX12.h>

#include <LogManager.h>

using namespace Graphics;
using namespace Microsoft::WRL;

HeapAllocatorDX12::HeapAllocatorDX12( std::shared_ptr<DeviceDX12> device, 
                                      D3D12_HEAP_TYPE d3d12HeapType,
                                      uint64_t pageSize,
                                      D3D12_RESOURCE_FLAGS d3d12ResourceFlags,
                                      D3D12_RESOURCE_STATES d3d12ResourceState )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
    , m_d3d12HeapType( d3d12HeapType )
    , m_PageSize( pageSize )
    , m_d3d12ResourceFlags( d3d12ResourceFlags )
    , m_d3d12ResouceState( d3d12ResourceState )
    , m_CurrentOffset( 0 )
{

}

HeapAllocatorDX12::~HeapAllocatorDX12()
{

}

HeapAllocation HeapAllocatorDX12::Allocate( size_t sizeInBytes, size_t alignment )
{
    // Cannot request more than a single page size.
    assert( sizeInBytes <= m_PageSize );

    scoped_lock lock( m_Mutex );

    // Aligned size
    const size_t alignedSize = Math::AlignUp( sizeInBytes, alignment );
    m_CurrentOffset = Math::AlignUp( m_CurrentOffset, alignment );

    if ( !m_CurrentBuffer || m_CurrentOffset + alignedSize > m_PageSize )
    {
        m_CurrentBuffer = RequestBuffer();
        assert( m_CurrentBuffer );
        m_CurrentOffset = 0;
    }

    HeapAllocation heapAllocation;
    heapAllocation.CpuPtr = static_cast<uint8_t*>( m_CurrentBuffer->m_DataPtr ) + m_CurrentOffset;
    heapAllocation.GpuAddress = m_CurrentBuffer->m_d3d12GPUVirtualAddress + m_CurrentOffset;

    m_CurrentOffset += alignedSize;

    return heapAllocation;
}


std::shared_ptr<DynamicBufferDX12> HeapAllocatorDX12::RequestBuffer()
{
    std::shared_ptr<DynamicBufferDX12> ret;

    if ( !m_AvailableBuffers.empty() )
    {
        ret = m_AvailableBuffers.front();
        m_AvailableBuffers.pop();
    }
    else
    {
        ret = CreateBuffer();
        m_BufferPool.push( ret );
    }

    return ret;
}

std::shared_ptr<DynamicBufferDX12> HeapAllocatorDX12::CreateBuffer()
{
    ComPtr<ID3D12Resource> d3d12Resource;
    if ( FAILED( m_d3d12Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES( m_d3d12HeapType ),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer( m_PageSize, m_d3d12ResourceFlags ),
        m_d3d12ResouceState,
        nullptr,
        IID_PPV_ARGS( &d3d12Resource ) ) ) )
    {
        LOG_ERROR( "Failed to create committed resource." );
        return nullptr;
    }

    return std::make_shared<DynamicBufferDX12>( m_Device.lock(), d3d12Resource, m_d3d12ResouceState );
}

void HeapAllocatorDX12::Free()
{
    scoped_lock lock( m_Mutex );

    m_AvailableBuffers = m_BufferPool;
    m_CurrentBuffer.reset();
    m_CurrentOffset = 0;
}