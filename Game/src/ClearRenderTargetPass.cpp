#include <GamePCH.h>

#include <ClearRenderTargetPass.h>

#include <Graphics/GraphicsCommandBuffer.h>
#include <Graphics/RenderTarget.h>
#include <Graphics/Texture.h>

#include <Events.h>

using namespace Graphics;
using namespace Core;

ClearRenderTargetPass::ClearRenderTargetPass( std::shared_ptr<Graphics::RenderTarget> renderTarget, Graphics::ClearFlags clearFlags, const Graphics::ClearColor& clearColor, float depth, uint8_t stencil )
    : m_RenderTarget( renderTarget )
    , m_ClearFlags( clearFlags )
    , m_ClearColor( clearColor )
    , m_ClearDepth( depth )
    , m_ClearStencil( stencil )
{}

ClearRenderTargetPass::ClearRenderTargetPass( std::shared_ptr<Graphics::Texture> texture, Graphics::ClearFlags clearFlags, const Graphics::ClearColor& clearColor, float depth, uint8_t stencil )
    : m_Texture( texture )
    , m_ClearFlags( clearFlags )
    , m_ClearColor( clearColor )
    , m_ClearDepth( depth )
    , m_ClearStencil( stencil )
{}

ClearRenderTargetPass::~ClearRenderTargetPass()
{}

void ClearRenderTargetPass::Render( Core::RenderEventArgs& e )
{
    if ( m_RenderTarget && e.GraphicsCommandBuffer )
    {
        e.GraphicsCommandBuffer->ClearRenderTarget( m_RenderTarget, m_ClearFlags, m_ClearColor, m_ClearDepth, m_ClearStencil );
    }
    if ( m_Texture && e.GraphicsCommandBuffer )
    {
        if ( ( m_ClearFlags & ClearFlags::Color ) != 0 )
        {
            e.GraphicsCommandBuffer->ClearTexture( m_Texture, m_ClearColor );
        }
        if ( ( m_ClearFlags & ClearFlags::DepthStencil ) != 0 )
        {
            e.GraphicsCommandBuffer->ClearDepthStencilTexture( m_Texture, m_ClearFlags, m_ClearDepth, m_ClearStencil );
        }
    }
}
