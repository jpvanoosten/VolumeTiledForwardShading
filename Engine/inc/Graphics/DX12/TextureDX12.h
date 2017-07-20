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
 *  @file TextureDX11.h
 *  @date January 26, 2016
 *  @author Jeremiah
 *
 *  @brief DX12 texture implementation.
 */

#include "ResourceDX12.h"
#include "../Texture.h"

namespace Graphics
{
    class DeviceDX12;
    class ResourceDX12;

    class TextureDX12 : public ResourceDX12, public virtual Texture, public std::enable_shared_from_this<TextureDX12>
    {
    public:
        // Create an empty texture.
        TextureDX12( std::shared_ptr<DeviceDX12> device );
        // Create a texture from an existing texture resource.
        TextureDX12( std::shared_ptr<DeviceDX12> device, Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource, TextureDimension textureDimension );

        // Create a 1D texture.
        TextureDX12( std::shared_ptr<DeviceDX12> device, uint16_t width, uint16_t slices, const TextureFormat& format );
        // Create a 2D texture.
        TextureDX12( std::shared_ptr<DeviceDX12> device, uint16_t width, uint16_t height, uint16_t slices, const TextureFormat& format );

        // TODO: 3D textures
        // TOOD: Cubemap textures

        /**
        * Return the type of the texture.
        */
        virtual TextureDimension GetTextureDimension() override;

        /**
         * Return the internal texture format.
         */
        virtual TextureFormat GetTextureFormat() override;


        /**
        * Load a 2D texture from a file path.
        */
        virtual bool LoadTexture2D( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, const std::wstring& fileName ) override;

        /**
        * Get the filename that was used to load this texture.
        * If this texture was not loaded from a file, this function will return an empty string.
        */
        virtual const std::wstring& GetFileName() const override;

        /**
        * Get the width of the texture in texels.
        */
        virtual uint32_t GetWidth() const override;

        /**
        * Get the height of the texture in texels.
        */
        virtual uint32_t GetHeight() const override;

        /**
        * Get the depth of the texture in texture slices for 3D textures
        * or the number of cube faces for cubemap textures.
        */
        virtual uint32_t GetDepthOrArraySize() const override;

        /**
        * Gets the bits-per-pixel of the texture.
        */
        virtual uint8_t GetBPP() const override;

        /**
        * Get the size in bytes of a row of pixels.
        */
        virtual uint64_t GetPitch() const override;

        /**
         * Get the number of mipmap levels this texture contains.
         */
        virtual uint8_t GetMipLevels() const override;

        /**
        * Does this texture have an alpha channel?
        */
        virtual bool IsTransparent() const override;

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
        virtual void Resize( uint32_t width, uint32_t height = 1, uint32_t depthOrArraySize = 1, uint8_t mipLevels = 1 ) override;

        DXGI_SAMPLE_DESC GetSampleDesc() const;
        DXGI_FORMAT GetResourceFormat() const;
        DXGI_FORMAT GetSRVFormat() const;
        DXGI_FORMAT GetRTVFormat() const;
        DXGI_FORMAT GetDSVFormat() const;

        D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;
        D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

        virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t index = 0 ) override;
        virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer, uint32_t index = 0 ) override;

    protected:

    private:
        // Initialize texture formats
        void InitFormats( const TextureFormat& textureFormat );
        // Initialize texture views.
        void InitViews(const D3D12_RESOURCE_DESC& d3d12ResourceDesc );

        uint32_t m_Width;
        uint32_t m_Height;
        uint32_t m_DepthOrArraySize;
        uint64_t m_Pitch;
        uint8_t m_BPP;
        // Since max texture size is 16,384, the maximum 
        // number of mip levels is 14.
        uint8_t m_MipLevels;

        bool m_IsTransparent;

        TextureFormat m_TextureFormat;
        TextureDimension m_TextureDimension;

        // Sample description
        DXGI_SAMPLE_DESC m_SampleDesc;

        // DXGI formats for various views of the texture.
        DXGI_FORMAT m_ResourceFormat;
        DXGI_FORMAT m_DSVFormat;
        DXGI_FORMAT m_SRVFormat;
        DXGI_FORMAT m_RTVFormat;

        // Used for shader resource and UAV views.
        CD3DX12_CPU_DESCRIPTOR_HANDLE m_ShaderResourceView;
        // Used for render target views.
        CD3DX12_CPU_DESCRIPTOR_HANDLE  m_RenderTargetView;
        // Used for depth/stencil views.
        CD3DX12_CPU_DESCRIPTOR_HANDLE  m_DepthStencilView;
        // Used for the UAV views for mip maps.
        // Max texture dimension is 16,384 so maximum 
        // number of mipmap levels is 15 (0-14).
        CD3DX12_CPU_DESCRIPTOR_HANDLE m_UnorderedAccessView;
        UINT m_DescriptorIncrementSize;

        D3D12_FEATURE_DATA_FORMAT_SUPPORT m_ResourceFormatSupport;
        D3D12_FEATURE_DATA_FORMAT_SUPPORT m_DSVFormatSupport;
        D3D12_FEATURE_DATA_FORMAT_SUPPORT m_SRVFormatSupport;
        D3D12_FEATURE_DATA_FORMAT_SUPPORT m_RTVFormatSupport;

        // The file path to the texture if the texture was loaded from disc.
        std::wstring m_TextureFileName;

    };
}
