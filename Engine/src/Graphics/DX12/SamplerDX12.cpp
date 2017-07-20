#include <EnginePCH.h>

#include <Graphics/DX12/SamplerDX12.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <LogManager.h>

using namespace Graphics;

Filter TranslateMinFilter( D3D12_FILTER d3d12Filter )
{
    Filter minFilter = Filter::Nearest;

    switch ( d3d12Filter )
    {
    case D3D12_FILTER_MIN_MAG_MIP_POINT:
    case D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR:
    case D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
    case D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
    case D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT:
    case D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
    case D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
    case D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
        minFilter = Filter::Nearest;
        break;
    case D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT:
    case D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MIN_MAG_MIP_LINEAR:
    case D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
    case D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
    case D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
    case D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
    case D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
    case D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
        minFilter = Filter::Linear;
        break;
    }

    return minFilter;
}

Filter TranslateMagFilter( D3D12_FILTER d3d12Filter )
{
    Filter magFilter = Filter::Nearest;

    switch ( d3d12Filter )
    {
    case D3D12_FILTER_MIN_MAG_MIP_POINT:
    case D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT:
    case D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
    case D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
    case D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT:
    case D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
    case D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
    case D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
    case D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
        magFilter = Filter::Nearest;
        break;
    case D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR:
    case D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MIN_MAG_MIP_LINEAR:
    case D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
    case D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
    case D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
    case D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
    case D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
    case D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
        magFilter = Filter::Linear;
        break;
    }

    return magFilter;
}

Filter TranslateMipFilter( D3D12_FILTER d3d12Filter )
{
    Filter mipFilter = Filter::Nearest;

    switch ( d3d12Filter )
    {
    case D3D12_FILTER_MIN_MAG_MIP_POINT:
    case D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT:
    case D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
    case D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
    case D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT:
    case D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
    case D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
    case D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
    case D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
    case D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
        mipFilter = Filter::Nearest;
        break;
    case D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR:
    case D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MIN_MAG_MIP_LINEAR:
    case D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
    case D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
    case D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
    case D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
    case D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
    case D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
    case D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
        mipFilter = Filter::Linear;
        break;
    }

    return mipFilter;
}

bool IsAnisotropic( D3D12_FILTER d3d12Filter )
{
    bool isAnisotropic = false;

    switch ( d3d12Filter )
    {
    case D3D12_FILTER_ANISOTROPIC:
    case D3D12_FILTER_COMPARISON_ANISOTROPIC:
    case D3D12_FILTER_MINIMUM_ANISOTROPIC:
    case D3D12_FILTER_MAXIMUM_ANISOTROPIC:
        isAnisotropic = true;
        break;
    }

    return isAnisotropic;
}

WrapMode TranslateAddressMode( D3D12_TEXTURE_ADDRESS_MODE d3d12AddressMode )
{
    WrapMode wrapMode = WrapMode::Repeat;

    switch ( d3d12AddressMode )
    {
    case D3D12_TEXTURE_ADDRESS_MODE_WRAP:
        wrapMode = WrapMode::Repeat;
        break;
    case D3D12_TEXTURE_ADDRESS_MODE_MIRROR:
        wrapMode = WrapMode::Mirror;
        break;
    case D3D12_TEXTURE_ADDRESS_MODE_CLAMP:
        wrapMode = WrapMode::Clamp;
        break;
    case D3D12_TEXTURE_ADDRESS_MODE_BORDER:
        wrapMode = WrapMode::Border;
        break;
    case D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE:
        wrapMode = WrapMode::MirrorOnce;
        break;
    default:
        break;
    }

    return wrapMode;
}

