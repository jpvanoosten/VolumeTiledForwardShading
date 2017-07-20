#include <EnginePCH.h>

#include <Application.h>
#include <Graphics/DX12/TextureDX12.h>
#include <Graphics/DX12/GraphicsCommandQueueDX12.h>
#include <Graphics/DX12/GraphicsCommandBufferDX12.h>
#include <Graphics/DXGI/TextureFormatDXGI.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/ResourceDX12.h>

#include <LogManager.h>

using namespace Core;
using namespace Graphics;
using namespace Microsoft::WRL;

TextureDX12::TextureDX12( std::shared_ptr<DeviceDX12> device )
    : ResourceDX12( device )
    , m_Width( 0 )
    , m_Height( 0 )
    , m_DepthOrArraySize( 0 )
    , m_Pitch( 0 )
    , m_BPP( 0 )
    , m_MipLevels( 0 )
    , m_IsTransparent( false )
    , m_TextureDimension( TextureDimension::Unknonwn )
    , m_SampleDesc( { 1, 0 } )
    , m_ResourceFormat( DXGI_FORMAT_UNKNOWN )
    , m_DSVFormat( DXGI_FORMAT_UNKNOWN )
    , m_SRVFormat( DXGI_FORMAT_UNKNOWN )
    , m_RTVFormat( DXGI_FORMAT_UNKNOWN )
    , m_ShaderResourceView( D3D12_DEFAULT )
    , m_RenderTargetView( D3D12_DEFAULT )
    , m_DepthStencilView( D3D12_DEFAULT )
    , m_UnorderedAccessView( D3D12_DEFAULT )
{
    m_DescriptorIncrementSize = m_d3d12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
}

TextureDX12::TextureDX12( std::shared_ptr<DeviceDX12> device, Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource, TextureDimension textureDimension )
    : ResourceDX12( device, d3d12Resource )
    , m_TextureDimension( textureDimension )
    , m_ShaderResourceView( D3D12_DEFAULT )
    , m_RenderTargetView( D3D12_DEFAULT )
    , m_DepthStencilView( D3D12_DEFAULT )
    , m_UnorderedAccessView( D3D12_DEFAULT )
{
    m_DescriptorIncrementSize = m_d3d12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );

    D3D12_RESOURCE_DESC d3d12ResourceDesc = m_d3d12Resource->GetDesc();

    m_Width = static_cast<uint32_t>( d3d12ResourceDesc.Width );
    m_Height = d3d12ResourceDesc.Height;
    m_DepthOrArraySize = d3d12ResourceDesc.DepthOrArraySize;
    m_MipLevels = static_cast<uint8_t>( d3d12ResourceDesc.MipLevels );
    
    m_TextureFormat = ConvertTextureFormat( d3d12ResourceDesc.Format, d3d12ResourceDesc.SampleDesc.Count );
    m_SampleDesc = d3d12ResourceDesc.SampleDesc;

    InitFormats( m_TextureFormat );

    InitViews( d3d12ResourceDesc );
}

TextureDX12::TextureDX12( std::shared_ptr<DeviceDX12> device, uint16_t width, uint16_t slices, const TextureFormat& format )
    : ResourceDX12( device )
    , m_ShaderResourceView( D3D12_DEFAULT )
    , m_RenderTargetView( D3D12_DEFAULT )
    , m_DepthStencilView( D3D12_DEFAULT )
    , m_UnorderedAccessView( D3D12_DEFAULT )
{
    m_DescriptorIncrementSize = m_d3d12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );

    InitFormats( format );
    
    if ( slices > 1 )
    {
        m_TextureDimension = TextureDimension::Texture1DArray;
    }
    else
    {
        m_TextureDimension = TextureDimension::Texture1D;
    }

    Resize( width, 1, slices, 1 );

}

