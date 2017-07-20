#include <EnginePCH.h>

#include <Graphics/DX12/FenceDX12.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/GraphicsCommandQueueDX12.h>
#include <LogManager.h>

using namespace Graphics;
using namespace Microsoft::WRL;

FenceDX12::FenceDX12( std::shared_ptr<GraphicsCommandQueueDX12> commandQueue, uint64_t fenceValue )
    : m_CommandQueue( commandQueue )
    , m_FenceValue( fenceValue )
{}

FenceDX12::~FenceDX12()
{}

FenceStatus FenceDX12::GetStatus() const
{
    FenceStatus status;
    if ( m_CommandQueue->GetCompletedFenceValue() < m_FenceValue )
    {
        status = FenceStatus::NotReady;
    }
    else
    {
        status = FenceStatus::Ready;
    }

    return status;
}

void FenceDX12::WaitFor( std::chrono::milliseconds duration )
{
    m_CommandQueue->WaitForFence( m_FenceValue, duration );
}

uint64_t FenceDX12::GetFenceValue() const
{
    return m_FenceValue;
}
