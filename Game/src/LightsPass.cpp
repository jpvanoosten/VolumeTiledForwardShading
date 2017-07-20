#include <GamePCH.h>
#include <LightsPass.h>
#include <Graphics/Mesh.h>
#include <Graphics/GraphicsCommandBuffer.h>

LightsPass::LightsPass( std::shared_ptr<Graphics::Device> device,
                        const std::vector<Graphics::PointLight>& pointLights, const std::vector<Graphics::SpotLight>& spotLights, const std::vector<Graphics::DirectionalLight>& dirLights,
                        std::shared_ptr<Graphics::Scene> pointLightScene, std::shared_ptr<Graphics::Scene> spotLightScene, std::shared_ptr<Graphics::Scene> dirLightScene,
                        std::shared_ptr<Graphics::GraphicsPipelineState> pointLightPSO, std::shared_ptr<Graphics::GraphicsPipelineState> spotLightPSO, std::shared_ptr<Graphics::GraphicsPipelineState> dirLightPSO )
    : BasePass( nullptr, nullptr, false )
    , m_PointLights( pointLights )
    , m_SpotLights( spotLights )
    , m_DirLights( dirLights )
    , m_PointLightPSO( pointLightPSO )
    , m_SpotLightPSO( spotLightPSO )
    , m_DirLightPSO( dirLightPSO )
    , m_PointLightScene( pointLightScene )
    , m_SpotLightScene( spotLightScene )
    , m_DirLightScene( dirLightScene )
{
    m_LightMaterial = device->CreateMaterial();
    m_LightMaterial->SetOpacity( 0.5f );
}

LightsPass::~LightsPass()
{}

void LightsPass::Render( Core::RenderEventArgs& e )
{
    m_GraphicsCommandBuffer->BindGraphicsPipelineState( m_PointLightPSO );
    m_InstanceCount = static_cast<uint32_t>( m_PointLights.size() );
    m_PointLightScene->Accept( *this );

    m_GraphicsCommandBuffer->BindGraphicsPipelineState( m_SpotLightPSO );
    m_InstanceCount = static_cast<uint32_t>( m_SpotLights.size() );
    m_SpotLightScene->Accept( *this );

    //m_GraphicsCommandBuffer->BindGraphicsPipelineState( m_DirLightPSO );
    //m_InstanceCount = m_DirLights.size();
    //m_DirLightScene->Accept( *this );
}

void LightsPass::Visit( Graphics::SceneNode& node )
{
    BasePass::Visit( node );
}

void LightsPass::Visit( Graphics::Mesh& mesh )
{
//    BindMaterial( m_LightMaterial );
    m_GraphicsCommandBuffer->BindGraphicsDynamicConstantBuffer( 1, m_LightMaterial->GetMaterialProperties() );
    mesh.Render( *m_pRenderEventArgs, m_InstanceCount );
}
