#include <EnginePCH.h>

#include <Graphics/DXGI/TextureFormatDXGI.h>
#include <LogManager.h>

using namespace Graphics;

inline void ReportTextureFormatError( const TextureFormat& format, const std::string& message )
{
    std::stringstream ss;
    ss << message << std::endl;
    ss << "Components: ";
    switch ( format.Components )
    {
    case TextureComponents::R:
        ss << "R" << std::endl;
        break;
    case TextureComponents::RG:
        ss << "RG" << std::endl;
        break;
    case TextureComponents::RGB:
        ss << "RGB" << std::endl;
        break;
    case TextureComponents::RGBA:
        ss << "RGBA" << std::endl;
        break;
    case TextureComponents::BGR:
        ss << "BGR" << std::endl;
        break;
    case TextureComponents::BGRA:
        ss << "BGRA" << std::endl;
        break;
    case TextureComponents::A:
        ss << "A" << std::endl;
        break;
    case TextureComponents::Depth:
        ss << "Depth" << std::endl;
        break;
    case TextureComponents::DepthStencil:
        ss << "DepthStencil" << std::endl;
        break;
    case TextureComponents::BC1:
        ss << "BC1" << std::endl;
        break;
    case TextureComponents::BC2:
        ss << "BC2" << std::endl;
        break;
    case TextureComponents::BC3:
        ss << "BC3" << std::endl;
        break;
    case TextureComponents::BC4:
        ss << "BC4" << std::endl;
        break;
    case TextureComponents::BC5:
        ss << "BC5" << std::endl;
        break;
    case TextureComponents::BC6U:
        ss << "BC6U" << std::endl;
        break;
    case TextureComponents::BC6S:
        ss << "BC6S" << std::endl;
        break;
    case TextureComponents::BC7:
        ss << "BC7" << std::endl;
        break;
    default:
        ss << "Unknown" << std::endl;
        break;
    }

    ss << "Type: ";
    switch ( format.Type )
    {
    case TextureType::Typeless:
        ss << "Typeless" << std::endl;
        break;
    case TextureType::UnsignedNormalized:
        ss << "UnsignedNormalized" << std::endl;
        break;
    case TextureType::SignedNormalized:
        ss << "SignedNormalized" << std::endl;
        break;
    case TextureType::Float:
        ss << "Float" << std::endl;
        break;
    case TextureType::UnsignedInteger:
        ss << "UnsignedInteger" << std::endl;
        break;
    case TextureType::SignedInteger:
        ss << "SignedInteger" << std::endl;
        break;
    case TextureType::sRGB:
        ss << "sRGB" << std::endl;
        break;
    default:
        ss << "Unknown" << std::endl;
        break;
    }

    ss << "RedBits:     " << (int32_t)format.RedBits << std::endl;
    ss << "GreenBits:   " << (int32_t)format.GreenBits << std::endl;
    ss << "BlueBits:    " << (int32_t)format.BlueBits << std::endl;
    ss << "AlphaBits:   " << (int32_t)format.AlphaBits << std::endl;
    ss << "DepthBits:   " << (int32_t)format.DepthBits << std::endl;
    ss << "StencilBits: " << (int32_t)format.StencilBits << std::endl;
    ss << "Num Samples: " << (int32_t)format.NumSamples << std::endl;

    LOG_ERROR( ss.str() );
}