CompareMode TranslateCompareMode( D3D12_FILTER d3d12Filter )
{
    CompareMode compareMode = CompareMode::None;

    if ( d3d12Filter < D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT )
    {
        compareMode = CompareMode::None;
    }
    else if ( d3d12Filter < D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT )
    {
        compareMode = CompareMode::CompareRefToTexture;
    }
    else if ( d3d12Filter < D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT )
    {
        compareMode = CompareMode::Minimum;
    }
    else
    {
        compareMode = CompareMode::Maximum;
    }

    return compareMode;
}

CompareFunction TranslateCompareFunc( D3D12_COMPARISON_FUNC d3d12ComparFunc )
{
    CompareFunction compareFunc = CompareFunction::Always;

    switch ( d3d12ComparFunc )
    {
    case D3D12_COMPARISON_FUNC_NEVER:
        compareFunc = CompareFunction::Never;
        break;
    case D3D12_COMPARISON_FUNC_LESS:
        compareFunc = CompareFunction::Less;
        break;
    case D3D12_COMPARISON_FUNC_EQUAL:
        compareFunc = CompareFunction::Equal;
        break;
    case D3D12_COMPARISON_FUNC_LESS_EQUAL:
        compareFunc = CompareFunction::LessOrEqual;
        break;
    case D3D12_COMPARISON_FUNC_GREATER:
        compareFunc = CompareFunction::Greater;
        break;
    case D3D12_COMPARISON_FUNC_NOT_EQUAL:
        compareFunc = CompareFunction::NotEqual;
        break;
    case D3D12_COMPARISON_FUNC_GREATER_EQUAL:
        compareFunc = CompareFunction::GreaterOrEqual;
        break;
    case D3D12_COMPARISON_FUNC_ALWAYS:
        compareFunc = CompareFunction::Always;
        break;
    }

    return compareFunc;
}

glm::vec4 TranslateStaticBorderColor( D3D12_STATIC_BORDER_COLOR d3d12StaticBorderColor )
{
    glm::vec4 borderColor(1);

    switch ( d3d12StaticBorderColor )
    {
    case D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK:
        borderColor = glm::vec4( 0 );
        break;
    case D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK:
        borderColor = glm::vec4( 0, 0, 0, 1 );
        break;
    case D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE:
        borderColor = glm::vec4( 1 );
        break;
    }

    return borderColor;
}

SamplerDX12::SamplerDX12( std::shared_ptr<DeviceDX12> device )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
    , m_d3d12CpuDescriptor( CD3DX12_CPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT) )
    , m_d3d12StaticSamplerDesc( {} )
    , m_MinFilter( Filter::Nearest )
    , m_MagFilter( Filter::Nearest )
    , m_MipFilter( Filter::Nearest )
    , m_WrapModeU( WrapMode::Repeat )
    , m_WrapModeV( WrapMode::Repeat )
    , m_WrapModeW( WrapMode::Repeat )
    , m_CompareMode( CompareMode::None )
    , m_CompareFunc( CompareFunction::Always )
    , m_fLODBias( 0.0f )
    , m_fMinLOD( 0.0f )
    , m_fMaxLOD( D3D12_FLOAT32_MAX )
    , m_BorderColor( 1 )
    , m_bIsAnisotropicFilteringEnabled( false )
    , m_AnisotropicFiltering( 1 )
    , m_bIsStaticSamplerDirty( true )
    , m_bIsDescriptorDirty( true )
{

}

