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
 *  @file IndexBufferDX12.h
 *  @date January 26, 2016
 *  @author Jeremiah
 *
 *  @brief DX12 Implementation of an index buffer.
 */

#include "../IndexBuffer.h"
#include "BufferDX12.h"

namespace Graphics
{
    class DeviceDX12;

    class IndexBufferDX12 : public virtual IndexBuffer, public BufferDX12
    {
    public:
        IndexBufferDX12( std::shared_ptr<DeviceDX12> device );
        virtual ~IndexBufferDX12();

        virtual void SetName( const std::wstring& name ) override
        {
            BufferDX12::SetName( name );
        }

        virtual ResourceState GetResourceState() const override
        {
            return BufferDX12::GetResourceState();
        }


        /**
        * Get the number of indices of this index buffer.
        */
        virtual size_t GetNumIndices() const override;

        /**
        * Get the size of each index in bytes.
        */
        virtual size_t GetIndexSizeInBytes() const override;

        virtual void CreateViews( size_t numElements, size_t elementSize ) override;
        D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

    protected:

    private:
        // Number of indices in the index buffer.
        size_t m_NumIndices;
        size_t m_BytesPerIndex;

        // Format of the internal index buffer.
        DXGI_FORMAT m_dxgiFormat;
        D3D12_INDEX_BUFFER_VIEW m_d3d12IndexBufferView;
    };
}