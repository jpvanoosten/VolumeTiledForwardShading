#include <EnginePCH.h>

#include <Graphics/RenderTarget.h>

using namespace Graphics;

void RenderTarget::OnAttachmentChanged( Core::EventArgs& e )
{
    AttachmentChanged( e );
}