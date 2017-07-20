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
 *  @file ByteAddressBufferDX12.h
 *  @date April 29, 2016
 *  @author jeremiah
 *
 *  @brief Byte address buffer that can be bound to a ByteAddressBuffer or RWByteAddressBuffer in an HLSL shader.
 */

#include "../ByteAddressBuffer.h"
#include "BufferDX12.h"

namespace Graphics
{
    class DeviceDX12;

    class ByteAddressBufferDX12 : public BufferDX12, public virtual ByteAddressBuffer, public std::enable_shared_from_this<ByteAddressBufferDX12>
    {
    public:
        ByteAddressBufferDX12( std::shared_ptr<DeviceDX12> device );
        virtual ~ByteAddressBufferDX12();

        virtual void SetName( const std::wstring& name ) override
        {
            BufferDX12::SetName( name );
        }

        virtual ResourceState GetResourceState() const
        {
            return BufferDX12::GetResourceState();
        }
        
        /**
         * Get the size of the buffer in bytes.
         */
        virtual size_t GetBufferSize() const override;

        virtual void CreateViews( size_t numElements, size_t elementSize ) override;

        virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t index = 0 ) override;
        virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t index = 0 ) override;

    protected:

    private:
        size_t m_BufferSize;

        D3D12_CPU_DESCRIPTOR_HANDLE m_d3d12ShaderResourceView;
        D3D12_CPU_DESCRIPTOR_HANDLE m_d3d12UniformAccessView;
    };
}