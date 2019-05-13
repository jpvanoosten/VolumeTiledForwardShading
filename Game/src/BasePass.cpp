#include <GamePCH.h>

#include <Graphics/Camera.h>
#include <Graphics/Scene.h>
#include <Graphics/SceneNode.h>
#include <Graphics/Mesh.h>
#include <Graphics/Material.h>
#include <Graphics/GraphicsPipelineState.h>
#include <Graphics/GraphicsCommandBuffer.h>
#include <Graphics/ShaderParameter.h>
#include <Graphics/Texture.h>

#include <ConstantBuffers.h>

#include <BasePass.h>

using namespace Graphics;

BasePass::BasePass( std::shared_ptr<Graphics::Scene> scene, std::shared_ptr<Graphics::GraphicsPipelineState> pipeline, bool bUseMaterials, uint32_t instanceCount, uint32_t firstInstance )
    : m_Scene( scene )
    , m_Pipeline( pipeline )
    , m_UseMaterials( bUseMaterials )
    , m_InstanceCount( instanceCount )
    , m_FirstInstance( firstInstance )
{
}

BasePass::~BasePass()
{
}

void BasePass::PreRender( Core::RenderEventArgs& e )
{
    m_pRenderEventArgs = &e;
    m_Camera = e.Camera;
    m_GraphicsCommandBuffer = e.GraphicsCommandBuffer;
    if ( m_GraphicsCommandBuffer && m_Pipeline )
    {
        m_GraphicsCommandBuffer->BindGraphicsPipelineState( m_Pipeline );
    }
}

void BasePass::Render( Core::RenderEventArgs& e )
{
    if ( m_Scene )
    {
        m_Scene->Accept( *this );
    }
}

void BasePass::PostRender( Core::RenderEventArgs& e )
{
    m_pRenderEventArgs = nullptr;
    m_Camera = nullptr;
    m_GraphicsCommandBuffer = nullptr;
}

// Inherited from Visitor
void BasePass::Visit( Graphics::Scene& scene )
{

}

void BasePass::Visit( Graphics::SceneNode& node )
{
    if ( m_Camera && m_GraphicsCommandBuffer )
    {
        PerObjectCB perObjectData;
        // Update the constant buffer data for the node.
        perObjectData.Model = node.GetWorldTransform();
        perObjectData.View = m_Camera->GetViewMatrix();
        perObjectData.InverseView = m_Camera->GetInverseViewMatrix();
        perObjectData.Projection = m_Camera->GetProjectionMatrix();
        perObjectData.ModelView = perObjectData.View * perObjectData.Model;
        perObjectData.ModelViewProjection = perObjectData.Projection * perObjectData.ModelView;
        perObjectData.InverseTransposeModel = glm::inverseTranspose( perObjectData.Model );
        perObjectData.InverseTransposeModelView = perObjectData.View * perObjectData.InverseTransposeModel;

        m_GraphicsCommandBuffer->BindGraphicsDynamicConstantBuffer( 0, perObjectData );
    }
}

void BasePass::Visit( Graphics::Mesh& mesh )
{
    std::shared_ptr<Graphics::Material> pMaterial = mesh.GetMaterial();

    if ( pMaterial )
    {
        BindMaterial( pMaterial );
        mesh.Render( *m_pRenderEventArgs, m_InstanceCount, m_FirstInstance );
    }
}

void BasePass::BindMaterial( std::shared_ptr<Graphics::Material> pMaterial )
{
    if ( pMaterial && m_UseMaterials )
    {
        assert( m_GraphicsCommandBuffer );

        const uint32_t numTextures = static_cast<uint32_t>( Material::TextureType::NumTypes );
        std::shared_ptr<Resource> textureArguments[numTextures];
        for ( uint32_t i = 0; i < numTextures; ++i )
        {
            textureArguments[i] = pMaterial->GetTexture( static_cast<Material::TextureType>( i ) );
        }

        // Bind the material to the rendering pipeline
        m_GraphicsCommandBuffer->BindGraphicsDynamicConstantBuffer( 1, pMaterial->GetMaterialProperties() );
        m_GraphicsCommandBuffer->BindGraphicsShaderArguments( 3, 0, ShaderArguments( textureArguments, textureArguments + numTextures ) );
    }
}
