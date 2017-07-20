#include <EnginePCH.h>

#include <Graphics/DX12/IndexBufferDX12.h>
#include <Graphics/DX12/BufferDX12.h>

#include <LogManager.h>

using namespace Graphics;
using namespace Microsoft::WRL;

IndexBufferDX12::IndexBufferDX12( std::shared_ptr<DeviceDX12> device )
    : BufferDX12( device )
    , m_NumIndices( 0 )
    , m_BytesPerIndex( 0 )
    , m_d3d12IndexBufferView( {} )
{
}

IndexBufferDX12::~IndexBufferDX12()
{}

size_t IndexBufferDX12::GetNumIndices() const
{
    return m_NumIndices;
}

size_t IndexBufferDX12::GetIndexSizeInBytes() const
{
    return m_BytesPerIndex;
}

void IndexBufferDX12::CreateViews( size_t numElements, size_t elementSize )
{
    m_NumIndices = numElements;
    m_BytesPerIndex = elementSize;

    switch ( m_BytesPerIndex )
    {
    case 2:
        m_dxgiFormat = DXGI_FORMAT_R16_UINT;
        break;
    case 4:
        m_dxgiFormat = DXGI_FORMAT_R32_UINT;
        break;
    default:
        LOG_ERROR( "Invalid index buffer format." );
        break;
    }

    m_d3d12IndexBufferView.BufferLocation = m_d3d12GPUVirtualAddress;
    m_d3d12IndexBufferView.SizeInBytes = static_cast<UINT>( m_NumIndices * m_BytesPerIndex );
    m_d3d12IndexBufferView.Format = m_dxgiFormat;
}

D3D12_INDEX_BUFFER_VIEW IndexBufferDX12::GetIndexBufferView() const
{
    return m_d3d12IndexBufferView;
}
