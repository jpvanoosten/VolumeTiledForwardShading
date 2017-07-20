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
 *  @file GraphicsPipelineStateDX12.h
 *  @date February 17, 2016
 *  @author jeremiah
 *
 *  @brief DX12 Pipeline state object.
 */

#include "../GraphicsPipelineState.h"
#include "RasterizerStateDX12.h"
#include "DepthStencilStateDX12.h"
#include "BlendStateDX12.h"
#include "../../Events.h"

namespace Graphics
{
    class DeviceDX12;
    class RenderTargetDX12;
    class ShaderSignatureDX12;

    class GraphicsPipelineStateDX12 : public GraphicsPipelineState, public std::enable_shared_from_this<GraphicsPipelineStateDX12>
    {
    public:
        GraphicsPipelineStateDX12( std::shared_ptr<DeviceDX12> device );
        virtual ~GraphicsPipelineStateDX12();

        virtual void SetShader( ShaderType type, std::shared_ptr<Shader> shader ) override;
        virtual std::shared_ptr<Shader> GetShader( ShaderType type ) const override;
        virtual const ShaderMap& GetShaders() const override;

        virtual void SetShaderSignature( std::shared_ptr<ShaderSignature> shaderSignature ) override;
        virtual std::shared_ptr<ShaderSignature> GetShaderSignature() const override;

        virtual void SetRasterizerState( const RasterizerState& rasterizerState ) override;
        virtual RasterizerState& GetRasterizerState() override;

        virtual void SetDepthStencilState( const DepthStencilState& depthStencilState ) override;
        virtual DepthStencilState& GetDepthStencilState() override;

        virtual void SetBlendState( const BlendState& blendState ) override;
        virtual BlendState& GetBlendState() override;

        virtual void SetRenderTarget( std::shared_ptr<RenderTarget> renderTarget ) override;
        virtual std::shared_ptr<RenderTarget> GetRenderTarget() override;

        virtual void SetPrimitiveTopology( PrimitiveTopology primTopology ) override;
        virtual PrimitiveTopology GetPrimitiveTopology() const override;

        virtual void SetPatchControlPoints( uint32_t patchControlPoints ) override;
        virtual uint32_t GetPatchControlPoints() const override;

        virtual void SetPrimitiveRestart( PrimitiveRestart primitiveRestart ) override;
        virtual PrimitiveRestart GetPrimitiveRestart() const override;

        Microsoft::WRL::ComPtr<ID3D12PipelineState> GetD3D12PipelineState() const;

        void Bind( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer );

    protected:

        // Called if any of the shaders attached to this pipeline state object
        // have been modified.
        virtual void OnFileChanged( Core::FileChangeEventArgs& e );
        // Called if the attachments on the render target have changed.
        virtual void OnAttachmentChanged( Core::EventArgs& e );

    private:
       
        std::weak_ptr<DeviceDX12> m_Device;

        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_d3d12PipelineState;

        ShaderMap m_Shaders;
        std::shared_ptr<ShaderSignatureDX12> m_ShaderSignature;
        std::shared_ptr<RenderTargetDX12> m_RenderTarget;

        RasterizerStateDX12 m_RasterizerState;
        DepthStencilStateDX12 m_DepthStencilState;
        BlendStateDX12 m_BlendState;

        PrimitiveTopology m_PrimitiveTopology;
        uint32_t m_PatchControlPoints;
        PrimitiveRestart m_PrimitiveRestart;

        // Flag to indicate that the pipeline state needs to be (re) created.
        std::atomic_bool m_IsDirty;

        Core::Event::ScopedConnections m_Connections;
    };
}