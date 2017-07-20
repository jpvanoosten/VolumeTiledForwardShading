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
 *  @file CommandQueue.h
 *  @date December 28, 2015
 *  @author Jeremiah
 *
 *  @brief The CommandQueue is the base class for the specific queue types.
 *  There are 3 queue types
 *  <ol>
 *  <li>Copy Command Queue: Execute copy command buffers</li>
 *  <li>Compute Command Queue: Execute copy, and compute command buffers</li>
 *  <li>Graphics Command Queue: Execute graphics, compute, and copy command buffers</li>
 *  </ol>
 */

#include "../EngineDefines.h"

namespace Graphics
{
    class QueueSemaphore;
    class CommandBuffer;
    class Fence;

    using CommandBufferList = std::vector< std::shared_ptr<CommandBuffer> >;

    class ENGINE_DLL CommandQueue
    {
    public:
        /**
         * Insert a semaphore signal into the GPU queue.
         */
        virtual void Signal( std::shared_ptr<QueueSemaphore> queueSemaphore ) = 0;

        /**
         * Insert a semaphore wait into the GPU queue.
         */
        virtual void Wait( std::shared_ptr<QueueSemaphore> queueSemaphore ) = 0;

        /**
         * Signal the queue. The fence that is returned can be waited for.
         */
        virtual std::shared_ptr<Fence> Signal() = 0;

        /**
         * Flush all commands currently queued and wait for completion.
         */
        virtual void Idle() = 0;

        /**
         * Submit a single command buffer.
         * @return A fence object that can be used to synchronize submissions
         * to the command queue.
         */
        virtual std::shared_ptr<Fence> Submit( std::shared_ptr<CommandBuffer> commandBuffer ) = 0;

        /**
         * Submit a list of command buffers to the queue.
         * @return A fence object that can be used to synchronize submissions 
         * to the command queue.
         */
        virtual std::shared_ptr<Fence> Submit( const CommandBufferList& commandBuffers ) = 0;

    };
}