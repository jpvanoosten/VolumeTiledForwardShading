#include <EnginePCH.h>

#include <Graphics/DX12/GraphicsCommandQueueDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/FenceDX12.h>
#include <Graphics/DX12/QueueSemaphoreDX12.h>

#include <LogManager.h>

using namespace Graphics;
using namespace Microsoft::WRL;

GraphicsCommandQueueDX12::GraphicsCommandQueueDX12( std::shared_ptr<DeviceDX12> device,
                                                    D3D12_COMMAND_LIST_TYPE type,
                                                    INT priority,
                                                    D3D12_COMMAND_QUEUE_FLAGS flags,
                                                    UINT nodeMask )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
    , m_FenceValue( 1 )
    , m_d3d12CommandListType( type )
    , m_Priority( priority )
    , m_Flags( flags )
    , m_NodeMask( nodeMask )
    , m_GPUFrequency( 0 )

{
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
    commandQueueDesc.Type = m_d3d12CommandListType;
    commandQueueDesc.Priority = m_Priority;
    commandQueueDesc.Flags = m_Flags;
    commandQueueDesc.NodeMask = m_NodeMask;

    if ( FAILED( m_d3d12Device->CreateCommandQueue( &commandQueueDesc, IID_PPV_ARGS( &m_d3d12CommandQueue ) ) ) )
    {
        LOG_ERROR( "Failed to create ID3D12CommandQueue" );
    }
    else
    {
        std::wstring commandQueueName = L"ID3D12CommandQueue";
        switch ( m_d3d12CommandListType )
        {
        case D3D12_COMMAND_LIST_TYPE_COPY:
            commandQueueName += L"(Copy)";
            break;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE:
            commandQueueName += L"(Compute)";
            break;
        case D3D12_COMMAND_LIST_TYPE_DIRECT:
            commandQueueName += L"(Direct)";
            break;
        case D3D12_COMMAND_LIST_TYPE_BUNDLE:
            commandQueueName += L"(Bundle)";
            break;
        }

        m_d3d12CommandQueue->SetName( commandQueueName.c_str() );

        // Timestamps are only supported on 3D and Compute queues.
        if ( m_d3d12CommandListType == D3D12_COMMAND_LIST_TYPE_COMPUTE || m_d3d12CommandListType == D3D12_COMMAND_LIST_TYPE_DIRECT )
        {
            if ( FAILED( m_d3d12CommandQueue->GetTimestampFrequency( &m_GPUFrequency ) ) )
            {
                LOG_ERROR( "Failed to query GPU timestamp frequency." );
            }
        }

        if ( FAILED( m_d3d12Device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &m_d3d12Fence ) ) ) )
        {
            LOG_ERROR( "Failed to create fence object." );
        }
        else
        {
            m_FenceEventHandle = CreateEvent( nullptr, false, false, nullptr );
            if ( m_FenceEventHandle == INVALID_HANDLE_VALUE )
            {
                LOG_ERROR( "Failed to create fence event." );
            }
        }
    }
}

GraphicsCommandQueueDX12::~GraphicsCommandQueueDX12()
{
    // Wait for the queue to finish it's work before exiting.
    Idle();
}

void GraphicsCommandQueueDX12::Signal( std::shared_ptr<QueueSemaphore> queueSemaphore )
{
    std::shared_ptr<QueueSemaphoreDX12> queueSemaphoreDX12 = std::dynamic_pointer_cast<QueueSemaphoreDX12>( queueSemaphore );
    assert( queueSemaphoreDX12 );

    m_d3d12CommandQueue->Signal( queueSemaphoreDX12->GetD3D12Fence().Get(), queueSemaphoreDX12->IncrementFence() );
}

void GraphicsCommandQueueDX12::Wait( std::shared_ptr<QueueSemaphore> queueSemaphore )
{
    std::shared_ptr<QueueSemaphoreDX12> queueSemaphoreDX12 = std::dynamic_pointer_cast<QueueSemaphoreDX12>( queueSemaphore );
    assert( queueSemaphoreDX12 );

    m_d3d12CommandQueue->Wait( queueSemaphoreDX12->GetD3D12Fence().Get(), queueSemaphoreDX12->GetFenceValue() );
}

uint64_t GraphicsCommandQueueDX12::GetCompletedFenceValue() const
{
    return m_d3d12Fence->GetCompletedValue();
}

bool GraphicsCommandQueueDX12::IsFenceComplete( uint64_t fenceValue )
{
    return fenceValue <= GetCompletedFenceValue();
}

void GraphicsCommandQueueDX12::WaitForFence( uint64_t fenceValue, std::chrono::milliseconds duration )
{
    if ( !IsFenceComplete( fenceValue ) )
    {
        scoped_lock lock( m_FenceMutex );
        m_d3d12Fence->SetEventOnCompletion( fenceValue, m_FenceEventHandle );
        WaitForSingleObject( m_FenceEventHandle, static_cast<DWORD>( duration.count() ) );
    }
}