namespace Graphics
{
    const TextureFormat ConvertTextureFormat( DXGI_FORMAT dxgiFormat, uint8_t numSamples )
    {
        TextureFormat textureFormat;
        textureFormat.NumSamples = numSamples;

        switch ( dxgiFormat )
        {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 32;
            textureFormat.BlueBits = 32;
            textureFormat.AlphaBits = 32;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::Float;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 32;
            textureFormat.BlueBits = 32;
            textureFormat.AlphaBits = 32;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R32G32B32A32_UINT:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::UnsignedInteger;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 32;
            textureFormat.BlueBits = 32;
            textureFormat.AlphaBits = 32;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R32G32B32A32_SINT:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::SignedInteger;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 32;
            textureFormat.BlueBits = 32;
            textureFormat.AlphaBits = 32;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R32G32B32_TYPELESS:
            textureFormat.Components = TextureComponents::RGB;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 32;
            textureFormat.BlueBits = 32;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R32G32B32_FLOAT:
            textureFormat.Components = TextureComponents::RGB;
            textureFormat.Type = TextureType::Float;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 32;
            textureFormat.BlueBits = 32;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R32G32B32_UINT:
            textureFormat.Components = TextureComponents::RGB;
            textureFormat.Type = TextureType::UnsignedInteger;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 32;
            textureFormat.BlueBits = 32;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R32G32B32_SINT:
            textureFormat.Components = TextureComponents::RGB;
            textureFormat.Type = TextureType::SignedInteger;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 32;
            textureFormat.BlueBits = 32;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 16;
            textureFormat.BlueBits = 16;
            textureFormat.AlphaBits = 16;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::Float;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 16;
            textureFormat.BlueBits = 16;
            textureFormat.AlphaBits = 16;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16G16B16A16_UNORM:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 16;
            textureFormat.BlueBits = 16;
            textureFormat.AlphaBits = 16;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16G16B16A16_UINT:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::UnsignedInteger;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 16;
            textureFormat.BlueBits = 16;
            textureFormat.AlphaBits = 16;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16G16B16A16_SNORM:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::SignedNormalized;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 16;
            textureFormat.BlueBits = 16;
            textureFormat.AlphaBits = 16;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16G16B16A16_SINT:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::SignedInteger;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 16;
            textureFormat.BlueBits = 16;
            textureFormat.AlphaBits = 16;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R32G32_TYPELESS:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 32;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R32G32_FLOAT:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::Float;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 32;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R32G32_UINT:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::UnsignedInteger;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 32;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R32G32_SINT:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::SignedInteger;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 32;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            textureFormat.Components = TextureComponents::DepthStencil;
            textureFormat.Type = TextureType::Float;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 32;
            textureFormat.StencilBits = 8;
            break;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 10;
            textureFormat.GreenBits = 10;
            textureFormat.BlueBits = 10;
            textureFormat.AlphaBits = 2;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R10G10B10A2_UNORM:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 10;
            textureFormat.GreenBits = 10;
            textureFormat.BlueBits = 10;
            textureFormat.AlphaBits = 2;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R10G10B10A2_UINT:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::UnsignedInteger;
            textureFormat.RedBits = 10;
            textureFormat.GreenBits = 10;
            textureFormat.BlueBits = 10;
            textureFormat.AlphaBits = 2;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R11G11B10_FLOAT:
            textureFormat.Components = TextureComponents::RGB;
            textureFormat.Type = TextureType::Float;
            textureFormat.RedBits = 11;
            textureFormat.GreenBits = 11;
            textureFormat.BlueBits = 10;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.BlueBits = 8;
            textureFormat.AlphaBits = 8;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.BlueBits = 8;
            textureFormat.AlphaBits = 8;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::sRGB;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.BlueBits = 8;
            textureFormat.AlphaBits = 8;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R8G8B8A8_UINT:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::UnsignedInteger;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.BlueBits = 8;
            textureFormat.AlphaBits = 8;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R8G8B8A8_SNORM:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::SignedNormalized;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.BlueBits = 8;
            textureFormat.AlphaBits = 8;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R8G8B8A8_SINT:
            textureFormat.Components = TextureComponents::RGBA;
            textureFormat.Type = TextureType::SignedInteger;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.BlueBits = 8;
            textureFormat.AlphaBits = 8;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16G16_TYPELESS:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 16;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16G16_FLOAT:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::Float;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 16;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16G16_UNORM:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 16;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16G16_UINT:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::UnsignedInteger;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 16;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16G16_SNORM:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::SignedNormalized;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 16;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16G16_SINT:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::SignedInteger;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 16;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R32_TYPELESS:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_D32_FLOAT:
            textureFormat.Components = TextureComponents::Depth;
            textureFormat.Type = TextureType::Float;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 32;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R32_FLOAT:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::Float;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R32_UINT:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::UnsignedInteger;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R32_SINT:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::SignedInteger;
            textureFormat.RedBits = 32;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R24G8_TYPELESS:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 24;
            textureFormat.GreenBits = 8;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            textureFormat.Components = TextureComponents::DepthStencil;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 24;
            textureFormat.StencilBits = 8;
            break;
        case DXGI_FORMAT_R8G8_TYPELESS:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R8G8_UNORM:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R8G8_UINT:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::UnsignedInteger;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R8G8_SNORM:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::SignedNormalized;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R8G8_SINT:
            textureFormat.Components = TextureComponents::RG;
            textureFormat.Type = TextureType::SignedInteger;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16_TYPELESS:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16_FLOAT:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::Float;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_D16_UNORM:
            textureFormat.Components = TextureComponents::Depth;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 16;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16_UNORM:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16_UINT:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::UnsignedInteger;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16_SNORM:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::SignedNormalized;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R16_SINT:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::SignedInteger;
            textureFormat.RedBits = 16;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R8_TYPELESS:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R8_UNORM:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R8_UINT:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::UnsignedInteger;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R8_SNORM:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::SignedNormalized;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R8_SINT:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::SignedInteger;
            textureFormat.RedBits = 8;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_A8_UNORM:
            textureFormat.Components = TextureComponents::A;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 8;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_R1_UNORM:
            textureFormat.Components = TextureComponents::R;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 1;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC1_TYPELESS:
            textureFormat.Components = TextureComponents::BC1;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC1_UNORM:
            textureFormat.Components = TextureComponents::BC1;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC1_UNORM_SRGB:
            textureFormat.Components = TextureComponents::BC1;
            textureFormat.Type = TextureType::sRGB;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC2_TYPELESS:
            textureFormat.Components = TextureComponents::BC2;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC2_UNORM:
            textureFormat.Components = TextureComponents::BC2;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC2_UNORM_SRGB:
            textureFormat.Components = TextureComponents::BC2;
            textureFormat.Type = TextureType::sRGB;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC3_TYPELESS:
            textureFormat.Components = TextureComponents::BC3;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC3_UNORM:
            textureFormat.Components = TextureComponents::BC3;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC3_UNORM_SRGB:
            textureFormat.Components = TextureComponents::BC3;
            textureFormat.Type = TextureType::sRGB;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC4_TYPELESS:
            textureFormat.Components = TextureComponents::BC4;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC4_UNORM:
            textureFormat.Components = TextureComponents::BC4;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC4_SNORM:
            textureFormat.Components = TextureComponents::BC4;
            textureFormat.Type = TextureType::SignedNormalized;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC5_TYPELESS:
            textureFormat.Components = TextureComponents::BC5;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC5_UNORM:
            textureFormat.Components = TextureComponents::BC5;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC5_SNORM:
            textureFormat.Components = TextureComponents::BC5;
            textureFormat.Type = TextureType::SignedNormalized;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_B5G6R5_UNORM:
            textureFormat.Components = TextureComponents::BGR;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.BlueBits = 5;
            textureFormat.GreenBits = 6;
            textureFormat.RedBits = 5;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_B5G5R5A1_UNORM:
            textureFormat.Components = TextureComponents::BGRA;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.BlueBits = 5;
            textureFormat.GreenBits = 5;
            textureFormat.RedBits = 5;
            textureFormat.AlphaBits = 1;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            textureFormat.Components = TextureComponents::BGRA;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.BlueBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.RedBits = 8;
            textureFormat.AlphaBits = 8;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_B8G8R8X8_UNORM:
            textureFormat.Components = TextureComponents::BGRA;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.BlueBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.RedBits = 8;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
            textureFormat.Components = TextureComponents::BGRA;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.BlueBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.RedBits = 8;
            textureFormat.AlphaBits = 8;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            textureFormat.Components = TextureComponents::BGRA;
            textureFormat.Type = TextureType::sRGB;
            textureFormat.BlueBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.RedBits = 8;
            textureFormat.AlphaBits = 8;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
            textureFormat.Components = TextureComponents::BGRA;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.BlueBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.RedBits = 8;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            textureFormat.Components = TextureComponents::BGRA;
            textureFormat.Type = TextureType::sRGB;
            textureFormat.BlueBits = 8;
            textureFormat.GreenBits = 8;
            textureFormat.RedBits = 8;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC6H_TYPELESS:
            textureFormat.Components = TextureComponents::BC6S; // Could also be BC6U for typeless formats.
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC6H_UF16:
            textureFormat.Components = TextureComponents::BC6U;
            textureFormat.Type = TextureType::Float; // Unsigned 16-bit floating point format.
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC6H_SF16:
            textureFormat.Components = TextureComponents::BC6S;
            textureFormat.Type = TextureType::Float; // Signed 16-bit floating point format.
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC7_TYPELESS:
            textureFormat.Components = TextureComponents::BC7;
            textureFormat.Type = TextureType::Typeless;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC7_UNORM:
            textureFormat.Components = TextureComponents::BC7;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            textureFormat.Components = TextureComponents::BC7;
            textureFormat.Type = TextureType::sRGB;
            textureFormat.RedBits = 0;
            textureFormat.GreenBits = 0;
            textureFormat.BlueBits = 0;
            textureFormat.AlphaBits = 0;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        case DXGI_FORMAT_B4G4R4A4_UNORM:
            textureFormat.Components = TextureComponents::BGRA;
            textureFormat.Type = TextureType::UnsignedNormalized;
            textureFormat.BlueBits = 4;
            textureFormat.GreenBits = 4;
            textureFormat.RedBits = 4;
            textureFormat.AlphaBits = 4;
            textureFormat.DepthBits = 0;
            textureFormat.StencilBits = 0;
            break;
        default:
            LOG_ERROR( "Unsupported DXGI format." );
            break;
        }

        return textureFormat;
    }

