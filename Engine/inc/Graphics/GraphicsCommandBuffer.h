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
 *  @file GraphicsCommandBuffer.h
 *  @date January 2, 2016
 *  @author Jeremiah
 *
 *  @brief Use a Graphics Command Buffer to record graphics commands, compute commands or copy commands.
 */

#include "ComputeCommandBuffer.h"
#include "GraphicsEnums.h"
#include "ClearColor.h"

namespace Graphics
{
    class Resource;
    class IndexBuffer;
    class VertexBuffer;
    class GraphicsPipelineState;
    class RenderTarget;

    class ENGINE_DLL GraphicsCommandBuffer : public ComputeCommandBuffer
    {
    public:
        /**
         * Bind a single argument to the graphics pipeline.
         * @param slotID The index of the shader signature to bind to.
         * @param shaderArgument The shader argument to bind.
         */
        virtual void BindGraphicsShaderArgument( uint32_t slotID, std::shared_ptr<Resource> shaderArgument ) = 0;

        /**
        * Bind a range of shader arguments to the graphics pipeline.
        * @param slotID The shader signature slot to bind the parameter to.
        * @param offset The staring offset in the table range.
        * @param shaderArguments The shader arguments to bind to the graphics pipeline.
        */
        virtual void BindGraphicsShaderArguments( uint32_t slotID, uint32_t offset, const ShaderArguments& shaderArguments ) = 0;

        /**
         * Bind a render target to the rendering pipeline.
         */
        virtual void BindRenderTarget( std::shared_ptr<RenderTarget> renderTarget ) = 0;

        /**
         * Set the graphics pipeline state object.
         */
        virtual void BindGraphicsPipelineState( std::shared_ptr<GraphicsPipelineState> graphicsPipelineState ) = 0;

        /**
         * Bind a shader signature to the graphics rendering pipeline.
         */
        virtual void BindGraphicsShaderSignature( std::shared_ptr<ShaderSignature> shaderSignature ) = 0;

        /**
         * Bind 32-bit constants to the graphics pipeline.
         */
        virtual void BindGraphics32BitConstants( uint32_t slotID, uint32_t numConstants, const void* constants ) = 0;

        template<typename T>
        void BindGraphics32BitConstants( uint32_t slotID, const T& constants )
        {
            static_assert( sizeof( T ) % sizeof( uint32_t ) == 0, "Size of type must be a multiple of 4 bytes" );
            BindGraphics32BitConstants( slotID, sizeof( T ) / sizeof(uint32_t), &constants );
        }

        /**
        * Bind a dynamic constant buffer data to the graphics pipeline.
        * @param slotID The shader signature slot to bind to.
        * @param bufferSizeInBytes The size in bytes of the constant buffer.
        * @param bufferData A pointer to the constant buffer data.
        */
        virtual void BindGraphicsDynamicConstantBuffer( uint32_t slotID, size_t bufferSizeInBytes, const void* bufferData ) = 0;

        template<typename T>
        void BindGraphicsDynamicConstantBuffer( uint32_t slotID, const T& data )
        {
            BindGraphicsDynamicConstantBuffer( slotID, sizeof( T ), &data );
        }

        /**
         * Bind a dynamic structured buffer data to the graphics pipeline.
         * @param slotID The slot or register to bind the structured buffer to.
         * @param numElements The number of structured elements in the structured buffer.
         * @param elementSize The size of each element in the structured buffer.
         * @param bufferData A pointer to the constant buffer data.
         */
        virtual void BindGraphicsDynamicStructuredBuffer( uint32_t slotID, size_t numElements, size_t elementSize, const void* bufferData ) = 0;

        template<typename T>
        void BindGraphicsDynamicStructuredBuffer( uint32_t slotID, const std::vector<T>& data )
        {
            BindGraphicsDynamicStructuredBuffer( slotID, data.size(), sizeof( T ), data.data() );
        }

        /**
         * Bind an index buffer to the rendering pipeline.
         * @param indexBuffer The index buffer object to bind to the rendering pipeline.
         * @param offset The offset in bytes to the first index.
         */
        virtual void BindIndexBuffer( std::shared_ptr<IndexBuffer> indexBuffer, size_t offset = 0 ) = 0;

        /**
         * Bind a dynamic index buffer to the rendering pipeline.
         */
        virtual void BindDynamicIndexBuffer( size_t numIndicies, size_t indexSizeInBytes, const void* indexData ) = 0;

        template<typename T>
        void BindDynamicIndexBuffer( const std::vector<T>& indexData )
        {
            BindDynamicIndexBuffer( indexData.size(), sizeof( T ), indexData.data() );
        }

        /**
         * Bind a vertex buffer to the rendering pipeline.
         * @param vertexBuffer The vertex buffer to bind to the rendering pipeline.
         * @param slotID The index of the slot to bind the vertex buffer.
         */
        virtual void BindVertexBuffer( uint32_t slotID, std::shared_ptr<VertexBuffer> vertexBuffer ) = 0;

        /**
         * Bind a dynamic vertex buffer to the rendering pipeline.
         */
        virtual void BindDynamicVertexBuffer( uint32_t slotID, size_t numVertices, size_t vertexSizeInBytes, const void* vertexData ) = 0;
        
        template<typename T>
        void BindDynamicVertexBuffer( uint32_t slotID, const std::vector<T>& vertexData )
        {
            BindDynamicVertexBuffer( slotID, vertexData.size(), sizeof( T ), vertexData.data() );
        }
        
        /**
        * Clear the contents of a render target texture.
        */
        virtual void ClearTexture( std::shared_ptr<Texture> texture, const ClearColor& color = ClearColor::Black ) = 0;

        /**
        * Clear the contents of a depth/stencil texture.
        */
        virtual void ClearDepthStencilTexture( std::shared_ptr<Texture> texture, ClearFlags clearFlags = ClearFlags::DepthStencil, float depth = 1.0f, uint8_t stencil = 0 ) = 0;

        /**
        * Clear every texture attached to a render target.
        */
        virtual void ClearRenderTarget( std::shared_ptr<RenderTarget> renderTarget, ClearFlags clearFlags = ClearFlags::All, const ClearColor& color = ClearColor::Black, float depth = 1.0f, uint8_t stencil = 0 ) = 0;

        /**
         * Draw non-indexed instanced.
         * @param vertexCount The number of vertices to send for rendering.
         * @param firstVertex Offset to the first vertex in the currently bound vertex buffer.
         * @param instanceCount The number of instances to draw.
         * @param firstInstance The offset of the first instance to start drawing.
         */
        virtual void Draw( uint32_t vertexCount, uint32_t firstVertex = 0, uint32_t instanceCount = 1, uint32_t firstInstance = 0 ) = 0;

        /**
         * Draw indexed instanced.
         * @param indexCount The number of indices to send for rendering.
         * @param firstIndex Offset to the first index in the currently bound index buffer.
         * @param baseVertex Value to be added to each vertex index.
         * @param instanceCount The number of instances to draw.
         * @param firstInstance The offset of the first instance to start drawing.
         */
        virtual void DrawIndexed( uint32_t indexCount, uint32_t firstIndex = 0, int32_t baseVertex = 0, uint32_t instanceCount = 1, uint32_t firstInstance = 0 ) = 0;
    };
}