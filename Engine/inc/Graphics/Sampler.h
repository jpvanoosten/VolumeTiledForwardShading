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
 *  @file Sampler.h
 *  @date March 16, 2016
 *  @author jeremiah
 *
 *  @brief Define how a texture is sampled in a shader.
 */

#include "../EngineDefines.h"
#include "GraphicsEnums.h"

namespace Graphics
{
    class ENGINE_DLL Sampler
    {
    public:
        virtual void SetFilter( Filter minFilter = Filter::Nearest, Filter magFilter = Filter::Nearest, Filter mipFilter = Filter::Nearest ) = 0;
        virtual void GetFilter( Filter& minFilter, Filter& magFilter, Filter& mipFilter ) const = 0;

        virtual void SetWrapMode( WrapMode u = WrapMode::Repeat, WrapMode v = WrapMode::Repeat, WrapMode w = WrapMode::Repeat ) = 0;
        virtual void GetWrapMode( WrapMode& u, WrapMode& v, WrapMode& w ) const = 0;

        virtual void SetCompareMode( CompareMode compareMode = CompareMode::None ) = 0;
        virtual CompareMode GetCompareMode() const = 0;

        virtual void SetCompareFunction( CompareFunction compareFunc = CompareFunction::Always ) = 0;
        virtual CompareFunction GetCompareFunc() const = 0;

        /**
        * Set the offset from the calculated mipmap level.  For example, if mipmap texture 1 should be sampled and
        * LOD bias is set to 2, then the texture will be sampled at mipmap level 3.
        */
        virtual void SetLODBias( float lodBias ) = 0;
        virtual float GetLODBias() const = 0;

        /**
        * Set the minimum LOD level that will be sampled.  The highest resolution mip map is level 0.
        */
        virtual void SetMinLOD( float minLOD ) = 0;
        virtual float GetMinLOD() const = 0;

        /**
        * Set the maximum LOD level that will be sampled. The LOD level increases as the resolution of the mip-map decreases.
        */
        virtual void SetMaxLOD( float maxLOD ) = 0;
        virtual float GetMaxLOD() const = 0;

        /**
        * Sets the border color to use if the wrap mode is set to Border.
        */
        virtual void SetBorderColor( const glm::vec4& borderColor ) = 0;
        virtual glm::vec4 GetBorderColor() const = 0;

        /**
        * Enable Anisotropic filtering (where supported).
        */
        virtual void EnableAnisotropicFiltering( bool enabled ) = 0;
        virtual bool IsAnisotropicFilteringEnabled() const = 0;

        /**
        * When Anisotropic filtering is enabled, use this value to determine the maximum level
        * of anisotropic filtering to apply.  Valid values are in the range [1, 16].
        * May not be supported on all platforms.
        */
        virtual void SetMaxAnisotropy( uint8_t maxAnisotropy ) = 0;
        virtual uint8_t GetMaxAnisotropy() const = 0;
    };

}