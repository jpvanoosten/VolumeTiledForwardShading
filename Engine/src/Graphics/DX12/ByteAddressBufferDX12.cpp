#include <EnginePCH.h>

#include <Graphics/DX12/ByteAddressBufferDX12.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>
#include <Common.h>

using namespace Graphics;

ByteAddressBufferDX12::ByteAddressBufferDX12( std::shared_ptr<DeviceDX12> device )
    : BufferDX12( device )
    , m_BufferSize( 0 )
    , m_d3d12ShaderResourceView( {0} )
    , m_d3d12UniformAccessView( {0} )
{
    m_d3d12ShaderResourceView = device->AllocateDescriptors( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
    m_d3d12UniformAccessView = device->AllocateDescriptors( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
}

ByteAddressBufferDX12::~ByteAddressBufferDX12()
{}

size_t ByteAddressBufferDX12::GetBufferSize() const
{
    return m_BufferSize;
}

void ByteAddressBufferDX12::CreateViews( size_t numElements, size_t elementSize )
{
    // Make sure buffer size is aligned to 4 bytes.
    m_BufferSize = Math::AlignUp( numElements * elementSize, 4 );

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.NumElements = (UINT)m_BufferSize / 4;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

    m_d3d12Device->CreateShaderResourceView( m_d3d12Resource.Get(), &srvDesc, m_d3d12ShaderResourceView );

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    uavDesc.Buffer.NumElements = (UINT)m_BufferSize / 4;
    uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

    m_d3d12Device->CreateUnorderedAccessView( m_d3d12Resource.Get(), nullptr, &uavDesc, m_d3d12UniformAccessView );
}

D3D12_CPU_DESCRIPTOR_HANDLE ByteAddressBufferDX12::GetShaderResourceView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t )
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

D3D12_CPU_DESCRIPTOR_HANDLE ByteAddressBufferDX12::GetUnorderedAccessView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t )
{
    if ( commandBuffer )
    {
        commandBuffer->TransitionResoure( shared_from_this(), ResourceState::UAV );
    }
    return m_d3d12UniformAccessView;
}
