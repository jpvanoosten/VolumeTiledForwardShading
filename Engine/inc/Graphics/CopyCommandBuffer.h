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
 *  @file CopyCommandBuffer.h
 *  @date January 2, 2016
 *  @author Jeremiah
 *
 *  @brief Use a copy command buffer to record resource copy operations.
 */

#include "CommandBuffer.h"

namespace Graphics
{
    class VertexBuffer;
    class IndexBuffer;
    class ConstantBuffer;
    class ByteAddressBuffer;
    class StructuredBuffer;
    class Texture;

    class ENGINE_DLL CopyCommandBuffer : public CommandBuffer
    {
    public:
        /**
         * Set the contents of a vertex buffer.
         */
        virtual void SetVertexBuffer( std::shared_ptr<VertexBuffer> vertexBuffer, size_t numVertices, size_t vertexStride, const void* bufferData ) = 0;

        /**
         * Set the contents of an index buffer.
         */
        virtual void SetIndexBuffer( std::shared_ptr<IndexBuffer> indexBuffer, size_t numIndicies, size_t indexSizeInBytes, const void * indexData ) = 0;

        /**
         * Set the contents of a constant buffer.
         */
        virtual void SetConstantBuffer( std::shared_ptr<ConstantBuffer> constantBuffer, size_t sizeInBytes, const void* bufferData ) = 0;

        /**
         * Set the contents of a byte address buffer.
         */
        virtual void SetByteAddressBuffer( std::shared_ptr<ByteAddressBuffer> byteAddressBuffer, size_t sizeInBytes, const void* bufferData ) = 0;

        /**
         * Set the contents of a structured buffer.
         */
        virtual void SetStructuredBuffer( std::shared_ptr<StructuredBuffer> structuredBuffer, size_t numElements, size_t elementSize, const void* bufferData ) = 0;

        /**
         * Set the contents of a texture (sub) resource.
         */
        virtual void SetTextureSubresource( std::shared_ptr<Texture> texture, uint32_t mip, uint32_t arraySlice, const void* pTextureData ) = 0;

        /**
         * Copy the contents of one texture to another.
         */
        virtual void CopyResource( std::shared_ptr<Resource> dstResource, std::shared_ptr<Resource> srcResource ) = 0;

        /**
         * Resolve a multi-sampled resource into a single-sampled resource.
         */
        virtual void ResolveMultisampleTexture( std::shared_ptr<Texture> dstTexture, uint32_t mip, uint32_t arraySlice, std::shared_ptr<Texture> srcTexture ) = 0;
    };
}