SamplerDX12::SamplerDX12( std::shared_ptr<DeviceDX12> device, const D3D12_STATIC_SAMPLER_DESC& d3d12StaticSamplerDesc )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
    , m_d3d12CpuDescriptor( CD3DX12_CPU_DESCRIPTOR_HANDLE( D3D12_DEFAULT ) )
    , m_d3d12StaticSamplerDesc( d3d12StaticSamplerDesc )
    , m_MinFilter( TranslateMinFilter(d3d12StaticSamplerDesc.Filter) )
    , m_MagFilter( TranslateMagFilter(d3d12StaticSamplerDesc.Filter) )
    , m_MipFilter( TranslateMipFilter(d3d12StaticSamplerDesc.Filter) )
    , m_WrapModeU( TranslateAddressMode(d3d12StaticSamplerDesc.AddressU) )
    , m_WrapModeV( TranslateAddressMode(d3d12StaticSamplerDesc.AddressV) )
    , m_WrapModeW( TranslateAddressMode(d3d12StaticSamplerDesc.AddressW) )
    , m_CompareMode( TranslateCompareMode( d3d12StaticSamplerDesc.Filter ) )
    , m_CompareFunc( TranslateCompareFunc(d3d12StaticSamplerDesc.ComparisonFunc) )
    , m_fLODBias( d3d12StaticSamplerDesc.MipLODBias )
    , m_fMinLOD( d3d12StaticSamplerDesc.MinLOD )
    , m_fMaxLOD( d3d12StaticSamplerDesc.MaxLOD )
    , m_BorderColor( TranslateStaticBorderColor(d3d12StaticSamplerDesc.BorderColor) )
    , m_bIsAnisotropicFilteringEnabled( IsAnisotropic( d3d12StaticSamplerDesc.Filter ) )
    , m_AnisotropicFiltering( d3d12StaticSamplerDesc.MaxAnisotropy )
    , m_bIsStaticSamplerDirty( false )
    , m_bIsDescriptorDirty( true )
{

}


SamplerDX12::~SamplerDX12()
{}

void SamplerDX12::SetDirty()
{
    m_bIsStaticSamplerDirty = true;
    m_bIsDescriptorDirty = true;
}

void SamplerDX12::SetFilter( Filter minFilter, Filter magFilter, Filter mipFilter )
{
    m_MinFilter = minFilter;
    m_MagFilter = magFilter;
    m_MipFilter = mipFilter;
    SetDirty();
}

void SamplerDX12::GetFilter( Filter& minFilter, Filter& magFilter, Filter& mipFilter ) const
{
    minFilter = m_MinFilter;
    magFilter = m_MagFilter;
    mipFilter = m_MipFilter;
}

void SamplerDX12::SetWrapMode( WrapMode u, WrapMode v, WrapMode w )
{
    m_WrapModeU = u;
    m_WrapModeV = v;
    m_WrapModeW = w;
    SetDirty();
}

void SamplerDX12::GetWrapMode( WrapMode& u, WrapMode& v, WrapMode& w ) const
{
    u = m_WrapModeU;
    v = m_WrapModeV;
    w = m_WrapModeW;
}

void SamplerDX12::SetCompareFunction( CompareFunction compareFunc )
{
    m_CompareFunc = compareFunc;
    SetDirty();
}

CompareFunction SamplerDX12::GetCompareFunc() const
{
    return m_CompareFunc;
}

void SamplerDX12::SetCompareMode( CompareMode compareMode )
{
    m_CompareMode = compareMode;
    SetDirty();
}

CompareMode SamplerDX12::GetCompareMode() const
{
    return m_CompareMode;
}

void SamplerDX12::SetLODBias( float lodBias )
{
    m_fLODBias = lodBias;
    SetDirty();
}

float SamplerDX12::GetLODBias() const
{
    return m_fLODBias;
}

void SamplerDX12::SetMinLOD( float minLOD )
{
    m_fMinLOD = minLOD;
    SetDirty();
}

float SamplerDX12::GetMinLOD() const
{
    return m_fMinLOD;
}

void SamplerDX12::SetMaxLOD( float maxLOD )
{
    m_fMaxLOD = maxLOD;
    SetDirty();
}

float SamplerDX12::GetMaxLOD() const
{
    return m_fMaxLOD;
}

void SamplerDX12::SetBorderColor( const glm::vec4& borderColor )
{
    m_BorderColor = borderColor;
    SetDirty();
}

glm::vec4 SamplerDX12::GetBorderColor() const
{
    return m_BorderColor;
}

