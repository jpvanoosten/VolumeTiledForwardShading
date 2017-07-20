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
 *  @file RenderTargetDX12.h
 *  @date February 18, 2016
 *  @author jeremiah
 *
 *  @brief DX12 Render target implementation.
 */

#include "../RenderTarget.h"
#include "../../Object.h"

namespace Graphics
{
    class DeviceDX12;
    class TextureDX12;
    class GraphicsCommandBufferDX12;

    class RenderTargetDX12 : public Core::Object, public virtual RenderTarget
    {
    public:
        RenderTargetDX12( std::shared_ptr<DeviceDX12> device );
        virtual ~RenderTargetDX12();

        virtual void AttachTexture( AttachmentPoint attachment, std::shared_ptr<Texture> texture ) override;
        virtual std::shared_ptr<Texture> GetTexture( AttachmentPoint attachment ) const override;

        virtual void Resize( uint16_t width, uint16_t height ) override;

        DXGI_SAMPLE_DESC GetSampleDesc() const;
        std::vector<DXGI_FORMAT> GetRTVFormats() const;
        DXGI_FORMAT GetDSVFormat() const;

        void Bind( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer );

        const std::vector< std::shared_ptr<TextureDX12> >& GetTextures() const;

    protected:

    private:
        std::weak_ptr<DeviceDX12> m_Device;
        
        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;

        using TextureList = std::vector< std::shared_ptr<TextureDX12> >;
        TextureList m_Textures;
    };
}