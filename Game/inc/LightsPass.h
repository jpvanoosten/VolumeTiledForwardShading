#pragma once

#include "BasePass.h"

#include <Graphics/PointLight.h>
#include <Graphics/SpotLight.h>
#include <Graphics/DirectionalLight.h>

#include <Graphics/Device.h>
#include <Graphics/GraphicsPipelineState.h>
#include <Graphics/Material.h>
#include <Graphics/Scene.h>
#include <Graphics/StructuredBuffer.h>

class LightsPass : public BasePass
{
public:
    LightsPass( std::shared_ptr<Graphics::Device> device,
                const std::vector<Graphics::PointLight>& pointLights, const std::vector<Graphics::SpotLight>& spotLights, const std::vector<Graphics::DirectionalLight>& dirLights,
                std::shared_ptr<Graphics::Scene> pointLightScene, std::shared_ptr<Graphics::Scene> spotLightScene, std::shared_ptr<Graphics::Scene> dirLightScene,
                std::shared_ptr<Graphics::GraphicsPipelineState> pointLightPSO, std::shared_ptr<Graphics::GraphicsPipelineState> spotLightPSO, std::shared_ptr<Graphics::GraphicsPipelineState> dirLightPSO );

    virtual ~LightsPass();

    // Render the pass. This should only be called by the RenderTechnique.
    virtual void Render( Core::RenderEventArgs& e ) override;

    // Inherited from Visitor
    virtual void Visit( Graphics::SceneNode& node ) override;
    virtual void Visit( Graphics::Mesh& mesh ) override;


protected:

private:
    const std::vector<Graphics::PointLight>& m_PointLights;
    const std::vector<Graphics::SpotLight>& m_SpotLights;
    const std::vector<Graphics::DirectionalLight>& m_DirLights;

    std::shared_ptr<Graphics::GraphicsPipelineState> m_PointLightPSO;
    std::shared_ptr<Graphics::GraphicsPipelineState> m_SpotLightPSO;
    std::shared_ptr<Graphics::GraphicsPipelineState> m_DirLightPSO;

    std::shared_ptr<Graphics::Scene> m_PointLightScene;
    std::shared_ptr<Graphics::Scene> m_SpotLightScene;
    std::shared_ptr<Graphics::Scene> m_DirLightScene;

    std::shared_ptr<Graphics::Material> m_LightMaterial;
};