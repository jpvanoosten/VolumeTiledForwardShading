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
 *  @file FenceDX12.h
 *  @date January 12, 2016
 *  @author Jeremiah
 *
 *  @brief DX12 Implementation of a fence.
 */

#include "../Fence.h"

namespace Graphics
{
    class GraphicsCommandQueueDX12;

    class FenceDX12 : public Fence
    {
    public:
        FenceDX12( std::shared_ptr<GraphicsCommandQueueDX12> commandQueue, uint64_t fenceValue );
        virtual ~FenceDX12();

        /**
        * Check the fence status.
        */
        virtual FenceStatus GetStatus() const override;

        /**
         * Wait for the fence to complete.
         * @param duration Time to wait in milliseconds. Default if "INFINITE" wait.
         */
        virtual void WaitFor( std::chrono::milliseconds duration = std::chrono::milliseconds::max() ) override;

        uint64_t GetFenceValue() const;

    protected:

    private:
        std::shared_ptr<GraphicsCommandQueueDX12> m_CommandQueue;
        uint64_t m_FenceValue;
    };
}