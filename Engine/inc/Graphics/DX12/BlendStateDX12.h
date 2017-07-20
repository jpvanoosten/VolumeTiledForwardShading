#pragma once

#include "../BlendState.h"

namespace Graphics
{
    class GraphicsCommandBufferDX12;

    class BlendStateDX12 : public BlendState
    {
    public:
        BlendStateDX12();
        virtual ~BlendStateDX12();

        virtual void SetBlendMode( const BlendMode& blendMode ) override;
        virtual void SetBlendModes( const std::vector<BlendMode>& blendModes ) override;
        virtual const std::vector<BlendMode>& GetBlendModes() const override;

        virtual void SetConstBlendFactor( const ConstantBlendFactor& constantBlendFactor ) override;
        virtual const ConstantBlendFactor& GetConstBlendFactor() const override;

        virtual void SetSampleMask( uint32_t sampleMask ) override;
        virtual uint32_t GetSampleMask() const override;

        virtual void SetAlphaCoverage( bool enabled ) override;
        virtual bool GetAlphaCoverage() const override;

        virtual void SetIndependentBlend( bool enabled ) override;
        virtual bool GetIndependentBlend() const override;

        /**
         * If any of the blend state settings have been modified, the blend state
         * will be makred "dirty".
         */
        bool IsDirty() const;

        D3D12_BLEND_DESC GetD3D12BlendDesc();

        void Bind( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer );

    protected:

    private:
        D3D12_BLEND_DESC m_d3d12BlendDesc;

        std::vector<BlendMode> m_BlendModes;

        bool m_AlphaToCoverageEnable;
        bool m_IndependentBlendEnable;
        uint32_t m_SampleMask;

        ConstantBlendFactor m_ConstantBlendFactor;

        bool m_IsDirty;

    };
}