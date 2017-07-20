#pragma once

#include "../StructuredBuffer.h"
#include "BufferDX12.h"

namespace Graphics
{
    class DeviceDX12;
    class ByteAddressBufferDX12;

    class StructuredBufferDX12 : public BufferDX12, public virtual StructuredBuffer, public std::enable_shared_from_this<StructuredBufferDX12>
    {
    public:
        StructuredBufferDX12( std::shared_ptr<DeviceDX12> device, std::shared_ptr<CopyCommandBuffer> copyCommandBuffer );
        virtual ~StructuredBufferDX12();

        virtual void SetName( const std::wstring& name ) override
        {
            BufferDX12::SetName( name );
        }

        virtual ResourceState GetResourceState() const
        {
            return BufferDX12::GetResourceState();
        }

        /**
         * Get the number of elements contained in this buffer.
         */
        virtual size_t GetNumElements() const override;

        /**
         * Get the size in bytes of each element in this buffer.
         */
        virtual size_t GetElementSize() const override;

        /**
         * Get the buffer that stores the internal counter of the structured buffer.
         */
        virtual std::shared_ptr<ByteAddressBuffer> GetCounterBuffer() const override;


        virtual void CreateViews( size_t numElements, size_t elementSize ) override;

        virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t index = 0 ) override;
        virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t index = 0 ) override;

    protected:

    private:

        size_t m_NumElements;
        size_t m_ElementSize;

        D3D12_CPU_DESCRIPTOR_HANDLE m_d3d12ShaderResourceView;
        D3D12_CPU_DESCRIPTOR_HANDLE m_d3d12UniformAccessView;

        std::shared_ptr<ByteAddressBufferDX12> m_CounterBuffer;
    };
}
