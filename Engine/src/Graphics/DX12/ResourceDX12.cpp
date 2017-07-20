#include <EnginePCH.h>

#include <Graphics/DX12/ResourceDX12.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>

using namespace Graphics;
using namespace Microsoft::WRL;

ResourceState TranslateResourceState( D3D12_RESOURCE_STATES d3d12ResourceState )
{
    ResourceState resourceState = ResourceState::Common;

    if ( ( d3d12ResourceState & D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER ) != 0 )
    {
        resourceState |= ResourceState::ConstantBuffer | ResourceState::VertexBuffer;
    }
    if ( ( d3d12ResourceState & D3D12_RESOURCE_STATE_INDEX_BUFFER ) != 0 )
    {
        resourceState |= ResourceState::IndexBuffer;
    }
    if ( ( d3d12ResourceState & D3D12_RESOURCE_STATE_RENDER_TARGET ) != 0 )
    {
        resourceState |= ResourceState::RenderTarget;
    }
    if ( ( d3d12ResourceState & D3D12_RESOURCE_STATE_UNORDERED_ACCESS ) != 0 )
    {
        resourceState |= ResourceState::UAV;
    }
    if ( ( d3d12ResourceState & D3D12_RESOURCE_STATE_DEPTH_WRITE ) != 0 )
    {
        resourceState |= ResourceState::DepthWrite;
    }
    if ( ( d3d12ResourceState & D3D12_RESOURCE_STATE_DEPTH_READ ) != 0 )
    {
        resourceState |= ResourceState::DepthRead;
    }
    if ( ( d3d12ResourceState & D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE ) != 0 )
    {
        resourceState |= ResourceState::NonPixelShader;
    }
    if ( ( d3d12ResourceState & D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE ) != 0 )
    {
        resourceState |= ResourceState::PixelShader;
    }
    if ( ( d3d12ResourceState & D3D12_RESOURCE_STATE_STREAM_OUT ) != 0 )
    {
        resourceState |= ResourceState::StreamOut;
    }
    if ( ( d3d12ResourceState & D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT ) != 0 )
    {
        resourceState |= ResourceState::IndirectArgument;
    }
    if ( ( d3d12ResourceState & D3D12_RESOURCE_STATE_COPY_DEST ) != 0 )
    {
        resourceState |= ResourceState::CopyDest;
    }
    if ( ( d3d12ResourceState & D3D12_RESOURCE_STATE_COPY_SOURCE ) != 0 )
    {
        resourceState |= ResourceState::CopySrc;
    }
    if ( ( d3d12ResourceState & D3D12_RESOURCE_STATE_RESOLVE_DEST ) != 0 )
    {
        resourceState |= ResourceState::ResolveDest;
    }
    if ( ( d3d12ResourceState & D3D12_RESOURCE_STATE_RESOLVE_SOURCE ) != 0 )
    {
        resourceState |= ResourceState::ResolveSrc;
    }
    if ( ( d3d12ResourceState & D3D12_RESOURCE_STATE_PREDICATION ) != 0 )
    {
        resourceState |= ResourceState::Predication;
    }

    return resourceState;
}

ResourceDX12::ResourceDX12( std::shared_ptr<DeviceDX12> device )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
    , m_d3d12ResourceState( D3D12_RESOURCE_STATE_COMMON )
{}

ResourceDX12::ResourceDX12( std::shared_ptr<DeviceDX12> device, Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource, D3D12_RESOURCE_STATES state, uint64_t offset )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
{
    SetD3D12Resource( d3d12Resource, state, offset );
}

ResourceDX12::~ResourceDX12()
{
}

void ResourceDX12::SetName( const std::wstring& name )
{
    m_ResourceName = name;
    if ( m_d3d12Resource )
    {
        m_d3d12Resource->SetName( m_ResourceName.c_str() );
    }
}

ResourceState ResourceDX12::GetResourceState() const
{
    return TranslateResourceState( m_d3d12ResourceState );
}

Microsoft::WRL::ComPtr<ID3D12Resource> ResourceDX12::GetD3D12Resource() const
{
    return m_d3d12Resource;
}

void ResourceDX12::SetD3D12Resource( Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource, D3D12_RESOURCE_STATES state, UINT64 offset )
{
    m_d3d12Resource = d3d12Resource;
    if ( m_d3d12Resource )
    {
        m_d3d12ResourceDesc = m_d3d12Resource->GetDesc();
        if ( m_d3d12ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER )
        {
            m_d3d12GPUVirtualAddress = m_d3d12Resource->GetGPUVirtualAddress() + offset;
        }
    }
    m_d3d12ResourceState = state;
}

D3D12_RESOURCE_STATES ResourceDX12::GetD3D12ResourceState() const 
{
    return m_d3d12ResourceState;
}

D3D12_GPU_VIRTUAL_ADDRESS ResourceDX12::GetD3D12GPUVirtualAddress() const
{
    assert( m_d3d12ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER );
    return m_d3d12GPUVirtualAddress;
}

void ResourceDX12::CreateViews( size_t numElements, size_t elementSize )
{}

D3D12_CPU_DESCRIPTOR_HANDLE ResourceDX12::GetConstantBufferView( std::shared_ptr<GraphicsCommandBufferDX12>, uint32_t )
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE( D3D12_DEFAULT );
}

D3D12_CPU_DESCRIPTOR_HANDLE ResourceDX12::GetShaderResourceView( std::shared_ptr<GraphicsCommandBufferDX12>, uint32_t )
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE( D3D12_DEFAULT );
}

D3D12_CPU_DESCRIPTOR_HANDLE ResourceDX12::GetUnorderedAccessView( std::shared_ptr<GraphicsCommandBufferDX12> , uint32_t )
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE( D3D12_DEFAULT );
}
