#pragma once

#include "../Sampler.h"

namespace Graphics
{
    class DeviceDX12;

    class SamplerDX12 : public Sampler
    {
    public:
        SamplerDX12( std::shared_ptr<DeviceDX12> device );
        SamplerDX12( std::shared_ptr<DeviceDX12> device, const D3D12_STATIC_SAMPLER_DESC& d3d12StaticSamplerDesc );

        virtual ~SamplerDX12();

        virtual void SetFilter( Filter minFilter = Filter::Nearest, Filter magFilter = Filter::Nearest, Filter mipFilter = Filter::Nearest ) override;
        virtual void GetFilter( Filter& minFilter, Filter& magFilter, Filter& mipFilter ) const override;

        virtual void SetWrapMode( WrapMode u = WrapMode::Repeat, WrapMode v = WrapMode::Repeat, WrapMode w = WrapMode::Repeat ) override;
        virtual void GetWrapMode( WrapMode& u, WrapMode& v, WrapMode& w ) const override;

        virtual void SetCompareFunction( CompareFunction compareFunc = CompareFunction::Always ) override;
        virtual CompareFunction GetCompareFunc() const override;

        virtual void SetCompareMode( CompareMode compareMode = CompareMode::None ) override;
        virtual CompareMode GetCompareMode() const override;

        virtual void SetLODBias( float lodBias ) override;
        virtual float GetLODBias() const override;

        virtual void SetMinLOD( float minLOD ) override;
        virtual float GetMinLOD() const override;

        virtual void SetMaxLOD( float maxLOD ) override;
        virtual float GetMaxLOD() const override;

        virtual void SetBorderColor( const glm::vec4& borderColor ) override;
        virtual glm::vec4 GetBorderColor() const override;

        virtual void EnableAnisotropicFiltering( bool enabled ) override;
        virtual bool IsAnisotropicFilteringEnabled() const override;

        virtual void SetMaxAnisotropy( uint8_t maxAnisotropy ) override;
        virtual uint8_t GetMaxAnisotropy() const override;

        D3D12_CPU_DESCRIPTOR_HANDLE GetSamplerDescriptor();
        D3D12_STATIC_SAMPLER_DESC GetStaticSamplerDesc();

    protected:
        D3D12_FILTER TranslateFilter() const;
        D3D12_TEXTURE_ADDRESS_MODE TranslateWrapMode( WrapMode wrapMode ) const;
        D3D12_COMPARISON_FUNC TranslateComparisonFunction( CompareFunction compareFunc ) const;

    private:

        // The sampler state has been modified. Mark the dirty flags so the sampler gets
        // recreated when it is accessed.
        void SetDirty();

        std::weak_ptr<DeviceDX12> m_Device;
        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;
        D3D12_CPU_DESCRIPTOR_HANDLE m_d3d12CpuDescriptor;
        D3D12_STATIC_SAMPLER_DESC m_d3d12StaticSamplerDesc;

        Filter m_MinFilter;
        Filter m_MagFilter;
        Filter m_MipFilter;
        WrapMode  m_WrapModeU, m_WrapModeV, m_WrapModeW;
        CompareMode m_CompareMode;
        CompareFunction m_CompareFunc;

        float       m_fLODBias;
        float       m_fMinLOD;
        float       m_fMaxLOD;

        glm::vec4   m_BorderColor;

        bool        m_bIsAnisotropicFilteringEnabled;
        uint8_t     m_AnisotropicFiltering;

        // Set to true if the sampler state needs to be recreated.
        bool        m_bIsStaticSamplerDirty;
        bool        m_bIsDescriptorDirty;
    };
}