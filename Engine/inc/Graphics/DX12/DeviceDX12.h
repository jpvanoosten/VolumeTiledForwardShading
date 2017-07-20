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
 *  @file RenderDeviceDX12.h
 *  @date December 28, 2015
 *  @author Jeremiah
 *
 *  @brief D3D12 Render Device.
 */


#include "../Device.h"
#include "../../ThreadSafeQueue.h"

namespace Graphics
{
    class AdapterDXGI;
    class CopyCommandQueueDX12;
    class ComputeCommandQueueDX12;
    class GraphicsCommandQueueDX12;
    class DescriptorAllocatorDX12;
    class ComputePipelineStateDX12;

    class DeviceDX12 : public Device, public std::enable_shared_from_this<DeviceDX12>
    {
    public:
        DeviceDX12( Microsoft::WRL::ComPtr<ID3D12Device> device, std::shared_ptr<AdapterDXGI> adapter );
        virtual ~DeviceDX12();

        /**
         * Idle the GPU until all command queues have finished executing.
         */
        virtual void IdleGPU() override;

        virtual std::shared_ptr<Adapter> GetAdapter() const override;

        /**
        * Get the graphics queue.
        * @param index The index of the graphics queue to retrieve. Default is 0.
        *
        */
        virtual std::shared_ptr<GraphicsCommandQueue> GetGraphicsQueue() const override;

        /**
        * Get the compute queue.
        * @param index The index of the compute queue to retrieve. Default is 0.
        */
        virtual std::shared_ptr<ComputeCommandQueue> GetComputeQueue() const override;

        /**
        * Get the copy queue.
        * @param index The index of the copy queue to retrieve. Default is 0.
        */
        virtual std::shared_ptr<CopyCommandQueue> GetCopyQueue() const override;

        /**
        * Create a queue semaphore object that can be used to synchronize queues.
        */
        virtual std::shared_ptr<QueueSemaphore> CreateQueueSemaphore() override;

        /**
        * Create a mesh object.
        */
        virtual std::shared_ptr<Mesh> CreateMesh() override;

        /**
        * Create a material object.
        */
        virtual std::shared_ptr<Material> CreateMaterial() override;

        /**
         * Create a scene object for loading scene files.
         */
        virtual std::shared_ptr<Scene> CreateScene() override;

        /**
         * Create a sphere mesh.
         */
        virtual std::shared_ptr<Scene> CreateSphere( std::shared_ptr<ComputeCommandBuffer> commandBuffer, float radius, float tesselation = 1.0f ) override;

        /**
         * Create a cylinder mesh.
         */
        virtual std::shared_ptr<Scene> CreateCylinder( std::shared_ptr<ComputeCommandBuffer> commandBuffer, float baseRadius, float apexRadius, float height, const glm::vec3& axis ) override;

        /**
         * Create a screen-space quad that can be used to render full-screen post-process effects to the screen.
         * By default, the quad will have clip-space coordinates and can be used with a pass-through vertex shader
         * to render full-screen post-process effects. If you want more control over the area of the screen the quad covers,
         * you can specify your own screen coordinates and supply an appropriate orthographic projection matrix to align the
         * screen quad appropriately.
         */
        virtual std::shared_ptr<Scene> CreateScreenQuad( std::shared_ptr<ComputeCommandBuffer> commandBuffer, float left = -1.0f, float right = 1.0f, float bottom = -1.0f, float top = 1.0f, float z = 0.0f ) override;

        /**
        * Create a shader signature. Required by the pipeline state object.
        */
        virtual std::shared_ptr<ShaderSignature> CreateShaderSignature() override;

        /**
         * Create a constant buffer
         */
        virtual std::shared_ptr<ConstantBuffer> CreateConstantBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, size_t bufferSize, const void* bufferData ) override;

