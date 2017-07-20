#include <EnginePCH.h>

#include <Graphics/DX12/RenderTargetDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/TextureDX12.h>

using namespace Graphics;
using namespace Microsoft::WRL;

RenderTargetDX12::RenderTargetDX12( std::shared_ptr<DeviceDX12> device )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
{
    m_Textures.resize( static_cast<size_t>( AttachmentPoint::NumAttachmentPoints ) );
}

RenderTargetDX12::~RenderTargetDX12()
{

}

void RenderTargetDX12::AttachTexture( AttachmentPoint attachment, std::shared_ptr<Texture> texture )
{
    m_Textures[static_cast<size_t>(attachment)] = std::dynamic_pointer_cast<TextureDX12>( texture );

    // Notify listeners that an attachment point has changed.
    Core::EventArgs eventAgs( *this );
    OnAttachmentChanged( eventAgs );
}

std::shared_ptr<Texture> RenderTargetDX12::GetTexture( AttachmentPoint attachment ) const
{
    return m_Textures[static_cast<size_t>( attachment )];
}
const std::vector< std::shared_ptr<TextureDX12> >& RenderTargetDX12::GetTextures() const
{
    return m_Textures;
}

void RenderTargetDX12::Resize( uint16_t width, uint16_t height )
{
    for ( auto texture : m_Textures )
    {
        if ( texture )
        {
            texture->Resize( width, height );
        }
    }
}

DXGI_SAMPLE_DESC RenderTargetDX12::GetSampleDesc() const
{
    DXGI_SAMPLE_DESC sampleDesc = { 1, 0 };
    for ( size_t i = 0; i < static_cast<size_t>( AttachmentPoint::NumAttachmentPoints ); ++i )
    {
        auto texture = m_Textures[i];
        if ( texture )
        {
            // Return the sample description of the first attached render target.
            // All render targets should have the same multi-sample settings.
            sampleDesc = texture->GetSampleDesc();
            break;
        }
    }
    return sampleDesc;
}

std::vector<DXGI_FORMAT> RenderTargetDX12::GetRTVFormats() const
{
    std::vector<DXGI_FORMAT> dxgiFormats;
    for ( size_t i = 0; i <= static_cast<size_t>( AttachmentPoint::Color7 ); ++i )
    {
        auto texture = m_Textures[i];
        if ( texture )
        {
            DXGI_FORMAT dxgiFormat = texture->GetRTVFormat();
            dxgiFormats.push_back( dxgiFormat );
        }
    }

    return dxgiFormats;
}

DXGI_FORMAT RenderTargetDX12::GetDSVFormat() const
{
    auto depthTexture = m_Textures[static_cast<size_t>( AttachmentPoint::Depth )];
    auto depthStencilTexture = m_Textures[static_cast<size_t>( AttachmentPoint::DepthStencil )];

    if ( depthTexture )
    {
        return depthTexture->GetDSVFormat();
    }
    else if ( depthStencilTexture )
    {
        return depthStencilTexture->GetDSVFormat();
    }

    return DXGI_FORMAT_UNKNOWN;
}

void RenderTargetDX12::Bind( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer )
{
    D3D12_CPU_DESCRIPTOR_HANDLE renderTargetDescriptors[8];
    UINT numRenderTargetDescriptors = 0;

    ComPtr<ID3D12GraphicsCommandList> commandList = commandBuffer->GetD3D12CommandList();

    for ( size_t i = 0; i <= static_cast<size_t>( AttachmentPoint::Color7 ); ++i )
    {
        auto texture = m_Textures[i];
        if ( texture )
        {
            commandBuffer->TransitionResoure( std::static_pointer_cast<ResourceDX12>( texture ), ResourceState::RenderTarget );
            renderTargetDescriptors[numRenderTargetDescriptors++] = texture->GetRenderTargetView();
        }
    }

    auto depthTexture = m_Textures[static_cast<size_t>( AttachmentPoint::DepthStencil )];
    if ( !depthTexture )
    {
        depthTexture = m_Textures[static_cast<size_t>( AttachmentPoint::Depth )];
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE depthStencilDescriptor( D3D12_DEFAULT );
    if ( depthTexture )
    {
        commandBuffer->TransitionResoure( std::static_pointer_cast<ResourceDX12>( depthTexture ), ResourceState::DepthWrite );
        depthStencilDescriptor = depthTexture->GetDepthStencilView();
    }

    commandBuffer->FlushResourceBarriers();

    commandList->OMSetRenderTargets( numRenderTargetDescriptors, renderTargetDescriptors, FALSE, &depthStencilDescriptor );
}