void SamplerDX12::EnableAnisotropicFiltering( bool enabled )
{
    m_bIsAnisotropicFilteringEnabled = enabled;
    SetDirty();
}

bool SamplerDX12::IsAnisotropicFilteringEnabled() const
{
    return m_bIsAnisotropicFilteringEnabled;
}


void SamplerDX12::SetMaxAnisotropy( uint8_t maxAnisotropy )
{
    m_AnisotropicFiltering = glm::clamp<uint8_t>( maxAnisotropy, 1, 16 );
    SetDirty();
}

uint8_t SamplerDX12::GetMaxAnisotropy() const
{
    return m_AnisotropicFiltering;
}

inline D3D12_FILTER operator|( D3D12_FILTER lhs, D3D12_FILTER rhs )
{
    return static_cast<D3D12_FILTER>( static_cast<uint32_t>( lhs ) | static_cast<uint32_t>( rhs ) );
}

inline D3D12_FILTER operator|=( D3D12_FILTER& lhs, D3D12_FILTER rhs )
{
    return lhs = lhs | rhs;
}

D3D12_FILTER SamplerDX12::TranslateFilter() const
{
    D3D12_FILTER filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    if ( m_bIsAnisotropicFilteringEnabled )
    {
        filter = D3D12_FILTER_ANISOTROPIC;
    }
    else
    {
        if ( m_MinFilter == Filter::Nearest && m_MagFilter == Filter::Nearest && m_MipFilter == Filter::Nearest )
        {
            filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        }
        else if ( m_MinFilter == Filter::Nearest && m_MagFilter == Filter::Nearest && m_MipFilter == Filter::Linear )
        {
            filter = D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        }
        else if ( m_MinFilter == Filter::Nearest && m_MagFilter == Filter::Linear&& m_MipFilter == Filter::Nearest )
        {
            filter = D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        }
        else if ( m_MinFilter == Filter::Nearest && m_MagFilter == Filter::Linear && m_MipFilter == Filter::Linear )
        {
            filter = D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
        }
        else if ( m_MinFilter == Filter::Linear && m_MagFilter == Filter::Nearest && m_MipFilter == Filter::Nearest )
        {
            filter = D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        }
        else if ( m_MinFilter == Filter::Linear && m_MagFilter == Filter::Nearest && m_MipFilter == Filter::Linear )
        {
            filter = D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        }
        else if ( m_MinFilter == Filter::Linear && m_MagFilter == Filter::Linear && m_MipFilter == Filter::Nearest )
        {
            filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        }
        else if ( m_MinFilter == Filter::Linear && m_MagFilter == Filter::Linear && m_MipFilter == Filter::Linear )
        {
            filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        }
        else
        {
            LOG_ERROR( "Invalid texture filter modes." );
        }
    }

    switch ( m_CompareMode )
    {
    case CompareMode::CompareRefToTexture:
        filter |= D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
        break;
    case CompareMode::Minimum:
        filter |= D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
        break;
    case CompareMode::Maximum:
        filter |= D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
        break;
    }

    return filter;
}

D3D12_TEXTURE_ADDRESS_MODE SamplerDX12::TranslateWrapMode( WrapMode wrapMode ) const
{
    D3D12_TEXTURE_ADDRESS_MODE addressMode = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

    switch ( wrapMode )
    {
    case WrapMode::Repeat:
        addressMode = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        break;
    case WrapMode::Clamp:
        addressMode = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        break;
    case WrapMode::Mirror:
        addressMode = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        break;
    case WrapMode::Border:
        addressMode = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        break;
    case WrapMode::MirrorOnce:
        addressMode = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
        break;
    }

    return addressMode;
}

