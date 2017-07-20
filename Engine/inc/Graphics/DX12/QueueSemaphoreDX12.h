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
 *  @file QueueSemaphoreDX12.h
 *  @date April 1, 2016
 *  @author jeremiah
 *
 *  @brief Queue semaphore for DX12.
 */

#include "../QueueSemaphore.h"

namespace Graphics
{
    class DeviceDX12;

    class QueueSemaphoreDX12 : public QueueSemaphore
    {
    public:
        QueueSemaphoreDX12( std::shared_ptr<DeviceDX12> device );
        virtual ~QueueSemaphoreDX12();

        /**
         * Increment and return the fence value.
         */
        uint64_t IncrementFence();

        Microsoft::WRL::ComPtr<ID3D12Fence> GetD3D12Fence() const;
        uint64_t GetFenceValue() const;

    private:
        std::weak_ptr<DeviceDX12> m_Device;

        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;
        Microsoft::WRL::ComPtr<ID3D12Fence> m_d3d12Fence;
        std::atomic_uint64_t m_FenceValue;
    };
}