#include <GamePCH.h>
#include <CompositePass.h>

CompositePass::CompositePass()
{}

CompositePass::~CompositePass()
{}

CompositePass& CompositePass::AddPass( std::shared_ptr<RenderPass> renderPass )
{
    m_CompositePasses.push_back( renderPass );
    return *this;
}

void CompositePass::Render( Core::RenderEventArgs& e )
{
    for ( auto renderPass : m_CompositePasses )
    {
        if ( renderPass->IsEnabled() )
        {
            renderPass->PreRender( e );
            renderPass->Render( e );
            renderPass->PostRender( e );
        }
    }
}
