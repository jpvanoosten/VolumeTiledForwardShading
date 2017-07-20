#include <EnginePCH.h>

#include <Graphics/TextureFormat.h>

// Define some standard texture formats for convenience.
Graphics::TextureFormat Graphics::TextureFormat::R8G8B8A8_UNORM = Graphics::TextureFormat( Graphics::TextureComponents::RGBA, TextureType::UnsignedNormalized, 1, 8, 8, 8, 8, 0, 0 );
Graphics::TextureFormat Graphics::TextureFormat::D24_UNORM_S8_UINT = Graphics::TextureFormat( Graphics::TextureComponents::DepthStencil, TextureType::UnsignedNormalized, 1, 0, 0, 0, 0, 24, 8 );
Graphics::TextureFormat Graphics::TextureFormat::D32_FLOAT = Graphics::TextureFormat( Graphics::TextureComponents::Depth, TextureType::Float, 1, 0, 0, 0, 0, 32, 0 );