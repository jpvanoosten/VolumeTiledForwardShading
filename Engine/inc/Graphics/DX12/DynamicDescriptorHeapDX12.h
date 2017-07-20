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
 *  @file DynamicDescriptorsDX12.h
 *  @date March 23, 2016
 *  @author Jeremiah
 *
 *  @brief Dynamic descriptor allocator for uploading GPU visible descriptors.
 *  Used internally by the GraphicsCommandBuffer.
 */

#include "../GraphicsEnums.h"

namespace Graphics
{
    class Resource;
    class ResourceDX12;
    class SamplerDX12;
    class ShaderParameter;
    class ShaderSignatureDX12;
    class GraphicsCommandBufferDX12;
    
    using ShaderArguments = std::vector< std::shared_ptr<Resource> >;

    /**
     * Maximum number of descriptors that can be copied in a single root signature.
     * If a root signature has multiple table ranges, the sum of all ranges must not
     * be more than MAX_DESCRIPTORS_PER_TABLE.
     */
    constexpr uint32_t MAX_NUM_DESCRIPTORS_PER_ROOT_SIGNATURE = 1024;

    /**
     * Maximum number of root parameter indices.
     */
    constexpr uint32_t MAX_ROOT_PARAMETERS = 16;

    /**
     * Maximum number of descriptor ranges per root descriptor table.
     */
    constexpr uint32_t MAX_DESCRIPTOR_RANGES_PER_ROOT_DESCRIPTOR_TABLE = 16;

    class DynamicDescriptorHeapDX12
    {
    public:
        DynamicDescriptorHeapDX12( Microsoft::WRL::ComPtr<ID3D12Device> d3d12Device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap = MAX_NUM_DESCRIPTORS_PER_ROOT_SIGNATURE );
        virtual ~DynamicDescriptorHeapDX12();

        /**
         * Copy the descriptors to a cached descriptor location.
         */
        void StageDescriptors( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, Pipeline pipeline, uint32_t rootParameterIndex, uint32_t offset, const ShaderArguments& shaderArguments );
        void StageDescriptors( Pipeline pipeline, uint32_t rootParameterIndex, uint32_t offset, uint32_t numDescriptors, D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptor );

        /**
         * Copy all of the staged descriptors to the GPU visible descriptor heap 
         * and bind the descriptor tables to the command buffer.
         */
        void CopyAndBindStagedDescriptors( Pipeline pipeline, std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer );

        /**
         * Directly copy a CPU descriptor handle to a GPU visible descriptor.
         */
        D3D12_GPU_DESCRIPTOR_HANDLE CopyDescriptor( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptor );
        
        /**
         * Free used descriptors and descriptor heaps.
         * This function doesn't actually free the memory. It just resets the descriptor
         * heap, handles and it marks all of the heaps as "available" again.
         * Should only be called when the command buffer is finished rendering.
         */
        void Free();

        /**
         * Parse the shader signature to determine the descriptor layouts.
         * Changing the shader signature will invalidate the descriptor cache.
         */
        void SetShaderSignature( Pipeline pipeline, std::shared_ptr<ShaderSignatureDX12> shaderSignature );

        // Retrieve the current descriptor heap.
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetCurrentHeap() const;

    protected:
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RequestDescriptorHeap();
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors );

    private:
        /**
         * Descriptor table cache is used to stage descriptors until they need
         * to be copied to a GPU visible descriptor heap.
         */
        struct DescriptorTableCache
        {
            struct DescriptorRange
            {
                D3D12_DESCRIPTOR_RANGE_TYPE RangeType;
                uint32_t NumDescriptors;
            };

            DescriptorTableCache()
                : Descriptors( nullptr )
                , NumDescriptors( 0 )
                , NumDescriptorRanges( 0 )
            {}

            /**
            * Get the descriptor type.
            * @param offset The descriptor offset from the table start.
            */
            D3D12_DESCRIPTOR_RANGE_TYPE GetDescriptorRangeType( uint32_t offset ) const;

            void operator()( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t index, std::shared_ptr<ResourceDX12> resource, uint32_t subresource = 0 );
            void operator()( uint32_t index, std::shared_ptr<SamplerDX12> sampler );
            void operator()( uint32_t index, D3D12_CPU_DESCRIPTOR_HANDLE hCPU );

            D3D12_CPU_DESCRIPTOR_HANDLE* Descriptors;
            uint32_t NumDescriptors;

            uint32_t NumDescriptorRanges;
            DescriptorRange DescriptorRanges[MAX_DESCRIPTOR_RANGES_PER_ROOT_DESCRIPTOR_TABLE];
        };

        static DescriptorTableCache::DescriptorRange TranslateDescriptorRange( const ShaderParameter& shaderParameter );
        uint32_t ComputeNumDescriptors( Pipeline pipeline ) const;

        using DescriptorHeapPool = std::queue< Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> >;

        DescriptorTableCache m_DescriptorTableCache[2][MAX_ROOT_PARAMETERS];
        D3D12_CPU_DESCRIPTOR_HANDLE m_DescriptorHandleCache[MAX_NUM_DESCRIPTORS_PER_ROOT_SIGNATURE];

        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CurrentDescriptorHeap;
        CD3DX12_GPU_DESCRIPTOR_HANDLE m_CurrentGPUDescriptorHandle;
        CD3DX12_CPU_DESCRIPTOR_HANDLE m_CurrentCPUDescriptorHandle;

        D3D12_DESCRIPTOR_HEAP_TYPE m_DescriptorHeapType;

        std::mutex m_Mutex;

        DescriptorHeapPool m_DescriptorHeapPool;
        DescriptorHeapPool m_AvailableDescriptorHeaps;
        uint32_t m_NumDescriptorsPerHeap;
        uint32_t m_DescriptorSize;
        uint32_t m_NumFreeHandles;

        // Used to mark which root parameters are being updated and need to be 
        // set on the command list.
        // Set if a descriptor table cache has more than 0 descriptors and is stale.
        uint32_t m_StaleRootParameterBitMask[2];

        std::vector<Microsoft::WRL::ComPtr<ID3D12Object> > m_ReferencedObjects;
    };
}