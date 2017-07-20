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
 *  @file PipelineState.h
 *  @date February 17, 2016
 *  @author jeremiah
 *
 *  @brief The PipelineState class encompasses pipeline configuration.
 *  This includes, shaders, blend states, rasterizer states,
 *  and depth/stencil states among other things....
 */

#include "Shader.h"
#include "GraphicsEnums.h"

namespace Graphics
{
    using ShaderMap = std::map< ShaderType, std::shared_ptr<Shader> >;

    class RasterizerState;
    class DepthStencilState;
    class BlendState;
    class RenderTarget;

    class ENGINE_DLL GraphicsPipelineState
    {
    public:
        virtual void SetShader( ShaderType type, std::shared_ptr<Shader> shader ) = 0;
        virtual std::shared_ptr<Shader> GetShader( ShaderType type ) const = 0;
        virtual const ShaderMap& GetShaders() const = 0;

        virtual void SetShaderSignature( std::shared_ptr<ShaderSignature> shaderSignature ) = 0;
        virtual std::shared_ptr<ShaderSignature> GetShaderSignature() const = 0;

        virtual void SetRasterizerState( const RasterizerState& rasterizerState ) = 0;
        virtual RasterizerState& GetRasterizerState() = 0;

        virtual void SetDepthStencilState( const DepthStencilState& depthStencilState ) = 0;
        virtual DepthStencilState& GetDepthStencilState() = 0;

        virtual void SetBlendState( const BlendState& blendState ) = 0;
        virtual BlendState& GetBlendState() = 0;

        virtual void SetRenderTarget( std::shared_ptr<RenderTarget> renderTarget ) = 0;
        virtual std::shared_ptr<RenderTarget> GetRenderTarget() = 0;

        virtual void SetPrimitiveTopology( PrimitiveTopology primTopology ) = 0;
        virtual PrimitiveTopology GetPrimitiveTopology() const = 0;
        
        virtual void SetPatchControlPoints( uint32_t patchControlPoints ) = 0;
        virtual uint32_t GetPatchControlPoints() const = 0;

        virtual void SetPrimitiveRestart( PrimitiveRestart primitiveRestart ) = 0;
        virtual PrimitiveRestart GetPrimitiveRestart() const = 0;
    };
}