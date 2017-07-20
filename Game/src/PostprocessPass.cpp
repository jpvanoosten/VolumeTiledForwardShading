#include <GamePCH.h>

#include <PostprocessPass.h>
#include <ConstantBuffers.h>

#include <Events.h>
#include <Graphics/Mesh.h>
#include <Graphics/Texture.h>
#include <Graphics/GraphicsCommandBuffer.h>

PostprocessPass::PostprocessPass( std::shared_ptr<Graphics::Scene> scene, std::shared_ptr<Graphics::GraphicsPipelineState> pipeline, const glm::mat4& projectionMatrix, std::shared_ptr<Graphics::Texture> texture, bool bUseMaterial )
    : BasePass( scene, pipeline, bUseMaterial )
    , m_ProjectionMatrix( projectionMatrix )
    , m_Texture( texture )
{}

void PostprocessPass::Render( Core::RenderEventArgs& e )
{
    if ( e.GraphicsCommandBuffer )
    {
        PerObjectCB perObjectData;
        perObjectData.Model = glm::mat4( 1.0f );
        perObjectData.View = glm::mat4( 1.0f );
        perObjectData.InverseView = glm::mat4( 1.0f );
        perObjectData.InverseTransposeModel = glm::mat4( 1.0f );
        perObjectData.ModelView = glm::mat4( 1.0f );
        perObjectData.InverseTransposeModelView = glm::mat4( 1.0f );
        perObjectData.Projection = m_ProjectionMatrix;
        perObjectData.ModelViewProjection = m_ProjectionMatrix;

        e.GraphicsCommandBuffer->BindGraphicsDynamicConstantBuffer( 0, perObjectData );
        e.GraphicsCommandBuffer->BindGraphicsShaderArguments( 1, 0, { m_Texture } );

        BasePass::Render( e );
    }
}

void PostprocessPass::Visit( Graphics::SceneNode& node )
{
    // Nothing to do in this case.
}

void PostprocessPass::Visit( Graphics::Mesh& mesh )
{
    mesh.Render( *m_pRenderEventArgs, m_InstanceCount, m_FirstInstance );
}