    DXGI_FORMAT ConvertTextureFormat( const TextureFormat& textureFormat )
    {
        DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;

        switch ( textureFormat.Components )
        {
        case TextureComponents::R:
            switch ( textureFormat.Type )
            {
            case TextureType::Typeless:
                if ( textureFormat.RedBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8_TYPELESS;
                }
                else if ( textureFormat.RedBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16_TYPELESS;
                }
                else if ( textureFormat.RedBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_R32_TYPELESS;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose the best format based on the requested format.
                    if ( textureFormat.RedBits > 16 )
                    {
                        dxgiFormat = DXGI_FORMAT_R32_TYPELESS;
                    }
                    else if ( textureFormat.RedBits > 8 )
                    {
                        dxgiFormat = DXGI_FORMAT_R16_TYPELESS;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R8_TYPELESS;
                    }
                }
                break;
            case TextureType::UnsignedNormalized:
                if ( textureFormat.RedBits == 1 )
                {
                    dxgiFormat = DXGI_FORMAT_R1_UNORM;
                }
                else if ( textureFormat.RedBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8_UNORM;
                }
                else if ( textureFormat.RedBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16_UNORM;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative to the requested format.
                    if ( textureFormat.RedBits > 8 )
                    {
                        dxgiFormat = DXGI_FORMAT_R16_UNORM;
                    }
                    else if ( textureFormat.RedBits > 1 )
                    {
                        dxgiFormat = DXGI_FORMAT_R8_UNORM;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R1_UNORM;
                    }
                }
                break;
            case TextureType::SignedNormalized:
                if ( textureFormat.RedBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8_SNORM;
                }
                else if ( textureFormat.RedBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16_SNORM;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative.
                    if ( textureFormat.RedBits > 8 )
                    {
                        dxgiFormat = DXGI_FORMAT_R16_SNORM;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R8_SNORM;
                    }
                }
                break;
            case TextureType::Float:
                if ( textureFormat.RedBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16_FLOAT;
                }
                else if ( textureFormat.RedBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_R32_FLOAT;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative.
                    if ( textureFormat.RedBits > 16 )
                    {
                        dxgiFormat = DXGI_FORMAT_R32_FLOAT;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R16_FLOAT;
                    }
                }
                break;
            case TextureType::UnsignedInteger:
                if ( textureFormat.RedBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8_UINT;
                }
                else if ( textureFormat.RedBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16_UINT;
                }
                else if ( textureFormat.RedBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_R32_UINT;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative.
                    if ( textureFormat.RedBits > 16 )
                    {
                        dxgiFormat = DXGI_FORMAT_R32_UINT;
                    }
                    else if ( textureFormat.RedBits > 8 )
                    {
                        dxgiFormat = DXGI_FORMAT_R16_UINT;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R8_UINT;
                    }
                }
                break;
            case TextureType::SignedInteger:
                if ( textureFormat.RedBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8_SINT;
                }
                else if ( textureFormat.RedBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16_SINT;
                }
                else if ( textureFormat.RedBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_R32_SINT;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative
                    if ( textureFormat.RedBits > 16 )
                    {
                        dxgiFormat = DXGI_FORMAT_R32_SINT;
                    }
                    else if ( textureFormat.RedBits > 8 )
                    {
                        dxgiFormat = DXGI_FORMAT_R16_SINT;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R8_SINT;
                    }
                }
                break;
            default:
                ReportTextureFormatError( textureFormat, "Unknown texture format." );
                break;
            }
            break;
        case TextureComponents::RG:
            switch ( textureFormat.Type )
            {
            case TextureType::Typeless:
                if ( textureFormat.RedBits == 8 && textureFormat.GreenBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8G8_TYPELESS;
                }
                else if ( textureFormat.RedBits == 16 && textureFormat.GreenBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16G16_TYPELESS;
                }
                else if ( textureFormat.RedBits == 24 && textureFormat.GreenBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R24G8_TYPELESS;
                }
                else if ( textureFormat.RedBits == 32 && textureFormat.GreenBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_R32G32_TYPELESS;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose the best format based on the requested format.
                    if ( textureFormat.RedBits > 24 || textureFormat.GreenBits > 16 )
                    {
                        dxgiFormat = DXGI_FORMAT_R32G32_TYPELESS;
                    }
                    else if ( textureFormat.RedBits > 16 && textureFormat.GreenBits <= 8 )
                    {
                        dxgiFormat = DXGI_FORMAT_R24G8_TYPELESS;
                    }
                    else if ( textureFormat.RedBits > 8 || textureFormat.GreenBits > 8 )
                    {
                        dxgiFormat = DXGI_FORMAT_R16G16_TYPELESS;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R8G8_TYPELESS;
                    }
                }
                break;
            case TextureType::UnsignedNormalized:
                if ( textureFormat.RedBits == 8 && textureFormat.GreenBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8G8_UNORM;
                }
                else if ( textureFormat.RedBits == 16 && textureFormat.GreenBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16G16_UNORM;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative.
                    if ( textureFormat.RedBits > 8 || textureFormat.GreenBits > 8 )
                    {
                        dxgiFormat = DXGI_FORMAT_R16G16_UNORM;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R8G8_UNORM;
                    }
                }
                break;
            case TextureType::SignedNormalized:
                if ( textureFormat.RedBits == 8 && textureFormat.GreenBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8G8_SNORM;
                }
                else if ( textureFormat.RedBits == 16 && textureFormat.GreenBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16G16_SNORM;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative.
                    if ( textureFormat.RedBits > 8 || textureFormat.GreenBits > 8 )
                    {
                        dxgiFormat = DXGI_FORMAT_R16G16_SNORM;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R8G8_SNORM;
                    }
                }
                break;
            case TextureType::Float:
                if ( textureFormat.RedBits == 16 && textureFormat.GreenBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16G16_FLOAT;
                }
                else if ( textureFormat.RedBits == 32 && textureFormat.GreenBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative.
                    if ( textureFormat.RedBits > 16 || textureFormat.GreenBits > 16 )
                    {
                        dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R16G16_FLOAT;
                    }
                }
                break;
            case TextureType::UnsignedInteger:
                if ( textureFormat.RedBits == 8 && textureFormat.GreenBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8G8_UINT;
                }
                else if ( textureFormat.RedBits == 16 && textureFormat.GreenBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16G16_UINT;
                }
                else if ( textureFormat.RedBits == 32 && textureFormat.GreenBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_R32G32_UINT;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative.
                    if ( textureFormat.RedBits > 16 || textureFormat.GreenBits > 16 )
                    {
                        dxgiFormat = DXGI_FORMAT_R32G32_UINT;
                    }
                    else if ( textureFormat.RedBits > 8 || textureFormat.GreenBits > 8 )
                    {
                        dxgiFormat = DXGI_FORMAT_R16G16_UINT;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R8G8_UINT;
                    }
                }
                break;
            case TextureType::SignedInteger:
                if ( textureFormat.RedBits == 8 && textureFormat.GreenBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8G8_SINT;
                }
                else if ( textureFormat.RedBits == 16 && textureFormat.GreenBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16G16_SINT;
                }
                else if ( textureFormat.RedBits == 32 && textureFormat.GreenBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_R32G32_SINT;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative.
                    if ( textureFormat.RedBits > 16 || textureFormat.GreenBits > 16 )
                    {
                        dxgiFormat = DXGI_FORMAT_R32G32_SINT;
                    }
                    else if ( textureFormat.RedBits > 8 || textureFormat.GreenBits > 8 )
                    {
                        dxgiFormat = DXGI_FORMAT_R16G16_SINT;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R8G8_SINT;
                    }
                }
                break;
            default:
                ReportTextureFormatError( textureFormat, "Unknown texture format." );
                break;
            }
            break;
        case TextureComponents::RGB:
            switch ( textureFormat.Type )
            {
            case TextureType::Typeless:
                if ( textureFormat.RedBits == 32 && textureFormat.GreenBits == 32 && textureFormat.BlueBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_R32G32B32_TYPELESS;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // There is only 1 RGB typeless format
                    dxgiFormat = DXGI_FORMAT_R32G32B32_TYPELESS;
                }
                break;
            case TextureType::Float:
                if ( textureFormat.RedBits == 11 && textureFormat.GreenBits == 11 && textureFormat.BlueBits == 10 )
                {
                    dxgiFormat = DXGI_FORMAT_R11G11B10_FLOAT;
                }
                else if ( textureFormat.RedBits == 32 && textureFormat.GreenBits == 32 && textureFormat.BlueBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative.
                    if ( textureFormat.RedBits > 11 || textureFormat.GreenBits > 11 || textureFormat.BlueBits > 10 )
                    {
                        dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R11G11B10_FLOAT;
                    }
                }
                break;
            case TextureType::UnsignedInteger:
                if ( textureFormat.RedBits == 32 && textureFormat.GreenBits == 32 && textureFormat.BlueBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_R32G32B32_UINT;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // There is only 1 3-component UINT format.
                    dxgiFormat = DXGI_FORMAT_R32G32B32_UINT;
                }
                break;
            case TextureType::SignedInteger:
                if ( textureFormat.RedBits == 32 && textureFormat.GreenBits == 32 && textureFormat.BlueBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_R32G32B32_SINT;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // There is only 1 3-component SINT format.
                    dxgiFormat = DXGI_FORMAT_R32G32B32_SINT;
                }
                break;
            default:
                ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                // Try to choose a reasonable alternative
                switch ( textureFormat.Type )
                {
                case TextureType::UnsignedNormalized:
                    // This is a non-normalized format. May result in unintended behavior.
                    dxgiFormat = DXGI_FORMAT_R32G32B32_UINT;
                    break;
                case TextureType::SignedNormalized:
                    // Non-normalized format. May result in unintended behavior.
                    dxgiFormat = DXGI_FORMAT_R32G32B32_SINT;
                    break;
                default:
                    ReportTextureFormatError( textureFormat, "Unknown texture format." );
                    break;
                }
                break;
            }
            break;
        case TextureComponents::RGBA:
            switch ( textureFormat.Type )
            {
            case TextureType::Typeless:
                if ( textureFormat.RedBits == 8 && textureFormat.GreenBits == 8 && textureFormat.BlueBits == 8 && textureFormat.AlphaBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
                }
                else if ( textureFormat.RedBits == 10 && textureFormat.GreenBits == 10 && textureFormat.BlueBits == 10 && textureFormat.AlphaBits == 2 )
                {
                    dxgiFormat = DXGI_FORMAT_R10G10B10A2_TYPELESS;
                }
                else if ( textureFormat.RedBits == 16 && textureFormat.GreenBits == 16 && textureFormat.BlueBits == 16 && textureFormat.AlphaBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16G16B16A16_TYPELESS;
                }
                else if ( textureFormat.RedBits == 32 && textureFormat.GreenBits == 32 && textureFormat.BlueBits == 32 && textureFormat.AlphaBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_R32G32B32A32_TYPELESS;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose the best format based on the requested format.
                    if ( textureFormat.RedBits > 16 || textureFormat.GreenBits > 16 || textureFormat.BlueBits > 16 || textureFormat.AlphaBits > 16 )
                    {
                        dxgiFormat = DXGI_FORMAT_R32G32B32A32_TYPELESS;
                    }
                    else if ( ( textureFormat.RedBits > 10 || textureFormat.GreenBits > 10 || textureFormat.BlueBits > 10 ) && textureFormat.AlphaBits <= 2 )
                    {
                        dxgiFormat = DXGI_FORMAT_R10G10B10A2_TYPELESS;
                    }
                    else if ( textureFormat.RedBits > 8 || textureFormat.GreenBits > 8 || textureFormat.BlueBits > 8 || textureFormat.AlphaBits > 8 )
                    {
                        dxgiFormat = DXGI_FORMAT_R16G16B16A16_TYPELESS;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
                    }
                }
                break;
            case TextureType::UnsignedNormalized:
                if ( textureFormat.RedBits == 8 && textureFormat.GreenBits == 8 && textureFormat.BlueBits == 8 && textureFormat.AlphaBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
                }
                else if ( textureFormat.RedBits == 10 && textureFormat.GreenBits == 10 && textureFormat.BlueBits == 10 && textureFormat.AlphaBits == 2 )
                {
                    dxgiFormat = DXGI_FORMAT_R10G10B10A2_UNORM;
                }
                else if ( textureFormat.RedBits == 16 && textureFormat.GreenBits == 16 && textureFormat.BlueBits == 16 && textureFormat.AlphaBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16G16B16A16_UNORM;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative.
                    if ( textureFormat.RedBits > 10 || textureFormat.GreenBits > 10 || textureFormat.BlueBits > 10 || textureFormat.AlphaBits > 8 )
                    {
                        dxgiFormat = DXGI_FORMAT_R16G16B16A16_UNORM;
                    }
                    else if ( ( textureFormat.RedBits > 8 || textureFormat.GreenBits > 8 || textureFormat.BlueBits > 8 ) && textureFormat.AlphaBits <= 2 )
                    {
                        dxgiFormat = DXGI_FORMAT_R10G10B10A2_UNORM;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
                    }
                }
                break;
            case TextureType::SignedNormalized:
                if ( textureFormat.RedBits == 8 && textureFormat.GreenBits == 8 && textureFormat.BlueBits == 8 && textureFormat.AlphaBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8G8B8A8_SNORM;
                }
                else if ( textureFormat.RedBits == 16 && textureFormat.GreenBits == 16 && textureFormat.BlueBits == 16 && textureFormat.AlphaBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16G16B16A16_SNORM;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative.
                    if ( textureFormat.RedBits > 8 || textureFormat.GreenBits > 8 || textureFormat.BlueBits > 8 || textureFormat.AlphaBits > 8 )
                    {
                        dxgiFormat = DXGI_FORMAT_R16G16B16A16_SNORM;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R8G8B8A8_SNORM;
                    }
                }
                break;
            case TextureType::Float:
                if ( textureFormat.RedBits == 32 && textureFormat.GreenBits == 32 && textureFormat.BlueBits && textureFormat.AlphaBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
                }
                else if ( textureFormat.RedBits == 16 && textureFormat.GreenBits == 16 && textureFormat.BlueBits == 16 && textureFormat.AlphaBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative.
                    if ( textureFormat.RedBits > 16 || textureFormat.GreenBits > 16 || textureFormat.BlueBits > 16 || textureFormat.AlphaBits > 16 )
                    {
                        dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
                    }
                }
                break;
            case TextureType::UnsignedInteger:
                if ( textureFormat.RedBits == 8 && textureFormat.GreenBits == 8 && textureFormat.BlueBits == 8 && textureFormat.AlphaBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8G8B8A8_UINT;
                }
                else if ( textureFormat.RedBits == 10 && textureFormat.GreenBits == 10 && textureFormat.BlueBits == 10 && textureFormat.AlphaBits == 2 )
                {
                    dxgiFormat = DXGI_FORMAT_R10G10B10A2_UINT;
                }
                else if ( textureFormat.RedBits == 16 && textureFormat.GreenBits == 16 && textureFormat.BlueBits == 16 && textureFormat.AlphaBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16G16B16A16_UINT;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative.
                    if ( textureFormat.RedBits > 10 || textureFormat.GreenBits > 10 || textureFormat.BlueBits > 10 || textureFormat.AlphaBits > 10 )
                    {
                        dxgiFormat = DXGI_FORMAT_R16G16B16A16_UINT;
                    }
                    else if ( ( textureFormat.RedBits > 8 || textureFormat.GreenBits > 8 || textureFormat.BlueBits > 8 ) && textureFormat.AlphaBits <= 2 )
                    {
                        dxgiFormat = DXGI_FORMAT_R10G10B10A2_UINT;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R8G8B8A8_UINT;
                    }
                }
                break;
            case TextureType::SignedInteger:
                if ( textureFormat.RedBits == 8 && textureFormat.GreenBits == 8 && textureFormat.BlueBits == 8 && textureFormat.AlphaBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8G8B8A8_SINT;
                }
                else if ( textureFormat.RedBits == 16 && textureFormat.GreenBits == 16 && textureFormat.BlueBits == 16 && textureFormat.AlphaBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_R16G16B16A16_SINT;
                }
                else if ( textureFormat.RedBits == 32 && textureFormat.GreenBits == 32 && textureFormat.BlueBits == 32 && textureFormat.AlphaBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_R32G32B32A32_SINT;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Try to choose a reasonable alternative.
                    if ( textureFormat.RedBits > 16 || textureFormat.GreenBits > 16 || textureFormat.BlueBits > 16 || textureFormat.AlphaBits > 16 )
                    {
                        dxgiFormat = DXGI_FORMAT_R32G32B32A32_SINT;
                    }
                    if ( textureFormat.RedBits > 8 || textureFormat.GreenBits > 8 || textureFormat.BlueBits > 8 || textureFormat.AlphaBits > 8 )
                    {
                        dxgiFormat = DXGI_FORMAT_R16G16B16A16_SINT;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_R8G8B8A8_SINT;
                    }
                }
                break;
            case TextureType::sRGB:
                if ( textureFormat.RedBits == 8 && textureFormat.GreenBits == 8 && textureFormat.BlueBits == 8 && textureFormat.AlphaBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Only 1 format could match
                    dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
                }
                break;
            default:
                ReportTextureFormatError( textureFormat, "Unknown texture format." );
                break;
            }
            break;
        case TextureComponents::BGR:
            switch ( textureFormat.Type )
            {
            case TextureType::UnsignedNormalized:
                if ( textureFormat.BlueBits == 5 && textureFormat.GreenBits == 6 && textureFormat.RedBits == 5 )
                {
                    dxgiFormat = DXGI_FORMAT_B5G6R5_UNORM;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Only 1 format could match
                    dxgiFormat = DXGI_FORMAT_B5G6R5_UNORM;
                }
                break;
            default:
                ReportTextureFormatError( textureFormat, "Unknown texture format." );
                break;
            }
            break;
        case TextureComponents::BGRA:
            switch ( textureFormat.Type )
            {
            case TextureType::Typeless:
                if ( textureFormat.BlueBits == 8 && textureFormat.GreenBits == 8 && textureFormat.RedBits == 8 && textureFormat.AlphaBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_B8G8R8A8_TYPELESS;
                }
                else if ( textureFormat.BlueBits == 8 && textureFormat.GreenBits == 8 && textureFormat.RedBits == 8 && textureFormat.AlphaBits == 0 )
                {
                    dxgiFormat = DXGI_FORMAT_B8G8R8X8_TYPELESS;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    if ( textureFormat.AlphaBits > 0 )
                    {
                        dxgiFormat = DXGI_FORMAT_B8G8R8A8_TYPELESS;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_B8G8R8X8_TYPELESS;
                    }
                }
                break;
            case TextureType::UnsignedNormalized:
                if ( textureFormat.BlueBits == 5 && textureFormat.GreenBits == 5 && textureFormat.RedBits == 5 && textureFormat.AlphaBits == 1 )
                {
                    dxgiFormat = DXGI_FORMAT_B5G5R5A1_UNORM;
                }
                else if ( textureFormat.BlueBits == 8 && textureFormat.GreenBits == 8 && textureFormat.RedBits == 8 && textureFormat.AlphaBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
                }
                else if ( textureFormat.BlueBits == 8 && textureFormat.GreenBits == 8 && textureFormat.RedBits == 8 && textureFormat.AlphaBits == 0 )
                {
                    dxgiFormat = DXGI_FORMAT_B8G8R8X8_UNORM;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    if ( textureFormat.BlueBits > 5 || textureFormat.GreenBits > 5 || textureFormat.RedBits > 5 )
                    {
                        if ( textureFormat.AlphaBits > 1 )
                        {
                            dxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
                        }
                        else
                        {
                            dxgiFormat = DXGI_FORMAT_B8G8R8X8_UNORM;
                        }
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_B5G5R5A1_UNORM;
                    }
                }
                break;
            case TextureType::sRGB:
                if ( textureFormat.BlueBits == 8 && textureFormat.GreenBits == 8 && textureFormat.RedBits == 8 && textureFormat.AlphaBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
                }
                else if ( textureFormat.BlueBits == 8 && textureFormat.GreenBits == 8 && textureFormat.RedBits == 8 && textureFormat.AlphaBits == 0 )
                {
                    dxgiFormat = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    if ( textureFormat.AlphaBits > 0 )
                    {
                        dxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
                    }
                    else
                    {
                        dxgiFormat = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
                    }
                }
                break;
            default:
                ReportTextureFormatError( textureFormat, "Unknown texture format." );
                break;
            }
            break;
        case TextureComponents::A:
            switch ( textureFormat.Type )
            {
            case TextureType::UnsignedNormalized:
                if ( textureFormat.AlphaBits == 8 )
                {
                    dxgiFormat = DXGI_FORMAT_A8_UNORM;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // There is only 1 format that could match
                    dxgiFormat = DXGI_FORMAT_A8_UNORM;
                }
                break;
            default:
                ReportTextureFormatError( textureFormat, "Unknown texture format." );
                break;
            }
            break;
        case TextureComponents::Depth:
            switch ( textureFormat.Type )
            {
            case TextureType::UnsignedNormalized:
                if ( textureFormat.DepthBits == 16 )
                {
                    dxgiFormat = DXGI_FORMAT_D16_UNORM;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Only 1 format that could match.
                    dxgiFormat = DXGI_FORMAT_D16_UNORM;
                }
                break;
            case TextureType::Float:
                if ( textureFormat.DepthBits == 32 )
                {
                    dxgiFormat = DXGI_FORMAT_D32_FLOAT;
                }
                else
                {
                    ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                    // Only 1 format that could match.
                    dxgiFormat = DXGI_FORMAT_D32_FLOAT;
                }
                break;
            default:
                // There are no SNORM, SINT, or UINT depth-only formats.
                ReportTextureFormatError( textureFormat, "Unknown texture format." );
                break;
            }
            break;
        case TextureComponents::DepthStencil:
            // For Depth/Stencil formats, we'll ignore the type and try to deduce the format
            // based on the bit-depth values.
            if ( textureFormat.DepthBits == 24 && textureFormat.StencilBits == 8 )
            {
                dxgiFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
            }
            else if ( textureFormat.DepthBits == 32 && textureFormat.StencilBits == 8 )
            {
                dxgiFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
            }
            else
            {
                ReportTextureFormatError( textureFormat, "Unsupported texture format." );
                // Try to choose a reasonable alternative...
                if ( textureFormat.DepthBits > 24 )
                {
                    dxgiFormat = dxgiFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
                }
                else
                {
                    dxgiFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
                }
            }
            break;
        case TextureComponents::BC1:
            switch ( textureFormat.Type )
            {
            case TextureType::Typeless:
                dxgiFormat = DXGI_FORMAT_BC1_TYPELESS;
                break;
            case TextureType::UnsignedNormalized:
                dxgiFormat = DXGI_FORMAT_BC1_UNORM;
                break;
            case TextureType::sRGB:
                dxgiFormat = DXGI_FORMAT_BC1_UNORM_SRGB;
                break;
            default:
                ReportTextureFormatError( textureFormat, "Unknown texture format." );
                break;
            }
            break;
        case TextureComponents::BC2:
            switch ( textureFormat.Type )
            {
            case TextureType::Typeless:
                dxgiFormat = DXGI_FORMAT_BC2_TYPELESS;
                break;
            case TextureType::UnsignedNormalized:
                dxgiFormat = DXGI_FORMAT_BC2_UNORM;
                break;
            case TextureType::sRGB:
                dxgiFormat = DXGI_FORMAT_BC2_UNORM_SRGB;
                break;
            default:
                ReportTextureFormatError( textureFormat, "Unknown texture format." );
                break;
            }
            break;
        case TextureComponents::BC3:
            switch ( textureFormat.Type )
            {
            case TextureType::Typeless:
                dxgiFormat = DXGI_FORMAT_BC3_TYPELESS;
                break;
            case TextureType::UnsignedNormalized:
                dxgiFormat = DXGI_FORMAT_BC3_UNORM;
                break;
            case TextureType::sRGB:
                dxgiFormat = DXGI_FORMAT_BC3_UNORM_SRGB;
                break;
            default:
                ReportTextureFormatError( textureFormat, "Unknown texture format." );
                break;
            }
            break;
        case TextureComponents::BC4:
            switch ( textureFormat.Type )
            {
            case TextureType::Typeless:
                dxgiFormat = DXGI_FORMAT_BC4_TYPELESS;
                break;
            case TextureType::UnsignedNormalized:
                dxgiFormat = DXGI_FORMAT_BC4_UNORM;
                break;
            case TextureType::SignedNormalized:
                dxgiFormat = DXGI_FORMAT_BC4_SNORM;
                break;
            default:
                ReportTextureFormatError( textureFormat, "Unknown texture format." );
                break;
            }
            break;
        case TextureComponents::BC5:
            switch ( textureFormat.Type )
            {
            case TextureType::Typeless:
                dxgiFormat = DXGI_FORMAT_BC5_TYPELESS;
                break;
            case TextureType::UnsignedNormalized:
                dxgiFormat = DXGI_FORMAT_BC5_UNORM;
                break;
            case TextureType::SignedNormalized:
                dxgiFormat = DXGI_FORMAT_BC5_SNORM;
                break;
            default:
                ReportTextureFormatError( textureFormat, "Unknown texture format." );
                break;
            }
            break;
        case TextureComponents::BC6U:
            switch ( textureFormat.Type )
            {
            case TextureType::Typeless:
                dxgiFormat = DXGI_FORMAT_BC6H_TYPELESS;
                break;
            case TextureType::Float:
                dxgiFormat = DXGI_FORMAT_BC6H_UF16;
                break;
            default:
                ReportTextureFormatError( textureFormat, "Unknown texture format." );
                break;
            }
            break;
        case TextureComponents::BC6S:
            switch ( textureFormat.Type )
            {
            case TextureType::Typeless:
                dxgiFormat = DXGI_FORMAT_BC6H_TYPELESS;
                break;
            case TextureType::Float:
                dxgiFormat = DXGI_FORMAT_BC6H_SF16;
                break;
            default:
                ReportTextureFormatError( textureFormat, "Unknown texture format." );
                break;
            }
            break;
        case TextureComponents::BC7:
            switch ( textureFormat.Type )
            {
            case TextureType::Typeless:
                dxgiFormat = DXGI_FORMAT_BC7_TYPELESS;
                break;
            case TextureType::UnsignedNormalized:
                dxgiFormat = DXGI_FORMAT_BC7_UNORM;
                break;
            case TextureType::sRGB:
                dxgiFormat = DXGI_FORMAT_BC7_UNORM_SRGB;
                break;
            default:
                ReportTextureFormatError( textureFormat, "Unknown texture format." );
                break;
            }
            break;
        default:
            ReportTextureFormatError( textureFormat, "Unknown texture format." );
            break;
        }

        return dxgiFormat;
    }

