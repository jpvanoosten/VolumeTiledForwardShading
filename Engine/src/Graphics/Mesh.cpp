#include <EnginePCH.h>

#include <Graphics/Mesh.h>

#include <Events.h>
#include <SceneVisitor.h>

#include <Graphics/VertexBuffer.h>
#include <Graphics/IndexBuffer.h>
//#include <Material.h>

#include <Graphics/GraphicsCommandBuffer.h>
#include <Graphics/GraphicsPipelineState.h>

using namespace Graphics;

Mesh::Mesh( std::shared_ptr<Device> device )
    : m_Device( device )
{}

Mesh::~Mesh()
{}

void Mesh::SetVertexBuffer( uint32_t slotID, std::shared_ptr<VertexBuffer> vertexBuffer )
{
    m_VertexBuffers[slotID] = vertexBuffer;
}

void Mesh::SetIndexBuffer( std::shared_ptr<IndexBuffer> indexBuffer )
{
    m_IndexBuffer = indexBuffer;
}

void Mesh::SetMaterial( std::shared_ptr<Material> material )
{
    m_Material = material;
}

std::shared_ptr<Material> Mesh::GetMaterial() const
{
    return m_Material;
}

void Mesh::Render( Core::RenderEventArgs& renderArgs, uint32_t instanceCount, uint32_t firstInstance )
{
    std::shared_ptr<Graphics::GraphicsCommandBuffer> commandBuffer = renderArgs.GraphicsCommandBuffer;

    if ( commandBuffer )
    {
        for ( auto vertexBuffer : m_VertexBuffers )
        {
            commandBuffer->BindVertexBuffer( vertexBuffer.first, vertexBuffer.second );
        }

        if ( m_Material )
        {
            // TODO: Bind material to command buffer??
            // The application does this as the location to bind the textures is only known by the application.
        }

        if ( m_IndexBuffer )
        {
            commandBuffer->BindIndexBuffer( m_IndexBuffer );
            commandBuffer->DrawIndexed( static_cast<uint32_t>( m_IndexBuffer->GetNumIndices() ), 0, 0, instanceCount, firstInstance );
        }
        else
        {
            assert( m_VertexBuffers.size() > 0 );
            commandBuffer->Draw( static_cast<uint32_t>( m_VertexBuffers.begin()->second->GetVertexCount() ), 0, instanceCount, firstInstance );
        }
    }

}

void Mesh::Accept( Core::SceneVisitor& visitor )
{
    visitor.Visit( *this );
}

