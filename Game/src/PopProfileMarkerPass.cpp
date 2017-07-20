#include <GamePCH.h>

#include <PopProfileMarkerPass.h>
#include <Graphics/Profiler.h>

#include <Events.h>

using namespace Graphics;

PopProfileMarkerPass::PopProfileMarkerPass()
{}

PopProfileMarkerPass::~PopProfileMarkerPass()
{}

void PopProfileMarkerPass::Render( Core::RenderEventArgs& e )
{
    if ( e.GraphicsCommandBuffer )
    {
        Profiler::Get().PopProfilingMarker( e.GraphicsCommandBuffer );
    }
}
