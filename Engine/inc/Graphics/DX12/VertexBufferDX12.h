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
 *  @file VertexBufferDX12.h
 *  @date January 12, 2016
 *  @author Jeremiah
 *
 *  @brief DirectX 12 Implementation of a vertex buffer.
 */

#include "BufferDX12.h"
#include "../VertexBuffer.h"

namespace Graphics
{
    class DeviceDX12;

    class VertexBufferDX12 : public BufferDX12, public virtual VertexBuffer
    {
    public:
        VertexBufferDX12( std::shared_ptr<DeviceDX12> device );
        virtual ~VertexBufferDX12();

        virtual void SetName( const std::wstring& name ) override
        {
            BufferDX12::SetName( name );
        }

        virtual ResourceState GetResourceState() const
        {
            return BufferDX12::GetResourceState();
        }

        virtual size_t GetVertexCount() const override;
        virtual size_t GetVertexStride() const override;

        virtual void CreateViews( size_t numElements, size_t elementSize ) override;
        D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
    protected:

    private:
        D3D12_VERTEX_BUFFER_VIEW m_d3d12VertexBufferView;

        size_t m_NumVertices;
        size_t m_VertexStride;
    };
}