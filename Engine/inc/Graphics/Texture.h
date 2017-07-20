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
 *  @file Texture.h
 *  @date January 26, 2016
 *  @author Jeremiah
 *
 *  @brief A texture can be a 1D, 2D, 3D, or array variant of on of those types.
 */

#include "Resource.h"
#include "TextureFormat.h"

namespace Graphics
{
    enum class TextureDimension
    {
        Unknonwn,
        Texture1D,
        Texture1DArray,
        Texture2D,
        Texture2DArray,
        Texture3D,
        TextureCube,
        TextureCubeArray,
    };

    class ENGINE_DLL Texture : virtual public Resource
    {
    public:

        /**
         * Return the type of the texture.
         */
        virtual TextureDimension GetTextureDimension() = 0;

        /**
         * Return the internal texture format.
         */
        virtual TextureFormat GetTextureFormat() = 0;

        /**
         * Load a 2D texture from a file path.
         */
        virtual bool LoadTexture2D( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, const std::wstring& fileName ) = 0;

        /**
         * Get the filename that was used to load this texture.
         * If this texture was not loaded from a file, this function will return an empty string.
         */
        virtual const std::wstring& GetFileName() const = 0;

        /**
         * Get the width of the texture in texels.
         */
        virtual uint32_t GetWidth() const = 0;
        
        /**
         * Get the height of the texture in texels.
         */
        virtual uint32_t GetHeight() const = 0;

        /**
         * Get the depth of the texture in texture slices for 3D textures
         * or the number of array slices for 1D, 2D, and cube textures.
         * For Cubemap textures, this should always be 6.
         */
        virtual uint32_t GetDepthOrArraySize() const = 0;

        /**
         * Gets the bits-per-pixel of the texture.
         */
        virtual uint8_t GetBPP() const = 0;

        /**
         * Get the size in bytes of a row of pixels.
         */
        virtual uint64_t GetPitch() const = 0;

        /**
         * Get the number of mipmap levels this texture contains.
         */
        virtual uint8_t GetMipLevels() const = 0;

        /**
         * Does this texture have an alpha channel?
         */
        virtual bool IsTransparent() const = 0;

        /**
         * Resize the texture to the new dimensions.
         * Resizing a texture will cause the current texture to be discarded.
         * This should only be done on dynamic textures (for example, textures
         * used as render targets).
         * @param width The width of the texture for 1D, 2D, or 3D textures or
         * size of a cubemap face for cubemap textures.
         * @param height The height of the texture for 2D, or 3D textures.
         * @param depthOrArraySize For 3D textures, the depth of the texture. 
         * For 1D and 2D textures, the number of array slices.
         * @param mipLevels The number of mip levels to allocate for the texture.
         */
        virtual void Resize( uint32_t width, uint32_t height = 1, uint32_t depthOrArraySize = 1, uint8_t mipLevels = 1 ) = 0;

    protected:

    private:
    };
}