TextureDX12::TextureDX12( std::shared_ptr<DeviceDX12> device, uint16_t width, uint16_t height, uint16_t slices, const TextureFormat& format )
    : ResourceDX12( device )
    , m_ShaderResourceView( D3D12_DEFAULT )
    , m_RenderTargetView( D3D12_DEFAULT )
    , m_DepthStencilView( D3D12_DEFAULT )
    , m_UnorderedAccessView( D3D12_DEFAULT )
{
    InitFormats( format );

    if ( slices > 1 )
    {
        m_TextureDimension = TextureDimension::Texture2DArray;
    }
    else
    {
        m_TextureDimension = TextureDimension::Texture2D;
    }

    Resize( width, height, slices, 1 );

}

inline bool CheckSRVSupport( D3D12_FORMAT_SUPPORT1 support1 )
{
    return ( support1 & D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE ) != 0 || ( support1 & D3D12_FORMAT_SUPPORT1_SHADER_LOAD ) != 0;
}

inline bool CheckRTVSupport( D3D12_FORMAT_SUPPORT1 support1 )
{
    return ( support1 & D3D12_FORMAT_SUPPORT1_RENDER_TARGET ) != 0;
}

inline bool CheckDSVSupport( D3D12_FORMAT_SUPPORT1 support1 )
{
    return ( support1 & D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL ) != 0;
}

inline bool CheckUAVSupport( D3D12_FORMAT_SUPPORT1 support1 )
{
    return ( support1 & D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW ) != 0;
}

