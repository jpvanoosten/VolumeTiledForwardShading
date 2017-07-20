#pragma once

#include "AbstractPass.h"
#include "ConstantBuffers.h"

namespace Graphics
{
    class Camera;
    class Material;
    class GraphicsCommandBuffer;
    class GraphicsPipelineState;
}

// Base pass provides implementations for functions used by most passes.
class BasePass : public AbstractPass
{
public:
    typedef AbstractPass base;

    BasePass( std::shared_ptr<Graphics::Scene> scene, std::shared_ptr<Graphics::GraphicsPipelineState> pipeline, bool bUseMaterials = true, uint32_t instanceCount = 1, uint32_t firstInstance = 0 );
    virtual ~BasePass();

    // Render the pass. This should only be called by the RenderTechnique.
    virtual void PreRender( Core::RenderEventArgs& e ) override;
    virtual void Render( Core::RenderEventArgs& e ) override;
    virtual void PostRender( Core::RenderEventArgs& e ) override;

    // Inherited from Visitor
    virtual void Visit( Graphics::Scene& scene ) override;
    virtual void Visit( Graphics::SceneNode& node ) override;
    virtual void Visit( Graphics::Mesh& mesh ) override;

    void BindMaterial( std::shared_ptr<Graphics::Material> pMaterial );

protected:

    Core::RenderEventArgs* m_pRenderEventArgs;

    // Pointer to the current camera.
    // Only valid during rendering.
    std::shared_ptr<Graphics::Camera> m_Camera;
    // Pointer to the graphics command buffer. 
    // Only valid during rendering.
    std::shared_ptr<Graphics::GraphicsCommandBuffer> m_GraphicsCommandBuffer;

    // The scene to render.
    std::shared_ptr< Graphics::Scene > m_Scene;
    // The pipeline state that should be used to render this pass.
    std::shared_ptr< Graphics::GraphicsPipelineState > m_Pipeline;

    bool m_UseMaterials;
    uint32_t m_InstanceCount;
    uint32_t m_FirstInstance;
};