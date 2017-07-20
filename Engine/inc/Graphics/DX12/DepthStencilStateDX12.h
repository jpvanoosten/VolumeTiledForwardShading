#pragma once

#include "../DepthStencilState.h"

namespace Graphics
{
    class GraphicsCommandBufferDX12;

    class DepthStencilStateDX12 : public DepthStencilState
    {
    public:
        DepthStencilStateDX12();

        virtual ~DepthStencilStateDX12();

        virtual void SetDepthMode( const DepthMode& depthMode ) override;
        virtual const DepthMode& GetDepthMode() const override;

        virtual void SetStencilMode( const StencilMode& stencilMode ) override;
        virtual const StencilMode& GetStencilMode() const override;

        // If the depth/stencil state has been modified since it was last
        // retrieved, its state will be "dirty".
        bool IsDirty() const;

        D3D12_DEPTH_STENCIL_DESC GetD3D12DepthStencilDesc(); 

        void Bind( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer );

    protected:

    private:
        D3D12_DEPTH_STENCIL_DESC m_d3d12DepthStencilDesc;

        DepthMode m_DepthMode;
        StencilMode m_StencilMode;

        bool m_IsDirty;
    };
}