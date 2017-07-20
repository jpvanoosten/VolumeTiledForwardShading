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
 *  @file TextureFormatDXGI.h
 *  @date December 22, 2015
 *  @author Jeremiah
 *
 *  @brief This header contains the conversion function for DXGI_FORMAT.
 */
#include "../TextureFormat.h"

#include <dxgiformat.h>
#include <cstdint>
#include <sstream>

namespace Graphics
{
    /**
     * Convert a DXGI_FORMAT and sample count to a TextureFormat.
     */
    const TextureFormat ConvertTextureFormat( DXGI_FORMAT dxgiFormat, uint8_t numSamples = 1 );

    /**
     * Convert a TextureFormat to a DXGI_FORMAT.
     */
    DXGI_FORMAT ConvertTextureFormat( const TextureFormat& textureFormat );

    /**
     * Get the bits per pixel from a given DXGI_FORMAT.
     */
    size_t BitsPerPixel( DXGI_FORMAT format );

    /**
     * Get surface info required for copying texture subresources.
     * From: DDSTextureLoader.cpp
     */
    void GetSurfaceInfo( _In_ size_t width,
                         _In_ size_t height,
                         _In_ DXGI_FORMAT fmt,
                         _Out_opt_ size_t* outNumBytes,
                         _Out_opt_ size_t* outRowBytes,
                         _Out_opt_ size_t* outNumRows );

    /**
     * Get a (typeless) texture format from a depth/stencil format.
     */
    DXGI_FORMAT GetTextureFormat( DXGI_FORMAT format );

    /**
     * Get a depth/stencil view format from a typeless format.
     */
    DXGI_FORMAT GetDepthStencilViewFormat( DXGI_FORMAT format );

    /**
     * Get a shader resource view format from a depth/stencil format.
     */
    DXGI_FORMAT GetShaderResourceViewFormat( DXGI_FORMAT format );

    /**
     * Get a render target view format from a typeless format.
     */
    DXGI_FORMAT GetRenderTargetViewFormat( DXGI_FORMAT format );

    /**
     * Get a UAV format from a typeless format.
     */
    DXGI_FORMAT GetUnorderedAccessViewFormat( DXGI_FORMAT format );
}
