#pragma once

/*
 *  Copyright(c) 2015 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

/**
 *  @file TextureFormat.h
 *  @date December 22, 2015
 *  @author Jeremiah
 *
 *  @brief Texture formats.
 */

#include <EngineDefines.h>
#include <cstdint>

namespace Graphics
{
    enum class TextureComponents
    {
        R,              // One red component.
        RG,             // Red, and green components.
        RGB,            // Red, green, and blue components.
        RGBA,           // Red, green, blue, and alpha components.
        BGR,            // Blue, green, red components.
        BGRA,           // Blue, green, red, alpha components.
        A,              // Alpha channel only.
        Depth,          // Depth component.
        DepthStencil,   // Depth and stencil in the same texture.
        BC1,            // Block compression format. Equivalent to DXT1.
        BC2,            // Block compression format. Equivalent to DXT2, DXT3.
        BC3,            // Block compression format. Equivalent to DXT4, DXT5.
        BC4,            // Block compression format. Equivalent to ATI1.
        BC5,            // Block compression format. Equivalent to ATI2.
        BC6U,           // Block compression format with support for HDR (no alpha, unsigned 16-bit float).
        BC6S,           // Block compression format with support for HDR (no alpha, signed 16-bit float).
        BC7,            // Block compression format with support for HDR (with alpha).
    };

    enum class TextureType
    {
        Typeless,           // Typeless formats.
        UnsignedNormalized, // Unsigned normalized (8, 10, or 16-bit unsigned integer values mapped to the range [0..1])
        SignedNormalized,   // Signed normalized (8, or 16-bit signed integer values mapped to the range [-1..1])
        Float,              // Floating point format (16, or 32-bit).
        UnsignedInteger,    // Unsigned integer format (8, 16, or 32-bit unsigned integer formats).
        SignedInteger,      // Signed integer format (8, 16, or 32-bit signed integer formats).
        sRGB,               // Unsigned normalized sRGB integer format.
    };

    struct ENGINE_DLL TextureFormat
    {
        TextureComponents Components;
        TextureType Type;

        // For multi-sample textures, we can specify how many samples we want 
        // to use for this texture. Valid values are usually in the range [1 .. 16]
        // depending on hardware support.
        // A value of 1 will effectively disable multisampling in the texture.
        uint8_t NumSamples;

        // Components should commonly be 8, 16, or 32-bits but some texture formats
        // support 1, 10, 11, 12, or 24-bits per component.
        uint8_t RedBits;
        uint8_t GreenBits;
        uint8_t BlueBits;
        uint8_t AlphaBits;
        uint8_t DepthBits;
        uint8_t StencilBits;

        // By default create a 4-component unsigned normalized texture with 8-bits per component and no multisampling.
        TextureFormat( TextureComponents _components = TextureComponents::RGBA,
                       TextureType _type = TextureType::UnsignedNormalized,
                       uint8_t _numSamples = 1,
                       uint8_t _redBits = 8,
                       uint8_t _greenBits = 8,
                       uint8_t _blueBits = 8,
                       uint8_t _alphaBits = 8,
                       uint8_t _depthBits = 0,
                       uint8_t _stencilBits = 0 )
            : Components( _components )
            , Type( _type )
            , NumSamples( _numSamples )
            , RedBits( _redBits )
            , GreenBits( _greenBits )
            , BlueBits( _blueBits )
            , AlphaBits( _alphaBits )
            , DepthBits( _depthBits )
            , StencilBits( _stencilBits )
        {}

        bool operator==( const TextureFormat& rhs ) const
        {
            return ( Components == rhs.Components &&
                     Type == rhs.Type &&
                     NumSamples == rhs.NumSamples &&
                     RedBits == rhs.RedBits &&
                     GreenBits == rhs.GreenBits &&
                     BlueBits == rhs.BlueBits &&
                     AlphaBits == rhs.AlphaBits &&
                     DepthBits == rhs.DepthBits &&
                     StencilBits == rhs.StencilBits );
        }

        bool operator!=( const TextureFormat& rhs ) const
        {
            return !( *this == rhs );
        }

        // Predefined texture formats
        static TextureFormat R8G8B8A8_UNORM;
        static TextureFormat D24_UNORM_S8_UINT;
        static TextureFormat D32_FLOAT;
    };
}

