#include <EnginePCH.h>

#include <Graphics/DX12/DynamicBufferDX12.h>

#include <LogManager.h>

using namespace Graphics;

DynamicBufferDX12::DynamicBufferDX12( std::shared_ptr<DeviceDX12> device, 
                                      Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource, 
                                      D3D12_RESOURCE_STATES state )
    : BufferDX12( device )
{
    SetD3D12Resource( d3d12Resource, state, 0 );

    D3D12_RANGE readRange = { 0, 0 };
    if ( FAILED( m_d3d12Resource->Map( 0, &readRange, &m_DataPtr ) ) )
    {
        LOG_ERROR( "Failed to map resource." );
    }
}

DynamicBufferDX12::~DynamicBufferDX12()
{
    m_d3d12Resource->Unmap( 0, nullptr );
}
