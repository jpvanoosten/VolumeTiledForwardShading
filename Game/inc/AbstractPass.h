#pragma once

#include "RenderPass.h"

// Most passes only need to override a single method of a RenderPass.
// Passes can derive from this class to minimize the number of methods that 
// must be overriden in order to implement a render pass.
// Most of the methods of the abstract pass resolve to do-nothing functions.
class AbstractPass : public RenderPass
{
public:
    AbstractPass();
    virtual ~AbstractPass();

    // Enable or disable the pass. If a pass is disabled, the technique will skip it.
    virtual void SetEnabled( bool enabled );
    virtual bool IsEnabled() const;

    // Render the pass. This should only be called by the RenderTechnique.
    virtual void PreRender( Core::RenderEventArgs& e ) override;
    virtual void Render( Core::RenderEventArgs& e ) = 0;
    virtual void PostRender( Core::RenderEventArgs& e ) override;

    // Inherited from Visitor
    virtual void Visit( Graphics::Scene& scene ) override;
    virtual void Visit( Graphics::SceneNode& node ) override;
    virtual void Visit( Graphics::Mesh& mesh ) override;

private:
    bool m_Enabled;
};