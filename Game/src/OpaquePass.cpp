#include <GamePCH.h>

#include <OpaquePass.h>

#include <Graphics/Material.h>
#include <Graphics/Mesh.h>

OpaquePass::OpaquePass( std::shared_ptr<Graphics::Scene> scene, std::shared_ptr<Graphics::GraphicsPipelineState> pipeline, bool bUseMaterials, uint32_t instanceCount, uint32_t firstInstance )
    : base( scene, pipeline, bUseMaterials, instanceCount, firstInstance )
{}

OpaquePass::~OpaquePass()
{}

void OpaquePass::Visit( Graphics::Mesh& mesh )
{
    std::shared_ptr<Graphics::Material> pMaterial = mesh.GetMaterial();
    if ( pMaterial && !pMaterial->IsTransparent() )
    {
        BindMaterial( pMaterial );
        mesh.Render( *m_pRenderEventArgs, m_InstanceCount, m_FirstInstance );
    }
}
