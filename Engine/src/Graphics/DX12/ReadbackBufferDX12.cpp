#include <EnginePCH.h>

#include <Graphics/DX12/ReadbackBufferDX12.h>
#include <LogManager.h>

using namespace Graphics;
using namespace Microsoft::WRL;

ReadbackBufferDX12::ReadbackBufferDX12( std::shared_ptr<DeviceDX12> device, size_t bufferSize )
    : BufferDX12( device )
    , m_BufferSize( bufferSize )
{
    ComPtr<ID3D12Resource> d3d12Resource;

    if ( m_BufferSize > 0 )
    {
        // Create a read-back resource that will fit the buffer contents.
        if ( FAILED( m_d3d12Device->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_READBACK ),
                                                             D3D12_HEAP_FLAG_NONE,
                                                             &CD3DX12_RESOURCE_DESC::Buffer( m_BufferSize ),
                                                             D3D12_RESOURCE_STATE_COPY_DEST,
                                                             nullptr,
                                                             IID_PPV_ARGS( &d3d12Resource ) ) ) )
        {
            LOG_ERROR( "Failed to create readback buffer." );
            return;
        }
    }
    else
    {
//        LOG_WARNING( "Creating a 0 size buffer will result in a NULL resource." );
    }

    SetD3D12Resource( d3d12Resource, D3D12_RESOURCE_STATE_COPY_DEST );
}

ReadbackBufferDX12::~ReadbackBufferDX12()
{

}

void ReadbackBufferDX12::GetData( void* dstBuffer, size_t offset, size_t size ) const
{
    size_t bytesToRead = std::min( m_BufferSize - offset, size );
    if ( bytesToRead > 0 && dstBuffer != nullptr && m_d3d12Resource )
    {
        void* pData = nullptr;
        if ( FAILED( m_d3d12Resource->Map( 0, nullptr, &pData ) ) )
        {
            LOG_ERROR( "Failed to map readback buffer for reading." );
        }
        else
        {
            memcpy( dstBuffer, (char*)pData + offset, bytesToRead );
            D3D12_RANGE writeRange = { 0, 0 }; // Nothing was written.
            m_d3d12Resource->Unmap( 0, &writeRange );
        }
    }
}

