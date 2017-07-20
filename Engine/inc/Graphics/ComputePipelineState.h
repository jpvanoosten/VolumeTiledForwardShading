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
 *  @file ComputePipelineState.h
 *  @date March 30, 2016
 *  @author jeremiah
 *
 *  @brief Compute pipeline state object.
 */

#include "../EngineDefines.h"

namespace Graphics
{
    class Shader;
    class ShaderSignature;

    class ENGINE_DLL ComputePipelineState
    {
    public:

        virtual void SetShaderSignature( std::shared_ptr<ShaderSignature> shaderSignature ) = 0;
        virtual std::shared_ptr<ShaderSignature> GetShaderSignature() const = 0;

        /**
         * Compute pipeline states only support compute shaders so the shader type 
         * does not need to be specified.
         */
        virtual void SetShader( std::shared_ptr<Shader> shader ) = 0;
        virtual std::shared_ptr<Shader> GetShader() const = 0;

    };
}