    //--------------------------------------------------------------------------------------
    // Return the BPP for a particular format
    //--------------------------------------------------------------------------------------
    size_t BitsPerPixel( _In_ DXGI_FORMAT fmt )
    {
        switch ( fmt )
        {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
            return 128;

        case DXGI_FORMAT_R32G32B32_TYPELESS:
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32B32_UINT:
        case DXGI_FORMAT_R32G32B32_SINT:
            return 96;

        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R32G32_TYPELESS:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        case DXGI_FORMAT_Y416:
        case DXGI_FORMAT_Y210:
        case DXGI_FORMAT_Y216:
            return 64;

        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R16G16_TYPELESS:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        case DXGI_FORMAT_AYUV:
        case DXGI_FORMAT_Y410:
        case DXGI_FORMAT_YUY2:
            return 32;

        case DXGI_FORMAT_P010:
        case DXGI_FORMAT_P016:
            return 24;

        case DXGI_FORMAT_R8G8_TYPELESS:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_SINT:
        case DXGI_FORMAT_B5G6R5_UNORM:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
        case DXGI_FORMAT_A8P8:
        case DXGI_FORMAT_B4G4R4A4_UNORM:
            return 16;

        case DXGI_FORMAT_NV12:
        case DXGI_FORMAT_420_OPAQUE:
        case DXGI_FORMAT_NV11:
            return 12;

        case DXGI_FORMAT_R8_TYPELESS:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
        case DXGI_FORMAT_A8_UNORM:
        case DXGI_FORMAT_AI44:
        case DXGI_FORMAT_IA44:
        case DXGI_FORMAT_P8:
            return 8;

        case DXGI_FORMAT_R1_UNORM:
            return 1;

        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
            return 4;

        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            return 8;

        default:
            return 0;
        }
    }

