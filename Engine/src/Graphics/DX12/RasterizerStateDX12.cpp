#include <EnginePCH.h>

#include <Graphics/DX12/RasterizerStateDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>

using namespace Graphics;
using namespace Microsoft::WRL;

D3D12_FILL_MODE TranslateFillMode( FillMode fillMode )
{
    D3D12_FILL_MODE result = D3D12_FILL_MODE_SOLID;

    switch ( fillMode )
    {
    case FillMode::Solid:
        result = D3D12_FILL_MODE_SOLID;
        break;
    case FillMode::Wireframe:
        result = D3D12_FILL_MODE_WIREFRAME;
        break;
    }

    return result;
}

D3D12_CULL_MODE TranslateCullMode( CullMode cullMode )
{
    D3D12_CULL_MODE result = D3D12_CULL_MODE_BACK;

    switch ( cullMode )
    {
    case CullMode::None:
        result = D3D12_CULL_MODE_NONE;
        break;
    case CullMode::Back:
        result = D3D12_CULL_MODE_BACK;
        break;
    case CullMode::Front:
        result = D3D12_CULL_MODE_FRONT;
        break;
    }

    return result;
}

bool TranslateFrontFace( FrontFace frontFace )
{
    bool frontCounterClockwise = true;
    switch ( frontFace )
    {
    case FrontFace::Clockwise:
        frontCounterClockwise = false;
        break;
    case FrontFace::CounterClockwise:
        frontCounterClockwise = true;
        break;
    }

    return frontCounterClockwise;
}

std::vector<D3D12_RECT> TranslateRects( const std::vector<Rect>& rects )
{
    std::vector<D3D12_RECT> result( rects.size() );
    size_t i = 0;
    for ( auto rect : rects )
    {
        D3D12_RECT& d3d12Rect = result[i++];

        d3d12Rect.top = std::lround( rect.Y );
        d3d12Rect.bottom = std::lround( rect.Y + rect.Height );
        d3d12Rect.left = std::lround( rect.X );
        d3d12Rect.right = std::lround( rect.X + rect.Width );
    }

    return result;
}

std::vector<D3D12_VIEWPORT> TranslateViewports( const std::vector<Viewport>& viewports )
{
    std::vector<D3D12_VIEWPORT> result( viewports.size() );
    size_t i = 0;
    for ( auto viewport : viewports )
    {
        D3D12_VIEWPORT& d3d12Viewport = result[i++];

        // I could probably do a reinterpret cast here...
        d3d12Viewport.TopLeftX = viewport.X;
        d3d12Viewport.TopLeftY = viewport.Y;
        d3d12Viewport.Width = viewport.Width;
        d3d12Viewport.Height = viewport.Height;
        d3d12Viewport.MinDepth = viewport.MinDepth;
        d3d12Viewport.MaxDepth = viewport.MaxDepth;
    }

    return result;
}

RasterizerStateDX12::RasterizerStateDX12()
    : m_FrontFaceFillMode( FillMode::Solid )
    , m_BackFaceFillMode( FillMode::Solid )
    , m_CullMode( CullMode::Back )
    , m_FrontFace( FrontFace::CounterClockwise )
    , m_DepthBias( 0.0f )
    , m_SlopBias( 0.0f )
    , m_BiasClamp( 0.0f )
    , m_DepthClipEnabled( true )
    , m_ScissorEnabled( false )
    , m_MultisampleEnabled( false )
    , m_AntialiasedLineEnabled( false )
    , m_ConservativeRasterization( false )
    , m_ForcedSampleCount( 0 )
    , m_StateDirty( true )
    , m_ViewportsDirty( true )
    , m_ScissorRectsDirty( true )
{
    m_Viewports.resize( 8 );
    m_ScissorRects.resize( 8 );


    for ( auto& rect : m_ScissorRects )
    {
        rect.Width = std::numeric_limits<uint16_t>::max();
        rect.Height = std::numeric_limits<uint16_t>::max();
    }
}

RasterizerStateDX12::~RasterizerStateDX12()
{}

void RasterizerStateDX12::SetFillMode( FillMode frontFace, FillMode backFace )
{
    m_FrontFaceFillMode = frontFace;
    m_BackFaceFillMode = backFace;

    m_StateDirty = true;
}

void RasterizerStateDX12::GetFillMode( FillMode& frontFace, FillMode& backFace ) const
{
    frontFace = m_FrontFaceFillMode;
    backFace = m_BackFaceFillMode;
}

void RasterizerStateDX12::SetCullMode( CullMode cullMode )
{
    m_CullMode = cullMode;

    m_StateDirty = true;
}

CullMode RasterizerStateDX12::GetCullMode() const
{
    return m_CullMode;
}

void RasterizerStateDX12::SetFrontFacing( FrontFace frontFace )
{
    m_FrontFace = frontFace;

    m_StateDirty = true;
}

FrontFace RasterizerStateDX12::GetFrontFacing() const
{
    return m_FrontFace;
}

void RasterizerStateDX12::SetDepthBias( float depthBias, float slopeBias, float biasClamp )
{
    m_DepthBias = depthBias; 
    m_SlopBias = slopeBias;
    m_BiasClamp = biasClamp;

    m_StateDirty = true;
}

