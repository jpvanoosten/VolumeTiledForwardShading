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
 *  @file HeapAllocatorDX12.h
 *  @date February 29, 2016
 *  @author Jeremiah
 *
 *  @brief Dynamic heap allocator.
 */

#include "../../EngineDefines.h"

namespace Graphics
{
    class DeviceDX12;
    class DynamicBufferDX12;

    struct HeapAllocation
    {
        void* CpuPtr;
        D3D12_GPU_VIRTUAL_ADDRESS GpuAddress;
    };

    class HeapAllocatorDX12
    {
    public:
        HeapAllocatorDX12( std::shared_ptr<DeviceDX12> device, 
                           D3D12_HEAP_TYPE d3d12HeapType,
                           uint64_t pageSize = _2MB, 
                           D3D12_RESOURCE_FLAGS d3d12ResourceFlags = D3D12_RESOURCE_FLAG_NONE,
                           D3D12_RESOURCE_STATES d3d12ResourceState = D3D12_RESOURCE_STATE_GENERIC_READ );
        virtual ~HeapAllocatorDX12();

        HeapAllocation Allocate( size_t sizeInBytes, size_t alignment );

        /**
         * Release allocated buffers.
         * Should only be done if the CommandBuffer is finished with them.
         */
        void Free();

    protected:
        std::shared_ptr<DynamicBufferDX12> RequestBuffer();
        std::shared_ptr<DynamicBufferDX12> CreateBuffer();

    private:
        using DynamicBufferPool = std::queue< std::shared_ptr<DynamicBufferDX12> >;

        std::weak_ptr<DeviceDX12> m_Device;
        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;

        uint64_t m_PageSize;
        D3D12_HEAP_TYPE m_d3d12HeapType;
        D3D12_RESOURCE_FLAGS m_d3d12ResourceFlags;
        D3D12_RESOURCE_STATES m_d3d12ResouceState;
        
        DynamicBufferPool m_BufferPool;
        DynamicBufferPool m_AvailableBuffers;

        std::shared_ptr<DynamicBufferDX12> m_CurrentBuffer;
        size_t m_CurrentOffset;

        std::mutex m_Mutex;
    };
}