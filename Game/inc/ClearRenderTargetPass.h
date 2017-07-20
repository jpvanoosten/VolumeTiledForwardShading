#pragma once

#include "AbstractPass.h"
#include <Graphics/ClearColor.h>

namespace Graphics
{
    class RenderTarget;
    class Texture;
}

class ClearRenderTargetPass : public AbstractPass
{
public:
    ClearRenderTargetPass( std::shared_ptr<Graphics::RenderTarget> renderTarget,
                           Graphics::ClearFlags clearFlags = Graphics::ClearFlags::All,
                           const Graphics::ClearColor& clearColor = Graphics::ClearColor::Black,
                           float depth = 1.0f,
                           uint8_t stencil = 0 );
    ClearRenderTargetPass( std::shared_ptr<Graphics::Texture> texture,
                           Graphics::ClearFlags clearFlags = Graphics::ClearFlags::All,
                           const Graphics::ClearColor& clearColor = Graphics::ClearColor::Black,
                           float depth = 1.0f,
                           uint8_t stencil = 0 );
    virtual ~ClearRenderTargetPass();

    virtual void Render( Core::RenderEventArgs& e );

private:
    std::shared_ptr<Graphics::RenderTarget> m_RenderTarget;
    std::shared_ptr<Graphics::Texture> m_Texture;
    Graphics::ClearFlags m_ClearFlags;
    Graphics::ClearColor m_ClearColor;
    float m_ClearDepth;
    uint8_t m_ClearStencil;
};