        /**
         * Create a structured buffer.
         */
        virtual std::shared_ptr<StructuredBuffer> CreateStructuredBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, size_t numElements, size_t elementSize, const void* bufferData = nullptr ) override;

        /**
         * Create a byte address buffer.
         */
        virtual std::shared_ptr<ByteAddressBuffer> CreateByteAddressBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, size_t bufferSize, const void* bufferData = nullptr ) override;

        /**
         * Create a vertex buffer.
         */
        virtual std::shared_ptr<VertexBuffer> CreateVertexBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, size_t numVertices, size_t vertexStride, const void* vertexData ) override;

        /**
         * Create an index buffer.
         */
        virtual std::shared_ptr<IndexBuffer> CreateIndexBuffer( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, size_t numIndicies, size_t indexSizeInBytes, const void* indexData ) override;

        /**
         * Create a buffer that can be used to read back data from the GPU.
         * Use CopyCommandBuffer::CopyResource to copy the contents from a GPU buffer into a readback
         * buffer. Wait for the copy operation to finish (using a fence) before reading the contents of the readback buffer.
         */
        virtual std::shared_ptr<ReadbackBuffer> CreateReadbackBuffer( size_t bufferSize ) override;

        /**
         * Create a sampler that can be bound in either a table range or as a static
         * sampler in a root signature.
         */
        virtual std::shared_ptr<Sampler> CreateSampler() override;

        /**
         * Create texture from a file.
         */
        virtual std::shared_ptr<Texture> CreateTexture( std::shared_ptr<ComputeCommandBuffer> computeCommandBuffer, const std::wstring& fileName ) override;


        /**
         * Create an empty 2D Texture
         */
        virtual std::shared_ptr<Texture> CreateTexture2D( uint16_t width, uint16_t height, uint16_t slices, const TextureFormat& format ) override;

        /**
        * Create a graphics pipeline state.
        */
        virtual std::shared_ptr<GraphicsPipelineState> CreateGraphicsPipelineState() override;

        /**
        * Create a compute pipeline state.
        */
        virtual std::shared_ptr<ComputePipelineState> CreateComputePipelineState() override;

        /**
         * Create an indirect command signature.
         */
        virtual std::shared_ptr<IndirectCommandSignature> CreateIndirectCommandSignature() override;

        /**
         * Create a render target.
         */
        virtual std::shared_ptr<RenderTarget> CreateRenderTarget() override;

        /**
        * Create a shader.
        */
        virtual std::shared_ptr<Shader> CreateShader() override;

        /**
        * Create a GPU query.
        */
        virtual std::shared_ptr<Query> CreateQuery( QueryType queryType, uint32_t numQueries ) override;


        void Init();

        struct GenerateMips
        {
            enum GenerateMipsEnum   // C++11 scoped enums don't allow them to be used as array indices.
            {
                WidthHeightEven = 0,
                WidthOddHeightEven = 1,
                WidthEvenHeightOdd = 2,
                WidthHeightOdd = 3,
                NumVariations,
            };
        };
        std::shared_ptr<ComputePipelineStateDX12> GetMipsPipelineState( uint32_t variation ) const;

        Microsoft::WRL::ComPtr<ID3D12Device> GetD3D12Device() const;
        D3D12_CPU_DESCRIPTOR_HANDLE AllocateDescriptors( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors = 1 );
        DXGI_SAMPLE_DESC GetMultisampleQualityLevels( DXGI_FORMAT format, UINT numSamples, D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE ) const;

    protected:

    private:
        using TextureMap = std::map< std::wstring, std::shared_ptr<Texture> >;

        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;
        uint32_t m_NodeCount;
        D3D12_FEATURE_DATA_D3D12_OPTIONS m_FeatureOptions;

        // The adapter that was used to create this device.
        std::weak_ptr<AdapterDXGI> m_Adapter;

        // A device can have multiple graphics/compute/copy queues.
        std::shared_ptr<GraphicsCommandQueueDX12> m_CopyQueue;
        std::shared_ptr<GraphicsCommandQueueDX12> m_ComputeQueue;
        std::shared_ptr<GraphicsCommandQueueDX12> m_GraphicsQueue;

        // A compute pipeline state for generating mipmaps.
        std::shared_ptr<ComputePipelineStateDX12> m_GenerateMipsPSO[GenerateMips::NumVariations];

        std::unique_ptr<DescriptorAllocatorDX12> m_DescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
        
        TextureMap m_TextureMap;
    };
}
