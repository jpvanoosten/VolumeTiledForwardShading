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
 *  @file DynamicBufferDX12.h
 *  @date February 29, 2016
 *  @author Jeremiah
 *
 *  @brief Dynamic buffers are used for dynamic buffer allocation (Constant, Vertex, and Index buffers).
 *  @description This is an internal class only used by the HeapAllocator and GraphicsCommandBuffer.
 */

#include "BufferDX12.h"

namespace Graphics
{
    class DeviceDX12;

    class DynamicBufferDX12 : public BufferDX12
    {
    public:
        DynamicBufferDX12( std::shared_ptr<DeviceDX12> device, 
                           Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource, 
                           D3D12_RESOURCE_STATES state );
        virtual ~DynamicBufferDX12();

    private:
        friend class HeapAllocatorDX12;

        void* m_DataPtr;
    };
}