#include <EnginePCH.h>

#include <Graphics/DX12/StructuredBufferDX12.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>
#include <Graphics/DX12/ByteAddressBufferDX12.h>

using namespace Graphics;

StructuredBufferDX12::StructuredBufferDX12( std::shared_ptr<DeviceDX12> device, std::shared_ptr<CopyCommandBuffer> copyCommandBuffer )
    : BufferDX12( device )
    , m_NumElements( 0 )
    , m_ElementSize( 0 )
    , m_d3d12ShaderResourceView( {0} )
    , m_d3d12UniformAccessView( {0} )
{
    // Create a 32-bit raw buffer to store the internal counter for the structured buffer.
    m_CounterBuffer = std::dynamic_pointer_cast<ByteAddressBufferDX12>( device->CreateByteAddressBuffer( copyCommandBuffer, 4, nullptr ) );
    m_CounterBuffer->SetName( L"StructuredBufferDX12::Counter" );

    m_d3d12ShaderResourceView = device->AllocateDescriptors( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
    m_d3d12UniformAccessView = device->AllocateDescriptors( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
}

StructuredBufferDX12::~StructuredBufferDX12()
{

}

size_t StructuredBufferDX12::GetNumElements() const
{
    return m_NumElements;
}

size_t StructuredBufferDX12::GetElementSize() const
{
    return m_ElementSize;
}

std::shared_ptr<ByteAddressBuffer> StructuredBufferDX12::GetCounterBuffer() const
{
    return m_CounterBuffer;
}

void StructuredBufferDX12::CreateViews( size_t numElements, size_t elementSize )
{
    m_NumElements = numElements;
    m_ElementSize = elementSize;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.NumElements = static_cast<UINT>( m_NumElements );
    srvDesc.Buffer.StructureByteStride = static_cast<UINT>( m_ElementSize );
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

    m_d3d12Device->CreateShaderResourceView( m_d3d12Resource.Get(), &srvDesc, m_d3d12ShaderResourceView );

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.Buffer.CounterOffsetInBytes = 0;
    uavDesc.Buffer.NumElements = static_cast<UINT>( m_NumElements );
    uavDesc.Buffer.StructureByteStride = static_cast<UINT>( m_ElementSize );
    uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

    if ( m_d3d12Resource )
    {
        m_d3d12Device->CreateUnorderedAccessView( m_d3d12Resource.Get(), m_CounterBuffer->GetD3D12Resource().Get(), &uavDesc, m_d3d12UniformAccessView );
    }
    else
    {
        m_d3d12Device->CreateUnorderedAccessView( nullptr, nullptr, &uavDesc, m_d3d12UniformAccessView );
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE StructuredBufferDX12::GetShaderResourceView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t )
{
    if ( commandBuffer )
    {
        if ( commandBuffer->GetD3D12CommandListType() == D3D12_COMMAND_LIST_TYPE_COMPUTE )
        {
            commandBuffer->TransitionResoure( shared_from_this(), ResourceState::NonPixelShader );
        }
        else
        {
            commandBuffer->TransitionResoure( shared_from_this(), ResourceState::PixelShader | ResourceState::NonPixelShader );
        }
    }
    return m_d3d12ShaderResourceView;
}

D3D12_CPU_DESCRIPTOR_HANDLE StructuredBufferDX12::GetUnorderedAccessView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t )
{
    if ( commandBuffer )
    {
        commandBuffer->TransitionResoure( shared_from_this(), ResourceState::UAV );
    }
    return m_d3d12UniformAccessView;
}