void TextureDX12::InitFormats( const TextureFormat& textureFormat )
{
    std::shared_ptr<DeviceDX12> device = m_Device.lock();
    assert( device );

    m_TextureFormat = textureFormat;

    DXGI_FORMAT dxgiFormat = ConvertTextureFormat( m_TextureFormat );
    m_SampleDesc = device->GetMultisampleQualityLevels( dxgiFormat, m_TextureFormat.NumSamples );
    // The supported sample count could be different than the requested sample count from the application.
    // To make sure the actual sample count matches the supported sample count, we need to update the 
    // texture's texture format with the supported number of samples.
    m_TextureFormat.NumSamples = m_SampleDesc.Count;

    // Convert depth/stencil to typeless formats for the texture resource.
    m_ResourceFormat = Graphics::GetTextureFormat( dxgiFormat );
    // Convert typeless formats to depth/stencil formats.
    m_DSVFormat = Graphics::GetDepthStencilViewFormat( dxgiFormat );
    // Convert depth/stencil and typeless formats to shader resource view formats.
    m_SRVFormat = Graphics::GetShaderResourceViewFormat( dxgiFormat );
    // Convert typeless formats to render target view formats.
    m_RTVFormat = Graphics::GetRenderTargetViewFormat( dxgiFormat );

    m_BPP = static_cast<uint8_t>( BitsPerPixel( dxgiFormat ) );

    m_ResourceFormatSupport.Format = m_ResourceFormat;
    if ( FAILED( m_d3d12Device->CheckFeatureSupport( D3D12_FEATURE_FORMAT_SUPPORT, &m_ResourceFormatSupport, sizeof( D3D12_FEATURE_DATA_FORMAT_SUPPORT ) ) ) )
    {
        LOG_ERROR( "Failed to query texture resource format support." );
    }

    m_SRVFormatSupport.Format = m_SRVFormat;
    if ( FAILED( m_d3d12Device->CheckFeatureSupport( D3D12_FEATURE_FORMAT_SUPPORT, &m_SRVFormatSupport, sizeof( D3D12_FEATURE_DATA_FORMAT_SUPPORT ) ) ) )
    {
        LOG_ERROR( "Failed to query texture shader resource view format support." );
    }
    else if ( CheckSRVSupport( m_SRVFormatSupport.Support1 ) && m_ShaderResourceView.ptr == 0 )
    {
        m_ShaderResourceView = device->AllocateDescriptors( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
    }

    m_RTVFormatSupport.Format = m_RTVFormat;
    if ( FAILED( m_d3d12Device->CheckFeatureSupport( D3D12_FEATURE_FORMAT_SUPPORT, &m_RTVFormatSupport, sizeof( D3D12_FEATURE_DATA_FORMAT_SUPPORT ) ) ) )
    {
        LOG_ERROR( "Failed to query texture render target view format support." );
    }
    else 
    {
        if ( CheckRTVSupport( m_RTVFormatSupport.Support1 ) && m_RenderTargetView.ptr == 0 )
        {
            m_RenderTargetView = device->AllocateDescriptors( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
        }
        if ( CheckUAVSupport( m_RTVFormatSupport.Support1 ) && m_UnorderedAccessView.ptr == 0 )
        {
            m_UnorderedAccessView = device->AllocateDescriptors( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 15 );
        }
    }

    m_DSVFormatSupport.Format = m_DSVFormat;
    if ( FAILED( m_d3d12Device->CheckFeatureSupport( D3D12_FEATURE_FORMAT_SUPPORT, &m_DSVFormatSupport, sizeof( D3D12_FEATURE_DATA_FORMAT_SUPPORT ) ) ) )
    {
        LOG_ERROR( "Failed to query texture depth stencil format support." );
    }
    else if ( CheckDSVSupport( m_DSVFormatSupport.Support1 ) && m_DepthStencilView.ptr == 0 )
    {
        m_DepthStencilView = device->AllocateDescriptors( D3D12_DESCRIPTOR_HEAP_TYPE_DSV );
    }

}

TextureDimension TextureDX12::GetTextureDimension()
{
    return m_TextureDimension;
}

TextureFormat TextureDX12::GetTextureFormat()
{
    return m_TextureFormat;
}

// Compute the number of mipmap levels required for the given dimension.
uint8_t ComputeMipLevels( uint64_t size )
{
    DWORD MSB;
    _BitScanReverse64( &MSB, size );
    return static_cast<uint8_t>( MSB + 1 );
}

bool TextureDX12::LoadTexture2D( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, const std::wstring& fileName )
{
    const Application& app = Application::Get();
    const auto& searchPaths = app.GetAssetSerachPaths();

    fs::path filePath( fileName );
    
    auto searchPathIterator = searchPaths.cbegin();
    bool fileFound = fs::exists( filePath ) && fs::is_regular_file( filePath );
    while ( !fileFound && searchPathIterator != searchPaths.cend() )
    {
        filePath = ( *searchPathIterator ) / filePath.filename();
        fileFound = fs::exists( filePath ) && fs::is_regular_file( filePath );
        ++searchPathIterator;
    } 

    if ( !fileFound )
    {
        LOG_ERROR( "Could not find texture: ", fileName );
        return false;
    }

    LOG_INFO( "Loading texture ", filePath );
    m_TextureFileName = fileName;

    // Try to determine the file type from the image file.
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeU( filePath.c_str() );
    if ( fif == FIF_UNKNOWN )
    {
        fif = FreeImage_GetFIFFromFilenameU( filePath.c_str() );
    }

    if ( fif == FIF_UNKNOWN || !FreeImage_FIFSupportsReading( fif ) )
    {
        LOG_ERROR( "Unknow file format: " , filePath.string() );
        return false;
    }

    FIBITMAP* dib = FreeImage_LoadU( fif, filePath.c_str() );
    if ( dib == nullptr || FreeImage_HasPixels( dib ) == FALSE )
    {
        LOG_ERROR( "Failed to load image: " , filePath.string() );
        return false;
    }

    m_BPP = FreeImage_GetBPP( dib );
    FREE_IMAGE_TYPE imageType = FreeImage_GetImageType( dib );

    // Check to see if the texture has an alpha channel.
    m_IsTransparent = ( FreeImage_IsTransparent( dib ) == TRUE );

    DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;
    switch ( m_BPP )
    {
    case 8:
    {
        switch ( imageType )
        {
        case FIT_BITMAP:
            dxgiFormat = DXGI_FORMAT_R8_UNORM;
            break;
        default:
            LOG_ERROR( "Unknown image format." );
            return false;
        }
    }
    break;
    case 16:
    {
        switch ( imageType )
        {
        case FIT_BITMAP:
            dxgiFormat = DXGI_FORMAT_R8G8_UNORM;
            break;
        case FIT_UINT16:
            dxgiFormat = DXGI_FORMAT_R16_UINT;
            break;
        case FIT_INT16:
            dxgiFormat = DXGI_FORMAT_R16_SINT;
        break;
        default:
            LOG_ERROR( "Unknown image format." );
            return false;
        }
    }
    break;
    case 32:
    {
        switch ( imageType )
        {
        case FIT_BITMAP:
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
            dxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
#else
            dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
#endif
            break;
        case FIT_FLOAT:
            dxgiFormat = DXGI_FORMAT_R32_FLOAT;
            break;
        case FIT_INT32:
            dxgiFormat = DXGI_FORMAT_R32_SINT;
            break;
        case FIT_UINT32:
            dxgiFormat = DXGI_FORMAT_R32_UINT;
            break;
        default:
            LOG_ERROR( "Unknown image format." );
            return false;
        }
    }
    break;
    default:
    {
        FIBITMAP* dib32 = FreeImage_ConvertTo32Bits( dib );

        // Unload the original image.
        FreeImage_Unload( dib );

        dib = dib32;

        // Update pixel bit depth (should be 32 now if it wasn't before).
        m_BPP = FreeImage_GetBPP( dib );

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
        dxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
#else
        dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
#endif
    }
    break;
    }

    m_TextureDimension = TextureDimension::Texture2D;
    m_TextureFormat = ConvertTextureFormat( dxgiFormat );

    InitFormats( m_TextureFormat );

    m_Width = FreeImage_GetWidth( dib );
    m_Height = FreeImage_GetHeight( dib );
    m_DepthOrArraySize = 1;
    m_Pitch = FreeImage_GetPitch( dib );
    m_MipLevels = ComputeMipLevels( m_Width| m_Height );

    // Resize the internal resource to match the texture dimensions.
    Resize( m_Width, m_Height, m_DepthOrArraySize, m_MipLevels );

    BYTE* textureData = FreeImage_GetBits( dib );
    copyCommandBuffer->SetTextureSubresource( shared_from_this(), 0, 0, textureData );

    FreeImage_Unload( dib );

    SetName( filePath.filename() );

    return true;
}

const std::wstring& TextureDX12::GetFileName() const
{
    return m_TextureFileName;
}

uint32_t TextureDX12::GetWidth() const
{
    return m_Width;
}

uint32_t TextureDX12::GetHeight() const
{
    return m_Height;
}

uint32_t TextureDX12::GetDepthOrArraySize() const
{
    return m_DepthOrArraySize;
}

uint8_t TextureDX12::GetBPP() const
{
    return m_BPP;
}

uint64_t TextureDX12::GetPitch() const
{
    return m_Pitch;
}

uint8_t TextureDX12::GetMipLevels() const
{
    return m_MipLevels;
}

bool TextureDX12::IsTransparent() const
{
    return m_IsTransparent;
}

void TextureDX12::Resize( uint32_t width, uint32_t height, uint32_t depthOrArraySize, uint8_t mipLevels )
{
    std::shared_ptr<DeviceDX12> device = m_Device.lock();
    if ( !device ) return;

    m_Width = width;
    m_Height = height;
    m_DepthOrArraySize = depthOrArraySize;

    // Check mipmapping support
    m_ResourceFormatSupport.Format = m_ResourceFormat;
    
    // Determine mipmap support.
    if ( mipLevels > 1 && SUCCEEDED( m_d3d12Device->CheckFeatureSupport( D3D12_FEATURE_FORMAT_SUPPORT, &m_ResourceFormatSupport, sizeof( D3D12_FEATURE_DATA_FORMAT_SUPPORT ) ) ) )
    {
        if ( ( m_ResourceFormatSupport.Support1 & D3D12_FORMAT_SUPPORT1_MIP ) == 0 )
        {
            // Mipmapping is not supported with this format.
            m_MipLevels = 1;
        }
        else
        {
            m_MipLevels = mipLevels;
        }
    }
    else
    {
        // Either mipLevls is <= 1 or query the feature support failed. In either case, set mip levels to 1.
        m_MipLevels = 1;
    }

    D3D12_RESOURCE_DESC d3d12ResourceDesc = {};
    
    switch ( m_TextureDimension )
    {
    case TextureDimension::Texture1D:
    case TextureDimension::Texture1DArray:
        d3d12ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
        break;
    case TextureDimension::Texture2D:
    case TextureDimension::Texture2DArray:
    case TextureDimension::TextureCube:
    case TextureDimension::TextureCubeArray:
        d3d12ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        break;
    case TextureDimension::Texture3D:
        d3d12ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        break;
    default:
        LOG_ERROR( "Invalid texture dimension." );
        return;
    }

    d3d12ResourceDesc.Width = m_Width;
    d3d12ResourceDesc.Height = m_Height;
    d3d12ResourceDesc.DepthOrArraySize = m_DepthOrArraySize;
    d3d12ResourceDesc.MipLevels = m_MipLevels;
    d3d12ResourceDesc.Format = m_ResourceFormat;
    d3d12ResourceDesc.SampleDesc = m_SampleDesc;

    d3d12ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    d3d12ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_CLEAR_VALUE d3d12ClearValue = {};
    d3d12ClearValue.Format = m_SRVFormat;
    d3d12ClearValue.Color[0] = 0.0f;
    d3d12ClearValue.Color[1] = 0.0f;
    d3d12ClearValue.Color[2] = 0.0f;
    d3d12ClearValue.Color[3] = 1.0f;

    if ( ( m_DSVFormatSupport.Support1 & D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL ) != 0 )
    {
        d3d12ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        d3d12ClearValue.Format = m_DSVFormat;
        d3d12ClearValue.DepthStencil = { 1.0f, 0 };
    }
    if ( ( m_RTVFormatSupport.Support1 & D3D12_FORMAT_SUPPORT1_RENDER_TARGET ) != 0 )
    {
        d3d12ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        d3d12ClearValue.Format = m_RTVFormat;
    }
    if ( d3d12ResourceDesc.SampleDesc.Count <= 1 && ( m_RTVFormatSupport.Support1 & D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW ) != 0 )
    {
        // MSAA textures cannot be used with UAVs
        d3d12ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    m_d3d12ResourceState = D3D12_RESOURCE_STATE_COMMON;

    if ( FAILED( m_d3d12Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT ),
        D3D12_HEAP_FLAG_NONE,
        &d3d12ResourceDesc,
        m_d3d12ResourceState,
        &d3d12ClearValue,
        IID_PPV_ARGS( &m_d3d12Resource ) ) ) )
    {
        LOG_ERROR( "Failed to allocated committed resource." );
        return;
    }

    if ( !m_ResourceName.empty() )
    {
        m_d3d12Resource->SetName( m_ResourceName.c_str() );
    }

    InitViews( d3d12ResourceDesc );
}

DXGI_SAMPLE_DESC TextureDX12::GetSampleDesc() const
{
    return m_SampleDesc;
}

DXGI_FORMAT TextureDX12::GetResourceFormat() const
{
    return m_ResourceFormat;
}

DXGI_FORMAT TextureDX12::GetSRVFormat() const
{
    return m_SRVFormat;
}

DXGI_FORMAT TextureDX12::GetRTVFormat() const
{
    return m_RTVFormat;
}

DXGI_FORMAT TextureDX12::GetDSVFormat() const
{
    return m_DSVFormat;
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureDX12::GetShaderResourceView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t index )
{
    if ( commandBuffer )
    {
        if ( commandBuffer->GetD3D12CommandListType() == D3D12_COMMAND_LIST_TYPE_COMPUTE )
        {
            commandBuffer->TransitionResoure( std::static_pointer_cast<ResourceDX12>( shared_from_this() ), ResourceState::NonPixelShader );
        }
        else
        {
            commandBuffer->TransitionResoure( std::static_pointer_cast<ResourceDX12>( shared_from_this() ), ResourceState::PixelShader | ResourceState::NonPixelShader );
        }
    }
    return m_ShaderResourceView;
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureDX12::GetRenderTargetView() const
{
    return m_RenderTargetView;
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureDX12::GetUnorderedAccessView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t index )
{
    if ( commandBuffer )
    {
        commandBuffer->TransitionResoure( std::static_pointer_cast<ResourceDX12>( shared_from_this() ), ResourceState::UAV );
    }
    return CD3DX12_CPU_DESCRIPTOR_HANDLE( m_UnorderedAccessView, index * m_DescriptorIncrementSize );
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureDX12::GetDepthStencilView() const
{
    return m_DepthStencilView;
}

void TextureDX12::InitViews( const D3D12_RESOURCE_DESC& d3d12ResourceDesc )
{
    if ( (d3d12ResourceDesc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE ) == 0 && CheckSRVSupport( m_SRVFormatSupport.Support1 ) )
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC d3d12ShaderResourceViewDesc = {};
        d3d12ShaderResourceViewDesc.Format = m_SRVFormat;
        d3d12ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        switch ( m_TextureDimension )
        {
        case TextureDimension::Texture1D:
            d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
            d3d12ShaderResourceViewDesc.Texture1D.MipLevels = m_MipLevels;
            d3d12ShaderResourceViewDesc.Texture1D.MostDetailedMip = 0;
            d3d12ShaderResourceViewDesc.Texture1D.ResourceMinLODClamp = 0;
            break;
        case TextureDimension::Texture1DArray:
            d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
            d3d12ShaderResourceViewDesc.Texture1DArray.ArraySize = m_DepthOrArraySize;
            d3d12ShaderResourceViewDesc.Texture1DArray.FirstArraySlice = 0;
            d3d12ShaderResourceViewDesc.Texture1DArray.MipLevels = m_MipLevels;
            d3d12ShaderResourceViewDesc.Texture1DArray.MostDetailedMip = 0;
            d3d12ShaderResourceViewDesc.Texture1DArray.ResourceMinLODClamp = 0;
            break;
        case TextureDimension::Texture2D:
            if ( d3d12ResourceDesc.SampleDesc.Count > 1 )
            {
                d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
            }
            else
            {
                d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                d3d12ShaderResourceViewDesc.Texture2D.MipLevels = m_MipLevels;
                d3d12ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
                d3d12ShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
                d3d12ShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0;
            }

            break;
        case TextureDimension::Texture2DArray:
            if ( d3d12ResourceDesc.SampleDesc.Count > 1 )
            {
                d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
                d3d12ShaderResourceViewDesc.Texture2DMSArray.ArraySize = m_DepthOrArraySize;
                d3d12ShaderResourceViewDesc.Texture2DMSArray.FirstArraySlice = 0;
            }
            else
            {
                d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
                d3d12ShaderResourceViewDesc.Texture2DArray.ArraySize = m_DepthOrArraySize;
                d3d12ShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
                d3d12ShaderResourceViewDesc.Texture2DArray.MipLevels = m_MipLevels;
                d3d12ShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
                d3d12ShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
                d3d12ShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0;
            }
            break;
        case TextureDimension::Texture3D:
            d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
            d3d12ShaderResourceViewDesc.Texture3D.MipLevels = m_MipLevels;
            d3d12ShaderResourceViewDesc.Texture3D.MostDetailedMip = 0;
            d3d12ShaderResourceViewDesc.Texture3D.ResourceMinLODClamp = 0;
            break;
        case TextureDimension::TextureCube:
            d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
            d3d12ShaderResourceViewDesc.TextureCube.MipLevels = m_MipLevels;
            d3d12ShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
            d3d12ShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0;
            break;
        case TextureDimension::TextureCubeArray:
            d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
            d3d12ShaderResourceViewDesc.TextureCubeArray.First2DArrayFace = 0;
            d3d12ShaderResourceViewDesc.TextureCubeArray.MipLevels = m_MipLevels;
            d3d12ShaderResourceViewDesc.TextureCubeArray.MostDetailedMip = 0;
            d3d12ShaderResourceViewDesc.TextureCubeArray.NumCubes = m_DepthOrArraySize;
            d3d12ShaderResourceViewDesc.TextureCubeArray.ResourceMinLODClamp = 0;
            break;
        }

        m_d3d12Device->CreateShaderResourceView( m_d3d12Resource.Get(), &d3d12ShaderResourceViewDesc, m_ShaderResourceView );
    }

    if ( ( d3d12ResourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET ) != 0 && CheckRTVSupport( m_RTVFormatSupport.Support1 ) )
    {
        D3D12_RENDER_TARGET_VIEW_DESC d3d12RenderTargetViewDesc = {};

        d3d12RenderTargetViewDesc.Format = m_RTVFormat;

        switch ( m_TextureDimension )
        {
        case TextureDimension::Texture1D:
            d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
            d3d12RenderTargetViewDesc.Texture1D.MipSlice = 0;
            break;
        case TextureDimension::Texture1DArray:
            d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
            d3d12RenderTargetViewDesc.Texture1DArray.ArraySize = m_DepthOrArraySize;
            d3d12RenderTargetViewDesc.Texture1DArray.FirstArraySlice = 0;
            d3d12RenderTargetViewDesc.Texture1DArray.MipSlice = 0;
            break;
        case TextureDimension::Texture2D:
            if ( d3d12ResourceDesc.SampleDesc.Count > 1 )
            {
                d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
            }
            else
            {
                d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                d3d12RenderTargetViewDesc.Texture2D.MipSlice = 0;
                d3d12RenderTargetViewDesc.Texture2D.PlaneSlice = 0;
            }

            break;
        case TextureDimension::Texture2DArray:
            if ( d3d12ResourceDesc.SampleDesc.Count > 1 )
            {
                d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
                d3d12RenderTargetViewDesc.Texture2DMSArray.ArraySize = m_DepthOrArraySize;
                d3d12RenderTargetViewDesc.Texture2DMSArray.FirstArraySlice = 0;
            }
            else
            {
                d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                d3d12RenderTargetViewDesc.Texture2DArray.ArraySize = m_DepthOrArraySize;
                d3d12RenderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;
                d3d12RenderTargetViewDesc.Texture2DArray.MipSlice = 0;
                d3d12RenderTargetViewDesc.Texture2DArray.PlaneSlice = 0;
            }
            break;
        case TextureDimension::Texture3D:
            d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
            d3d12RenderTargetViewDesc.Texture3D.MipSlice = 0;
            d3d12RenderTargetViewDesc.Texture3D.FirstWSlice = 0;
            d3d12RenderTargetViewDesc.Texture3D.WSize = -1;
            break;

        }

        m_d3d12Device->CreateRenderTargetView( m_d3d12Resource.Get(), &d3d12RenderTargetViewDesc, m_RenderTargetView );
    }

    if ( ( d3d12ResourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS ) != 0 && CheckUAVSupport( m_RTVFormatSupport.Support1 ) )
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC d3d12UnorderedAccessViewDesc = {};

        switch ( m_TextureDimension )
        {
        case TextureDimension::Texture1D:
            d3d12UnorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
            d3d12UnorderedAccessViewDesc.Texture1D.MipSlice = 0;
            break;
        case TextureDimension::Texture1DArray:
            d3d12UnorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
            d3d12UnorderedAccessViewDesc.Texture1DArray.ArraySize = m_DepthOrArraySize;
            d3d12UnorderedAccessViewDesc.Texture1DArray.FirstArraySlice = 0;
            d3d12UnorderedAccessViewDesc.Texture1DArray.MipSlice = 0;
            break;
        case TextureDimension::Texture2D:
            d3d12UnorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            d3d12UnorderedAccessViewDesc.Texture2D.MipSlice = 0;
            d3d12UnorderedAccessViewDesc.Texture2D.PlaneSlice = 0;
            d3d12UnorderedAccessViewDesc.Texture2D.MipSlice = 0;
            break;
        case TextureDimension::Texture2DArray:
            d3d12UnorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
            d3d12UnorderedAccessViewDesc.Texture2DArray.ArraySize = m_DepthOrArraySize;
            d3d12UnorderedAccessViewDesc.Texture2DArray.FirstArraySlice = 0;
            d3d12UnorderedAccessViewDesc.Texture2DArray.MipSlice = 0;
            d3d12UnorderedAccessViewDesc.Texture2DArray.PlaneSlice = 0;
            break;
        case TextureDimension::Texture3D:
            d3d12UnorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
            d3d12UnorderedAccessViewDesc.Texture3D.FirstWSlice = 0;
            d3d12UnorderedAccessViewDesc.Texture3D.MipSlice = 0;
            d3d12UnorderedAccessViewDesc.Texture3D.WSize = m_DepthOrArraySize;
            break;
        }

        for ( UINT mipSlice = 0; mipSlice < m_MipLevels; ++mipSlice )
        {
            d3d12UnorderedAccessViewDesc.Texture1D.MipSlice = mipSlice;
            d3d12UnorderedAccessViewDesc.Texture1DArray.MipSlice = mipSlice;
            d3d12UnorderedAccessViewDesc.Texture2D.MipSlice = mipSlice;
            d3d12UnorderedAccessViewDesc.Texture2DArray.MipSlice = mipSlice;
            d3d12UnorderedAccessViewDesc.Texture3D.MipSlice = mipSlice;

            m_d3d12Device->CreateUnorderedAccessView( m_d3d12Resource.Get(), nullptr, &d3d12UnorderedAccessViewDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE( m_UnorderedAccessView, mipSlice * m_DescriptorIncrementSize ) );
        }
    }

    if ( ( d3d12ResourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL ) != 0 && CheckDSVSupport( m_DSVFormatSupport.Support1 ) )
    {
        D3D12_DEPTH_STENCIL_VIEW_DESC d3d12DepthStencilViewDesc = {};

        d3d12DepthStencilViewDesc.Format = m_DSVFormat;
        d3d12DepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

        switch ( m_TextureDimension )
        {
        case TextureDimension::Texture1D:
            d3d12DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
            d3d12DepthStencilViewDesc.Texture1D.MipSlice = 0;
            break;
        case TextureDimension::Texture1DArray:
            d3d12DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
            d3d12DepthStencilViewDesc.Texture1DArray.ArraySize = m_DepthOrArraySize;
            d3d12DepthStencilViewDesc.Texture1DArray.FirstArraySlice = 0;
            d3d12DepthStencilViewDesc.Texture1DArray.MipSlice = 0;
            break;
        case TextureDimension::Texture2D:
            if ( d3d12ResourceDesc.SampleDesc.Count > 1 )
            {
                d3d12DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
            }
            else
            {
                d3d12DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                d3d12DepthStencilViewDesc.Texture2D.MipSlice = 0;
            }
            break;
        case TextureDimension::Texture2DArray:
            if ( d3d12ResourceDesc.SampleDesc.Count > 1 )
            {
                d3d12DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
                d3d12DepthStencilViewDesc.Texture2DMSArray.ArraySize = m_DepthOrArraySize;
                d3d12DepthStencilViewDesc.Texture2DMSArray.FirstArraySlice = 0;
            }
            else
            {
                d3d12DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
                d3d12DepthStencilViewDesc.Texture2DArray.ArraySize = m_DepthOrArraySize;
                d3d12DepthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
                d3d12DepthStencilViewDesc.Texture2DArray.MipSlice = 0;
            }
            break;
        }

        m_d3d12Device->CreateDepthStencilView( m_d3d12Resource.Get(), &d3d12DepthStencilViewDesc, m_DepthStencilView );
    }
}