void RasterizerStateDX12::GetDepthBias( float& depthBias, float& slopeBias, float& biasClamp ) const
{
    depthBias = m_DepthBias;
    slopeBias = m_SlopBias;
    biasClamp = m_BiasClamp;
}

void RasterizerStateDX12::SetDepthClipEnabled( bool depthClipEnabled )
{
    m_DepthClipEnabled = depthClipEnabled;

    m_StateDirty = true;
}

bool RasterizerStateDX12::GetDepthClipEnabled() const
{
    return m_DepthClipEnabled;
}

void RasterizerStateDX12::SetViewport( const Viewport& viewport )
{
    m_Viewports[0] = viewport;

    m_ViewportsDirty = true;
}

void RasterizerStateDX12::SetViewports( const std::vector<Viewport>& viewports )
{
    m_Viewports = viewports;

    m_ViewportsDirty = true;
}

const std::vector<Viewport>& RasterizerStateDX12::GetViewports()
{
    return m_Viewports;
}

void RasterizerStateDX12::SetScissorEnabled( bool scissorEnable )
{
    m_ScissorEnabled = scissorEnable;

    m_StateDirty = true;
}
bool RasterizerStateDX12::GetScissorEnabled() const
{
    return m_ScissorEnabled;
}

void RasterizerStateDX12::SetScissorRect( const Rect& rect )
{
    m_ScissorRects[0] = rect;

    m_ScissorRectsDirty = true;
}

void RasterizerStateDX12::SetScissorRects( const std::vector<Rect>& rects )
{
    m_ScissorRects = rects;

    m_ScissorRectsDirty = true;
}

const std::vector<Rect>& RasterizerStateDX12::GetScissorRects() const
{
    return m_ScissorRects;
}

void RasterizerStateDX12::SetMultisampleEnabled( bool multisampleEnabled )
{
    m_MultisampleEnabled = multisampleEnabled;
    
    m_StateDirty = true;
}

bool RasterizerStateDX12::GetMultisampleEnabled() const
{
    return m_MultisampleEnabled;
}

void RasterizerStateDX12::SetAntialiasedLineEnable( bool antialiasedLineEnable )
{
    m_AntialiasedLineEnabled = antialiasedLineEnable;

    m_StateDirty = true;
}

bool RasterizerStateDX12::GetAntialiasedLineEnable() const
{
    return m_AntialiasedLineEnabled;
}

void RasterizerStateDX12::SetForcedSampleCount( uint8_t forcedSampleCount )
{
    m_ForcedSampleCount = forcedSampleCount;

    m_StateDirty = true;
}

uint8_t RasterizerStateDX12::GetForcedSampleCount()
{
    return m_ForcedSampleCount;
}

void RasterizerStateDX12::SetConservativeRasterizationEnabled( bool conservativeRasterizationEnabled )
{
    m_ConservativeRasterization = conservativeRasterizationEnabled;

    m_StateDirty = true;
}

bool RasterizerStateDX12::GetConservativeRasterizationEnabled() const
{
    return m_ConservativeRasterization;
}

bool RasterizerStateDX12::IsDirty() const
{
    return m_StateDirty;
}

D3D12_RASTERIZER_DESC RasterizerStateDX12::GetD3D12RasterizerDescription()
{
    if ( m_StateDirty )
    {
        m_d3d12RasterizerDesc.FillMode = TranslateFillMode( m_FrontFaceFillMode );
        m_d3d12RasterizerDesc.CullMode = TranslateCullMode( m_CullMode );
        m_d3d12RasterizerDesc.FrontCounterClockwise = TranslateFrontFace( m_FrontFace );
        m_d3d12RasterizerDesc.DepthBias = std::lround( m_DepthBias );
        m_d3d12RasterizerDesc.DepthBiasClamp = m_BiasClamp;
        m_d3d12RasterizerDesc.SlopeScaledDepthBias = m_SlopBias;
        m_d3d12RasterizerDesc.DepthClipEnable = m_DepthClipEnabled;
        m_d3d12RasterizerDesc.MultisampleEnable = m_MultisampleEnabled;
        m_d3d12RasterizerDesc.AntialiasedLineEnable = m_AntialiasedLineEnabled;
        m_d3d12RasterizerDesc.ForcedSampleCount = m_ForcedSampleCount;
        m_d3d12RasterizerDesc.ConservativeRaster = m_ConservativeRasterization ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        m_StateDirty = false;
    }

    return m_d3d12RasterizerDesc;
}

void RasterizerStateDX12::Bind( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer )
{
    ComPtr<ID3D12GraphicsCommandList> d3d12CommandList = commandBuffer->GetD3D12CommandList();

    if ( m_ScissorRectsDirty )
    {
        m_d3d12Rects = TranslateRects( m_ScissorRects );
        m_ScissorRectsDirty = false;
    }

    if ( m_ViewportsDirty )
    {
        m_d3d12Viewports = TranslateViewports( m_Viewports );
        m_ViewportsDirty = false;
    }

    d3d12CommandList->RSSetScissorRects( static_cast<UINT>( m_d3d12Rects.size() ), m_d3d12Rects.data() );
    d3d12CommandList->RSSetViewports( static_cast<UINT>( m_d3d12Viewports.size() ), m_d3d12Viewports.data() );
}
