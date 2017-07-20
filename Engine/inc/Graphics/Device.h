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
 *  @file RenderDevice.h
 *  @date December 28, 2015
 *  @author Jeremiah
 *
 *  @brief Render device interface for resource creation.
 */

#include "../EngineDefines.h"
#include "GraphicsEnums.h"

namespace Graphics
{
    class Adapter;
    class CopyCommandQueue;
    class CopyCommandBuffer;
    class ComputeCommandQueue;
    class ComputeCommandBuffer;
    class GraphicsCommandQueue;
    class GraphicsPipelineState;
    class ComputePipelineState;
    class ConstantBuffer;
    class ByteAddressBuffer;
    class StructuredBuffer;
    class VertexBuffer;
    class IndexBuffer;
    class Scene;
    class Mesh;
    class Material;
    class Texture;
    class Sampler;
    struct TextureFormat;
    class RenderTarget;
    class Shader;
    class ShaderSignature;
    class QueueSemaphore;
    class Query;
    class ReadbackBuffer;
    class IndirectCommandSignature;

    class ENGINE_DLL Device
    {
    public:

        /**
         * Idle the GPU until all command queues have finished executing.
         */
        virtual void IdleGPU() = 0;

        /**
         * Query the graphics adapter that was used to create this device.
         */
        virtual std::shared_ptr<Adapter> GetAdapter() const = 0;

        /**
        * Get the graphics queue.
        * @param index The index of the graphics queue to retrieve. Default is 0.
        * 
        */
        virtual std::shared_ptr<GraphicsCommandQueue> GetGraphicsQueue() const = 0;

        /**
        * Get the compute queue.
        * @param index The index of the compute queue to retrieve. Default is 0.
        */
        virtual std::shared_ptr<ComputeCommandQueue> GetComputeQueue() const = 0;

        /**
        * Get the copy queue.
        * @param index The index of the copy queue to retrieve. Default is 0.
        */
        virtual std::shared_ptr<CopyCommandQueue> GetCopyQueue() const = 0;

        /**
         * Create a queue semaphore object that can be used to synchronize queues.
         */
        virtual std::shared_ptr<QueueSemaphore> CreateQueueSemaphore() = 0;

        /**
         * Create a mesh object.
         */
        virtual std::shared_ptr<Mesh> CreateMesh() = 0;

        /**
         * Create a material object.
         */
        virtual std::shared_ptr<Material> CreateMaterial() = 0;

        /**
         * Create a scene object for loading scene files.
         */
        virtual std::shared_ptr<Scene> CreateScene() = 0;

        /**
         * Create a sphere mesh.
         */
        virtual std::shared_ptr<Scene> CreateSphere( std::shared_ptr<ComputeCommandBuffer> commandBuffer, float radius, float tesselation = 1.0f ) = 0;

        /**
         * Create a cylinder mesh.
         */
        virtual std::shared_ptr<Scene> CreateCylinder( std::shared_ptr<ComputeCommandBuffer> commandBuffer, float baseRadius, float apexRadius, float height, const glm::vec3& axis ) = 0;

        /** 
         * Create a screen-space quad that can be used to render full-screen post-process effects to the screen.
         * By default, the quad will have clip-space coordinates and can be used with a pass-through vertex shader
         * to render full-screen post-process effects. If you want more control over the area of the screen the quad covers, 
         * you can specify your own screen coordinates and supply an appropriate orthographic projection matrix to align the 
         * screen quad appropriately.
         */
        virtual std::shared_ptr<Scene> CreateScreenQuad( std::shared_ptr<ComputeCommandBuffer> commandBuffer, float left = -1.0f, float right = 1.0f, float bottom = -1.0f, float top = 1.0f, float z = 0.0f ) = 0;

        /**
         * Create a shader signature. Required by the pipeline state object.
         */
        virtual std::shared_ptr<ShaderSignature> CreateShaderSignature() = 0;

        /**
         * Create a constant buffer
         */
        virtual std::shared_ptr<ConstantBuffer> CreateConstantBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, size_t bufferSize, const void* bufferData ) = 0;

