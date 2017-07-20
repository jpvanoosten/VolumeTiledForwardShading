#include <EnginePCH.h>

#include <Graphics/DX12/VertexBufferDX12.h>
#include <Graphics/DX12/BufferDX12.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>
#include <Graphics/DX12/GraphicsCommandQueueDX12.h>

#include <LogManager.h>

using namespace Graphics;
using namespace Microsoft::WRL;

VertexBufferDX12::VertexBufferDX12( std::shared_ptr<DeviceDX12> device )
    : BufferDX12( device )
    , m_d3d12VertexBufferView( {} )
    , m_NumVertices( 0 )
    , m_VertexStride( 0 )
{
}

VertexBufferDX12::~VertexBufferDX12()
{

}

size_t VertexBufferDX12::GetVertexCount() const
{
    return m_NumVertices;
}

size_t VertexBufferDX12::GetVertexStride() const
{
    return m_VertexStride;
}
void VertexBufferDX12::CreateViews( size_t numElements, size_t elementSize )
{
    m_NumVertices = numElements;
    m_VertexStride = elementSize;

    m_d3d12VertexBufferView.BufferLocation = m_d3d12GPUVirtualAddress;
    m_d3d12VertexBufferView.SizeInBytes = static_cast<UINT>( m_NumVertices * m_VertexStride );
    m_d3d12VertexBufferView.StrideInBytes = static_cast<UINT>( m_VertexStride );
}

D3D12_VERTEX_BUFFER_VIEW VertexBufferDX12::GetVertexBufferView() const
{
    return m_d3d12VertexBufferView;
}
