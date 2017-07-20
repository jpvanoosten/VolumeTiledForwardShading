#include <EnginePCH.h>

#include <Graphics/DX12/QueueSemaphoreDX12.h>
#include <Graphics/DX12/DeviceDX12.h>

#include <LogManager.h>

using namespace Graphics;
using namespace Microsoft::WRL;

QueueSemaphoreDX12::QueueSemaphoreDX12( std::shared_ptr<DeviceDX12> device )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
    , m_FenceValue( 0 )
{
    if ( FAILED( m_d3d12Device->CreateFence( m_FenceValue, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS( &m_d3d12Fence ) ) ) )
    {
        LOG_ERROR( "Failed to create fence." );
    }
}

QueueSemaphoreDX12::~QueueSemaphoreDX12()
{}

uint64_t QueueSemaphoreDX12::IncrementFence()
{
    return ++m_FenceValue;
}

Microsoft::WRL::ComPtr<ID3D12Fence> QueueSemaphoreDX12::GetD3D12Fence() const
{
    return m_d3d12Fence;
}

uint64_t QueueSemaphoreDX12::GetFenceValue() const
{
    return m_FenceValue;
}