D3D12_COMPARISON_FUNC SamplerDX12::TranslateComparisonFunction( CompareFunction compareFunc ) const
{
    D3D12_COMPARISON_FUNC d3d12ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    switch ( compareFunc )
    {
    case CompareFunction::Never:
        d3d12ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        break;
    case CompareFunction::Less:
        d3d12ComparisonFunc = D3D12_COMPARISON_FUNC_LESS;
        break;
    case CompareFunction::Equal:
        d3d12ComparisonFunc = D3D12_COMPARISON_FUNC_EQUAL;
        break;
    case CompareFunction::LessOrEqual:
        d3d12ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        break;
    case CompareFunction::Greater:
        d3d12ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
        break;
    case CompareFunction::NotEqual:
        d3d12ComparisonFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL;
        break;
    case CompareFunction::GreaterOrEqual:
        d3d12ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        break;
    case CompareFunction::Always:
        d3d12ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        break;
    }

    return d3d12ComparisonFunc;
}

D3D12_CPU_DESCRIPTOR_HANDLE SamplerDX12::GetSamplerDescriptor()
{
    if ( m_d3d12CpuDescriptor.ptr == 0 )
    {
        std::shared_ptr<DeviceDX12> device = m_Device.lock();
        if ( device )
        {
            m_d3d12CpuDescriptor = device->AllocateDescriptors( D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER );
        }
        else
        {
            LOG_WARNING( "Invalid device while trying to allocate a descriptor." );
            return m_d3d12CpuDescriptor;
        }
    }

    if ( m_bIsDescriptorDirty )
    {
        D3D12_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter = TranslateFilter();
        samplerDesc.AddressU = TranslateWrapMode( m_WrapModeU );
        samplerDesc.AddressV = TranslateWrapMode( m_WrapModeV );
        samplerDesc.AddressW = TranslateWrapMode( m_WrapModeW );
        samplerDesc.MipLODBias = m_fLODBias;
        samplerDesc.MaxAnisotropy = m_AnisotropicFiltering;
        samplerDesc.ComparisonFunc = TranslateComparisonFunction( m_CompareFunc );
        samplerDesc.BorderColor[0] = m_BorderColor.r;
        samplerDesc.BorderColor[1] = m_BorderColor.g;
        samplerDesc.BorderColor[2] = m_BorderColor.b;
        samplerDesc.BorderColor[3] = m_BorderColor.a;
        samplerDesc.MinLOD = m_fMinLOD;
        samplerDesc.MaxLOD = m_fMaxLOD;

        m_d3d12Device->CreateSampler( &samplerDesc, m_d3d12CpuDescriptor );

        m_bIsDescriptorDirty = false;
    }

    return m_d3d12CpuDescriptor;
}

D3D12_STATIC_SAMPLER_DESC SamplerDX12::GetStaticSamplerDesc()
{
    if ( m_bIsStaticSamplerDirty )
    {
        m_d3d12StaticSamplerDesc.Filter = TranslateFilter();
        m_d3d12StaticSamplerDesc.AddressU = TranslateWrapMode( m_WrapModeU );
        m_d3d12StaticSamplerDesc.AddressV = TranslateWrapMode( m_WrapModeV );
        m_d3d12StaticSamplerDesc.AddressW = TranslateWrapMode( m_WrapModeW );
        m_d3d12StaticSamplerDesc.MipLODBias = m_fLODBias;
        m_d3d12StaticSamplerDesc.MaxAnisotropy = m_AnisotropicFiltering;
        m_d3d12StaticSamplerDesc.ComparisonFunc = TranslateComparisonFunction( m_CompareFunc );
        m_d3d12StaticSamplerDesc.MinLOD = m_fMinLOD;
        m_d3d12StaticSamplerDesc.MaxLOD = m_fMaxLOD;
        m_d3d12StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;

        if ( m_BorderColor == glm::vec4( 0, 0, 0, 1 ) )
        {
            m_d3d12StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
        }
        else if ( m_BorderColor == glm::vec4( 0 ) )
        {
            m_d3d12StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        }

        m_bIsStaticSamplerDirty = false;
    }

    return m_d3d12StaticSamplerDesc;
}
