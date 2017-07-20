#pragma once

namespace Core
{
    class RenderEventArgs;
}

class RenderPass;

// The rendering technique determines the method used to render a scene.
// Typical techniques include Forward, Deferred shading, or ForwardPlus.
// A rendering technique consists of one or more render passes, for example, 
// a pass for rendering shadow maps, a pass for rendering the opaque geometry of
// the scene, a pass for rendering the transparent geometry, and one or more 
// passes for rendering individual post-process effects.
class RenderTechnique
{
public:
    RenderTechnique();
    virtual ~RenderTechnique();

    // Add a pass to the technique. A reference to the pass itself 
    // is returned so the AddPass function can be chained.
    virtual RenderTechnique& AddPass( std::shared_ptr<RenderPass> pass );

    // Render the scene using the passes that have been configured.
    virtual void Render( Core::RenderEventArgs& renderEventArgs );

protected:

private:
    typedef std::vector< std::shared_ptr<RenderPass> > RenderPassList;
    RenderPassList m_Passes;

};