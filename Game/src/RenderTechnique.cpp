#include <GamePCH.h>

#include <RenderTechnique.h>
#include <RenderPass.h>

#include <Graphics/Profiler.h>
#include <Events.h>

RenderTechnique::RenderTechnique()
{}

RenderTechnique::~RenderTechnique()
{}

RenderTechnique& RenderTechnique::AddPass( std::shared_ptr<RenderPass> pass )
{
    // No check for duplicate passes (it may be intended to render the same pass multiple times?)
    m_Passes.push_back( pass );
    
    return *this;
}

// Render the scene using the passes that have been configured.
void RenderTechnique::Render( Core::RenderEventArgs& renderEventArgs )
{
    GPU_MARKER( __FUNCTION__, renderEventArgs.GraphicsCommandBuffer );
    for ( auto pass : m_Passes )
    {
        if ( pass->IsEnabled() )
        {
            pass->PreRender( renderEventArgs );
            pass->Render( renderEventArgs );
            pass->PostRender( renderEventArgs );
        }
    }
}