    //--------------------------------------------------------------------------------------
    // Get surface information for a particular format
    // From DDSTextureLoader.cpp
    //--------------------------------------------------------------------------------------
    void GetSurfaceInfo( _In_ size_t width,
                         _In_ size_t height,
                         _In_ DXGI_FORMAT fmt,
                         _Out_opt_ size_t* outNumBytes,
                         _Out_opt_ size_t* outRowBytes,
                         _Out_opt_ size_t* outNumRows )
    {
        size_t numBytes = 0;
        size_t rowBytes = 0;
        size_t numRows = 0;

        bool bc = false;
        bool packed = false;
        bool planar = false;
        size_t bpe = 0;
        switch ( fmt )
        {
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
            bc = true;
            bpe = 8;
            break;

        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            bc = true;
            bpe = 16;
            break;

        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
        case DXGI_FORMAT_YUY2:
            packed = true;
            bpe = 4;
            break;

        case DXGI_FORMAT_Y210:
        case DXGI_FORMAT_Y216:
            packed = true;
            bpe = 8;
            break;

        case DXGI_FORMAT_NV12:
        case DXGI_FORMAT_420_OPAQUE:
            planar = true;
            bpe = 2;
            break;

        case DXGI_FORMAT_P010:
        case DXGI_FORMAT_P016:
            planar = true;
            bpe = 4;
            break;

        }

        if ( bc )
        {
            size_t numBlocksWide = 0;
            if ( width > 0 )
            {
                numBlocksWide = std::max<size_t>( 1, ( width + 3 ) / 4 );
            }
            size_t numBlocksHigh = 0;
            if ( height > 0 )
            {
                numBlocksHigh = std::max<size_t>( 1, ( height + 3 ) / 4 );
            }
            rowBytes = numBlocksWide * bpe;
            numRows = numBlocksHigh;
            numBytes = rowBytes * numBlocksHigh;
        }
        else if ( packed )
        {
            rowBytes = ( ( width + 1 ) >> 1 ) * bpe;
            numRows = height;
            numBytes = rowBytes * height;
        }
        else if ( fmt == DXGI_FORMAT_NV11 )
        {
            rowBytes = ( ( width + 3 ) >> 2 ) * 4;
            numRows = height * 2; // Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
            numBytes = rowBytes * numRows;
        }
        else if ( planar )
        {
            rowBytes = ( ( width + 1 ) >> 1 ) * bpe;
            numBytes = ( rowBytes * height ) + ( ( rowBytes * height + 1 ) >> 1 );
            numRows = height + ( ( height + 1 ) >> 1 );
        }
        else
        {
            size_t bpp = BitsPerPixel( fmt );
            rowBytes = ( width * bpp + 7 ) / 8; // round up to nearest byte
            numRows = height;
            numBytes = rowBytes * height;
        }

        if ( outNumBytes )
        {
            *outNumBytes = numBytes;
        }
        if ( outRowBytes )
        {
            *outRowBytes = rowBytes;
        }
        if ( outNumRows )
        {
            *outNumRows = numRows;
        }
    }

