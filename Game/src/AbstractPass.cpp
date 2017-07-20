#include <GamePCH.h>

#include <AbstractPass.h>

AbstractPass::AbstractPass()
    : m_Enabled( true )
{
}

AbstractPass::~AbstractPass()
{}

// Enable or disable the pass. If a pass is disabled, the technique will skip it.
void AbstractPass::SetEnabled( bool enabled )
{
    m_Enabled = enabled;
}

bool AbstractPass::IsEnabled() const
{
    return m_Enabled;
}

// Render the pass. This should only be called by the RenderTechnique.
void AbstractPass::PreRender( Core::RenderEventArgs& e )
{}

void AbstractPass::Render( Core::RenderEventArgs& e )
{}

void AbstractPass::PostRender( Core::RenderEventArgs& e )
{}

// Inherited from Visitor
void AbstractPass::Visit( Graphics::Scene& scene )
{}

void AbstractPass::Visit( Graphics::SceneNode& node )
{}

void AbstractPass::Visit( Graphics::Mesh& mesh )
{}