void GraphicsCommandQueueDX12::Idle()
{
    scoped_lock lock( m_Mutex );
    CommandBufferQueue tmpQueue = m_CommandBufferQueue;
    while ( !tmpQueue.empty() )
    {
        CommandBufferEntry commandBufferEntry = tmpQueue.front();
        WaitForFence( commandBufferEntry.FenceValue );

        commandBufferEntry.CommandBuffer->ReleaseReferences();
        tmpQueue.pop();
    }
}

std::shared_ptr<Fence> GraphicsCommandQueueDX12::Signal()
{
    uint64_t fenceValue = m_FenceValue++;
    m_d3d12CommandQueue->Signal( m_d3d12Fence.Get(), fenceValue );

    return std::make_shared<FenceDX12>( shared_from_this(), fenceValue );
}

std::shared_ptr<Fence> GraphicsCommandQueueDX12::Submit( const CommandBufferList& commandBuffers )
{
    scoped_lock lock( m_Mutex );

    std::vector< ID3D12CommandList* > commandLists(commandBuffers.size());
    UINT numCommandLists = 0;
    for ( auto commandBuffer : commandBuffers )
    {
        std::shared_ptr<GraphicsCommandBufferDX12> commandBufferDX12 = std::dynamic_pointer_cast<GraphicsCommandBufferDX12>( commandBuffer );
        if ( commandBufferDX12 )
        {
            commandBufferDX12->End();
            commandLists[numCommandLists++] = commandBufferDX12->GetD3D12CommandList().Get();
        }
    }

    if ( numCommandLists > 0 )
    {
        m_d3d12CommandQueue->ExecuteCommandLists( numCommandLists, commandLists.data() );
    }

    // Signal the queue and return a fence object that can be used to synchronize queue execution.
    std::shared_ptr<FenceDX12> fence = std::dynamic_pointer_cast<FenceDX12>( Signal() );

    // Add the command buffers to a queue for reuse.
    for ( auto commandBuffer : commandBuffers )
    {
        std::shared_ptr<GraphicsCommandBufferDX12> commandBufferDX12 = std::dynamic_pointer_cast<GraphicsCommandBufferDX12>( commandBuffer );
        if ( commandBufferDX12 )
        {
            CommandBufferEntry commandBufferEntry = { fence->GetFenceValue(), commandBufferDX12 };
            m_CommandBufferQueue.push( commandBufferEntry );
        }
    }

    return fence;
}

std::shared_ptr<Fence> GraphicsCommandQueueDX12::Submit( std::shared_ptr<CommandBuffer> commandBuffer )
{
    CommandBufferList commandBuffers( 1, commandBuffer );
    return Submit( commandBuffers );
}

std::shared_ptr<CopyCommandBuffer> GraphicsCommandQueueDX12::GetCopyCommandBuffer()
{
    return GetCommandBuffer();
}

std::shared_ptr<ComputeCommandBuffer> GraphicsCommandQueueDX12::GetComputeCommandBuffer()
{
    if ( m_d3d12CommandListType != D3D12_COMMAND_LIST_TYPE_DIRECT && m_d3d12CommandListType != D3D12_COMMAND_LIST_TYPE_COMPUTE )
    {
        LOG_ERROR( "Compute command buffers can only be created with a graphics or compute command queue." );
        return nullptr;
    }

    return GetCommandBuffer();
}

std::shared_ptr<GraphicsCommandBuffer> GraphicsCommandQueueDX12::GetGraphicsCommandBuffer()
{
    if ( m_d3d12CommandListType != D3D12_COMMAND_LIST_TYPE_DIRECT )
    {
        LOG_ERROR( "Graphics command buffers can only be created with a graphics command queue." );
        return nullptr;
    }

    return GetCommandBuffer();
}

std::shared_ptr<GraphicsCommandBufferDX12> GraphicsCommandQueueDX12::GetCommandBuffer()
{
    scoped_lock lock( m_Mutex );
    std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer;

    // First check if there is an available command buffer ready for reuse.
    if ( !m_CommandBufferQueue.empty() && IsFenceComplete( m_CommandBufferQueue.front().FenceValue ) )
    {
        CommandBufferEntry commandBufferEntry = m_CommandBufferQueue.front();
        commandBuffer = commandBufferEntry.CommandBuffer;
        m_CommandBufferQueue.pop();
    }

    // If no available command buffer exists, create a new one.
    if ( !commandBuffer )
    {
        std::shared_ptr<DeviceDX12> device = m_Device.lock();
        assert( device );
        
        commandBuffer = std::make_shared<GraphicsCommandBufferDX12>( device, shared_from_this(), m_d3d12CommandListType, 1 );
        m_CommandBufferPool.push_back( commandBuffer );
    }

    commandBuffer->Begin();

    return commandBuffer;
}

uint64_t GraphicsCommandQueueDX12::GetGPUFrequency() const
{
    return m_GPUFrequency;
}

ComPtr<ID3D12CommandQueue> GraphicsCommandQueueDX12::GetD3D12CommandQueue() const
{
    return m_d3d12CommandQueue;
}

D3D12_COMMAND_LIST_TYPE GraphicsCommandQueueDX12::GetD3D12CommandListType() const
{
    return m_d3d12CommandListType;
}