    DXGI_FORMAT GetTextureFormat( DXGI_FORMAT format )
    {
        DXGI_FORMAT result = format;

        switch ( format )
        {
        case DXGI_FORMAT_D16_UNORM:
            result = DXGI_FORMAT_R16_TYPELESS;
            break;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            result = DXGI_FORMAT_R24G8_TYPELESS;
            break;
        case DXGI_FORMAT_D32_FLOAT:
            result = DXGI_FORMAT_R32_TYPELESS;
            break;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            result = DXGI_FORMAT_R32G8X24_TYPELESS;
            break;
        }

        return result;
    }

    DXGI_FORMAT GetDepthStencilViewFormat( DXGI_FORMAT format )
    {
        DXGI_FORMAT result = format;

        switch ( format )
        {
        case DXGI_FORMAT_R16_TYPELESS:
            result = DXGI_FORMAT_D16_UNORM;
            break;
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
            result = DXGI_FORMAT_D24_UNORM_S8_UINT;
            break;
        case DXGI_FORMAT_R32_TYPELESS:
            result = DXGI_FORMAT_D32_FLOAT;
            break;
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            result = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
            break;
        }

        return result;
    }

    DXGI_FORMAT GetShaderResourceViewFormat( DXGI_FORMAT format )
    {
        DXGI_FORMAT result = format;
        switch ( format )
        {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
            result = DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
        case DXGI_FORMAT_R32G32B32_TYPELESS:
            result = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
            result = DXGI_FORMAT_R16G16B16A16_UNORM;
            break;
        case DXGI_FORMAT_R32G32_TYPELESS:
            result = DXGI_FORMAT_R32G32_FLOAT;
            break;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
            result = DXGI_FORMAT_R10G10B10A2_UNORM;
            break;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            result = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case DXGI_FORMAT_R16G16_TYPELESS:
            result = DXGI_FORMAT_R16G16_UNORM;
            break;
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_D16_UNORM:
            result = DXGI_FORMAT_R16_UNORM;
            break;
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            result = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            break;
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
            result = DXGI_FORMAT_R32_FLOAT;
            break;
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            result = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
            break;
        case DXGI_FORMAT_R8G8_TYPELESS:
            result = DXGI_FORMAT_R8G8_UNORM;
            break;
        case DXGI_FORMAT_R8_TYPELESS:
            result = DXGI_FORMAT_R8_UNORM;
            break;
        }

        return result;
    }

