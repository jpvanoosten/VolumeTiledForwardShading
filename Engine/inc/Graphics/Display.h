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
 *  @file GraphicsDisplay.h
 *  @date December 22, 2015
 *  @author Jeremiah
 *
 *  @brief The graphics display represents a logical, virtual or physical display
 *  device connected to a GraphicsAdapter.
 */

#include "TextureFormat.h"
#include "Rect.h"

namespace Graphics
{
    class Adapter;

    /**
     * Display rotation angle.
     */
    enum class DisplayRotation
    {
        Rotate0,            // No rotation
        Rotate90,           // 90 degree rotation.
        Rotate180,          // 180 degree rotation.
        Rotate270,          // 270 degree rotation.
    };

    struct DisplayMode
    {
        uint32_t        Width;
        uint32_t        Height;
        uint32_t        RefreshRate;
        TextureFormat   Format;
        bool            Stereo;
    };

    using DisplayModeList = std::vector<DisplayMode>;

    class ENGINE_DLL Display
    {
    public:

        /**
         * A name that describes the display device.
         */
        virtual const std::wstring& GetName() const = 0;

        /**
         * Get the desktop coordinates of this display device.
         */
        virtual const Rect& GetDesktopCoordinates() const = 0;

        /**
         * Get the display rotation.
         */
        virtual DisplayRotation GetRotation() const = 0;

        /**
         * Get the display modes that match the requested texture format.
         */
        virtual DisplayModeList GetDisplayModes( const TextureFormat& textureFormat ) const = 0;

    };
}

inline std::wstring to_wstring( const Graphics::DisplayRotation& rot )
{
    std::wstring rotationString;
    switch ( rot )
    {
    case Graphics::DisplayRotation::Rotate0:
        rotationString = L"0°";
        break;
    case Graphics::DisplayRotation::Rotate90:
        rotationString = L"90°";
        break;
    case Graphics::DisplayRotation::Rotate180:
        rotationString = L"180°";
        break;
    case Graphics::DisplayRotation::Rotate270:
        rotationString = L"270°";
        break;
    }

    return rotationString;
}
