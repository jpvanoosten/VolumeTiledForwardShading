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
 *  @file ComputePipelineStateDX12.h
 *  @date March 30, 2016
 *  @author Jeremiah van Oosten
 *
 *  @brief Compute pipeline state object for DX12.
 */

#pragma once

#include "../ComputePipelineState.h"
#include "../../Events.h"

namespace Graphics
{
    class DeviceDX12;
    class ShaderDX12;
    class ShaderSignatureDX12;
    class GraphicsCommandBufferDX12;


    class ComputePipelineStateDX12 : public ComputePipelineState
    {
    public:
        ComputePipelineStateDX12( std::shared_ptr<DeviceDX12> device );
        virtual ~ComputePipelineStateDX12();

        virtual void SetShaderSignature( std::shared_ptr<ShaderSignature> shaderSignature ) override;
        virtual std::shared_ptr<ShaderSignature> GetShaderSignature() const override;

        virtual void SetShader( std::shared_ptr<Shader> shader ) override;
        virtual std::shared_ptr<Shader> GetShader() const override;

        void Bind( std::shared_ptr<GraphicsCommandBufferDX12> commandBuffer );
        Microsoft::WRL::ComPtr<ID3D12PipelineState> GetD3D12PipelineState() const;

    protected:
        virtual void OnFileChanged( Core::FileChangeEventArgs& e );

    private:
        std::weak_ptr<DeviceDX12> m_Device;

        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_d3d12PipelineState;

        std::shared_ptr<ShaderSignatureDX12> m_ShaderSignature;
        std::shared_ptr<ShaderDX12> m_ComputeShader;

        bool m_IsDirty;

        Core::Event::ScopedConnections m_Connections;
    };
}