#include <EnginePCH.h>

#include <Graphics/DX12/DescriptorAllocatorDX12.h>
#include <Graphics/DX12/DeviceDX12.h>

#include <LogManager.h>

using namespace Graphics;

DescriptorAllocatorDX12::DescriptorAllocatorDX12( Microsoft::WRL::ComPtr<ID3D12Device> d3d12Device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap )
    : m_d3d12Device( d3d12Device )
    , m_HeapType( type )
    , m_NumDescriptorsPerHeap( numDescriptorsPerHeap )
    , m_NumFreeHandles( 0 )
{
    m_DescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize( type );
}

DescriptorAllocatorDX12::~DescriptorAllocatorDX12()
{}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocatorDX12::Allocate( uint32_t numDescriptors )
{
    scoped_lock m_Lock( m_Mutex );

    if ( m_CurrentHeap == nullptr || m_NumFreeHandles < numDescriptors )
    {
        m_CurrentHeap = CreateDescriptorHeap( m_HeapType, m_NumDescriptorsPerHeap );
        m_CurrentHandle = m_CurrentHeap->GetCPUDescriptorHandleForHeapStart();
        m_NumFreeHandles = m_NumDescriptorsPerHeap;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE ret = m_CurrentHandle;
    m_CurrentHandle.Offset( numDescriptors, m_DescriptorSize );
    m_NumFreeHandles -= numDescriptors;

    return ret;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DescriptorAllocatorDX12::CreateDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors )
{
    // Descriptors created in this heap are not shader visible. 
    // Descriptors allocated from this heap must be copied to a shader visible heap 
    // before being used in a shader.
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = type;
    heapDesc.NumDescriptors = numDescriptors;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDesc.NodeMask = 1;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> d3d12DescriptorHeap;
    if ( FAILED( m_d3d12Device->CreateDescriptorHeap( &heapDesc, IID_PPV_ARGS( &d3d12DescriptorHeap ) ) ) )
    {
        LOG_ERROR( "Failed to create descriptor heap." );
    }
    else
    {
        m_DescriptorHeapPool.emplace_back( d3d12DescriptorHeap );
    }

    return d3d12DescriptorHeap;
}