    DXGI_FORMAT GetRenderTargetViewFormat( DXGI_FORMAT format )
    {
        DXGI_FORMAT result = format;

        switch ( format )
        {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
            result = DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
        case DXGI_FORMAT_R32G32B32_TYPELESS:
            result = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
            result = DXGI_FORMAT_R16G16B16A16_UNORM;
            break;
        case DXGI_FORMAT_R32G32_TYPELESS:
            result = DXGI_FORMAT_R32G32_FLOAT;
            break;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
            result = DXGI_FORMAT_R10G10B10A2_UNORM;
            break;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            result = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case DXGI_FORMAT_R16G16_TYPELESS:
            result = DXGI_FORMAT_R16G16_UNORM;
            break;
        case DXGI_FORMAT_R8G8_TYPELESS:
            result = DXGI_FORMAT_R8G8_UNORM;
            break;
        case DXGI_FORMAT_R32_TYPELESS:
            result = DXGI_FORMAT_R32_FLOAT;
            break;
        case DXGI_FORMAT_R8_TYPELESS:
            result = DXGI_FORMAT_R8_UNORM;
            break;
        }

        return result;
    }

    DXGI_FORMAT GetUnorderedAccessViewFormat( DXGI_FORMAT format )
    {
        return GetRenderTargetViewFormat( format );
    }
}
