#pragma once

/*
 *  Copyright(c) 2015 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

/**
 *  @file GraphicsCommandQueueDX12.h
 *  @date January 2, 2016
 *  @author Jeremiah
 *
 *  @brief Graphics command queue for DX12. Implemented using ID3D12CommandQueue.
 */

#include "../GraphicsCommandQueue.h"
#include "../../ThreadSafeQueue.h"

namespace Graphics
{
    class DeviceDX12;
    class FenceDX12;
    class GraphicsCommandBufferDX12;

    class GraphicsCommandQueueDX12 : public GraphicsCommandQueue, public std::enable_shared_from_this<GraphicsCommandQueueDX12>
    {
    public:
        GraphicsCommandQueueDX12( std::shared_ptr<DeviceDX12> device,
                                  D3D12_COMMAND_LIST_TYPE type,
                                  INT priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
                                  D3D12_COMMAND_QUEUE_FLAGS flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
                                  UINT nodeMask = 1 );

        virtual ~GraphicsCommandQueueDX12();

        /**
        * Insert a semaphore signal into the GPU queue.
        */
        virtual void Signal( std::shared_ptr<QueueSemaphore> queueSemaphore ) override;

        /**
        * Insert a semaphore wait into the GPU queue.
        */
        virtual void Wait( std::shared_ptr<QueueSemaphore> queueSemaphore ) override;

        /**
        * Signal the queue. The fence that is returned can be waited for.
        */
        virtual std::shared_ptr<Fence> Signal() override;

        /**
        * Submit a list of command buffers to the queue.
        */
        virtual std::shared_ptr<Fence> Submit( const CommandBufferList& commandBuffers ) override;

        /**
        * Submit a single command buffer.
        */
        virtual std::shared_ptr<Fence> Submit( std::shared_ptr<CommandBuffer> commandBuffer ) override;

        /**
         * Flush all commands currently queued and wait for completion.
         */
        virtual void Idle() override;

        /**
        * Creates a copy command buffer that can be used to record copy commands to the queue.
        */
        virtual std::shared_ptr<CopyCommandBuffer> GetCopyCommandBuffer() override;

        /**
        * Creates a compute command buffer that can be used to submit compute commands to the queue.
        */
        virtual std::shared_ptr<ComputeCommandBuffer> GetComputeCommandBuffer() override;

        /**
        * Creates a graphics command buffer that can be used to submit graphics commands to the queue.
        */
        virtual std::shared_ptr<GraphicsCommandBuffer> GetGraphicsCommandBuffer() override;

        /**
         * Get the GPU frequency of queries running on this queue.
         */
        virtual uint64_t GetGPUFrequency() const override;
        
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue() const;
        D3D12_COMMAND_LIST_TYPE GetD3D12CommandListType() const;

        uint64_t GetCompletedFenceValue() const;
        bool IsFenceComplete( uint64_t fenceValue );
        void WaitForFence( uint64_t fenceValue, std::chrono::milliseconds duration = std::chrono::milliseconds::max() );

    protected:
        // Create a command buffer matching the queue type.
        std::shared_ptr<GraphicsCommandBufferDX12> GetCommandBuffer();

    private:
        struct CommandBufferEntry
        {
            uint64_t FenceValue;
            std::shared_ptr<GraphicsCommandBufferDX12> CommandBuffer;
        };

        using CommandBufferQueue = std::queue< CommandBufferEntry >;
        using CommandBufferPool = std::vector< std::shared_ptr<GraphicsCommandBufferDX12> >;

        // A queue of available command buffers.
        CommandBufferQueue m_CommandBufferQueue;
        CommandBufferPool m_CommandBufferPool;

        std::weak_ptr<DeviceDX12> m_Device;

        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3d12CommandQueue;
        Microsoft::WRL::ComPtr<ID3D12Fence> m_d3d12Fence;
        HANDLE m_FenceEventHandle;
        std::atomic_uint64_t m_FenceValue;
        D3D12_COMMAND_LIST_TYPE m_d3d12CommandListType;
        INT m_Priority;
        D3D12_COMMAND_QUEUE_FLAGS m_Flags;
        UINT m_NodeMask;

        std::mutex m_Mutex;
        std::mutex m_FenceMutex;
        uint64_t m_GPUFrequency;
    };
}