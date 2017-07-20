#pragma once

#include "BasePass.h"

// A pass that renders the opaque geometry in the scene.
class OpaquePass : public BasePass
{
public:
    typedef BasePass base;

    OpaquePass( std::shared_ptr<Graphics::Scene> scene, std::shared_ptr<Graphics::GraphicsPipelineState> pipeline, bool bUseMaterials = true, uint32_t instanceCount = 1, uint32_t firstInstance = 0 );
    virtual ~OpaquePass();

    virtual void Visit( Graphics::Mesh& mesh );
protected:

private:
};