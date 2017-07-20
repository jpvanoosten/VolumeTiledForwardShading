#include <GamePCH.h>

#include <PushProfileMarkerPass.h>
#include <Graphics/Profiler.h>

#include <Events.h>

using namespace Graphics;

PushProfileMarkerPass::PushProfileMarkerPass( const std::wstring& profileMarker )
    : m_ProfilerMarker( profileMarker )
{}

PushProfileMarkerPass::~PushProfileMarkerPass()
{}

void PushProfileMarkerPass::Render( Core::RenderEventArgs& e )
{
    if ( e.GraphicsCommandBuffer )
    {
        Profiler::Get().PushProfilingMarker( m_ProfilerMarker, e.GraphicsCommandBuffer );
    }
}
