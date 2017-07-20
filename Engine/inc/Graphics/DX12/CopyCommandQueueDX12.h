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
 *  @file CopyCommandQueueDX12.h
 *  @date January 2, 2016
 *  @author Jeremiah
 *
 *  @brief Copy command queue in DX12 is implemented using a ID3D12GraphicsCommandList.
 */

#include "../CopyCommandQueue.h"
#include "CommandQueueDX12.h"

#include "../../EnableSharedFromThis.h"

namespace Graphics
{
    class DeviceDX12;

    class CopyCommandQueueDX12 : public CommandQueueDX12, public virtual CopyCommandQueue, public Core::EnableSharedFromThis<CopyCommandQueueDX12>
    {
    public:

        CopyCommandQueueDX12( std::shared_ptr<DeviceDX12> device,
                              D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_COPY,
                              INT priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
                              D3D12_COMMAND_QUEUE_FLAGS flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
                              UINT nodeMask = 1 );
        virtual ~CopyCommandQueueDX12(); 

        /**
        * Creates a copy command buffer that can be used to record copy commands to the queue.
        */
        virtual std::shared_ptr<CopyCommandBuffer> CreateCopyCommandBuffer() override;

    protected:

    private:
        using CopyCommandBufferList = std::vector< std::shared_ptr<CopyCommandBuffer> >;
        
        CopyCommandBufferList m_CopyCommandBuffers;
        std::mutex m_CommandBufferListMutex;
    };
}