        template<typename T>
        std::shared_ptr<ConstantBuffer> CreateConstantBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, const T& bufferData )
        {
            return CreateConstantBuffer( copyCommandBuffer, sizeof( T ), &bufferData );
        }

        /**
         * Create a structured buffer.
         */
        virtual std::shared_ptr<StructuredBuffer> CreateStructuredBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, size_t numElements, size_t elementSize, const void* bufferData = nullptr ) = 0;

        template<typename T>
        std::shared_ptr<StructuredBuffer> CreateStructuredBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, const std::vector<T>& bufferData )
        {
            return CreateStructuredBuffer( copyCommandBuffer, bufferData.size(), sizeof( T ), bufferData.data() );
        }

        /**
         * Create a byte address buffer.
         */
        virtual std::shared_ptr<ByteAddressBuffer> CreateByteAddressBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, size_t bufferSize, const void* bufferData = nullptr ) = 0;

        template<typename T>
        std::shared_ptr<ByteAddressBuffer> CreateByteAddressBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, const T& bufferData )
        {
            return CreateByteAddressBuffer( copyCommandBuffer, sizeof( T ), &bufferData );
        }

        /**
         * Create a vertex buffer.
         */
        virtual std::shared_ptr<VertexBuffer> CreateVertexBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, size_t numVertices, size_t vertexStride, const void* vertexData ) = 0;

        template<typename T>
        std::shared_ptr<VertexBuffer> CreateVertexBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, const std::vector<T>& vertexData )
        {
            return CreateVertexBuffer( copyCommandBuffer, vertexData.size(), sizeof( T ), vertexData.data() );
        }

        /**
         * Create an index buffer.
         */
        virtual std::shared_ptr<IndexBuffer> CreateIndexBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, size_t numIndicies, size_t indexSizeInBytes, const void* indexData ) = 0;

        template<typename T>
        std::shared_ptr<IndexBuffer> CreateIndexBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, const std::vector<T>& indexData )
        {
            return CreateIndexBuffer( copyCommandBuffer, indexData.size(), sizeof( T ), indexData.data() );
        }

        /**
         * Create a buffer that can be used to read back data from the GPU.
         * Use CopyCommandBuffer::CopyResource to copy the contents from a GPU buffer into a readback 
         * buffer. Wait for the copy operation to finish (using a fence) before reading the contents of the readback buffer.
         */
        virtual std::shared_ptr<ReadbackBuffer> CreateReadbackBuffer( size_t bufferSize ) = 0;

        /**
         * Create a sampler that can be bound in either a table range or as a static
         * sampler in a root signature.
         */
        virtual std::shared_ptr<Sampler> CreateSampler() = 0;

        /**
         * Create texture from a file.
         */
        virtual std::shared_ptr<Texture> CreateTexture( std::shared_ptr<ComputeCommandBuffer> computeCommandBuffer, const std::wstring& fileName ) = 0;

        /**
         * Create an empty 2D Texture
         * 
         * @param width The width of the texture in pixels.
         * @param height The height of the texture in pixels.
         * @param slices The number of array slices for 2D texture arrays.
         * @param format The internal texture format.
         * @param bUAV Whether this texture will be used as an unordered access view in shaders.
         */
        virtual std::shared_ptr<Texture> CreateTexture2D( uint16_t width, uint16_t height, uint16_t slices, const TextureFormat& format ) = 0;

        /**
         * Create a graphics pipeline state.
         */
        virtual std::shared_ptr<GraphicsPipelineState> CreateGraphicsPipelineState() = 0;

        /**
         * Create a compute pipeline state.
         */
        virtual std::shared_ptr<ComputePipelineState> CreateComputePipelineState() = 0;

        /**
         * Create an indirect command signature.
         */
        virtual std::shared_ptr<IndirectCommandSignature> CreateIndirectCommandSignature() = 0;

        /**
         * Create a render target.
         */
        virtual std::shared_ptr<RenderTarget> CreateRenderTarget() = 0;

        /**
         * Create a shader.
         */
        virtual std::shared_ptr<Shader> CreateShader() = 0;

        /**
         * Create a GPU query.
         */
        virtual std::shared_ptr<Query> CreateQuery( QueryType queryType, uint32_t numQueries ) = 0;
    };
}