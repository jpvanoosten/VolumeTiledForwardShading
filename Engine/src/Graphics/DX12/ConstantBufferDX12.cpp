#include <EnginePCH.h>

#include <Graphics/DX12/ConstantBufferDX12.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>
#include <Common.h>

using namespace Graphics;

ConstantBufferDX12::ConstantBufferDX12( std::shared_ptr<DeviceDX12> device )
    : BufferDX12( device )
    , m_SizeInBytes( 0 )
    , m_d3d12ConstantBufferView( {0} )
{
    m_d3d12ConstantBufferView = device->AllocateDescriptors( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
}

ConstantBufferDX12::~ConstantBufferDX12()
{}

size_t ConstantBufferDX12::GetSizeInBytes() const
{
    return m_SizeInBytes;
}

void ConstantBufferDX12::CreateViews(  size_t numElements, size_t elementSize )
{
    m_SizeInBytes = numElements * elementSize;

    D3D12_CONSTANT_BUFFER_VIEW_DESC d3d12ConstantBufferViewDesc;
    d3d12ConstantBufferViewDesc.BufferLocation = m_d3d12GPUVirtualAddress;
    d3d12ConstantBufferViewDesc.SizeInBytes = static_cast<UINT>( Math::AlignUp( m_SizeInBytes, 16 ) );
    
    m_d3d12Device->CreateConstantBufferView( &d3d12ConstantBufferViewDesc, m_d3d12ConstantBufferView );
}

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBufferDX12::GetConstantBufferView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t ) 
{
    commandBuffer->TransitionResoure( shared_from_this(), ResourceState::ConstantBuffer );
    return m_d3d12ConstantBufferView;
}