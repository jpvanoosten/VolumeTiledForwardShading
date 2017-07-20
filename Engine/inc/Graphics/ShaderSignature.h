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
 *  @file ShaderSignature.h
 *  @date March 16, 2016
 *  @author jeremiah
 *
 *  @brief Define a shader signature for a pipeline state object.
 *  A shader signature defines the parameters that are bound to the pipeline.
 */

#include "../EngineDefines.h"
#include "Shader.h" // For ShaderType

namespace Graphics
{
    class ShaderParameter;
    class Sampler;

    class ENGINE_DLL ShaderSignature
    {
    public:
        /**
         * The list of parameters stored in a shader signature.
         */
        using ParameterList = std::vector<ShaderParameter>;

        virtual const ParameterList& GetParameters() const = 0;
        virtual void SetParameters( const ParameterList& parameterList ) = 0;

        virtual const ShaderParameter& GetParameter( unsigned int index ) const = 0;
        virtual void SetParameter( unsigned int index, const ShaderParameter& parameter ) = 0;

        /**
         * Return the number of parameters in the shader signature.
         */
        virtual uint32_t GetNumParameters() const = 0;

        /**
         * Allow assignment.
         */
        virtual ShaderSignature& operator=( const ShaderSignature& rhs ) = 0;

        /**
         * Get or set a shader parameter at a particular slot in the shader signature.
         */
        virtual ShaderParameter& operator[]( unsigned int index ) = 0;
        virtual const ShaderParameter& operator[]( unsigned int index ) const = 0;
        
        /**
         * Set a static sampler defined in the shader signature.
         * @param slotID The slot in the shader signature to assign the static sampler.
         * @param sampler The texture sampler to assign.
         * @param shaderRegister The register location in the shader to assign this sampler.
         * @param registerSpace The register space to assign the sampler. Default is register space 0.
         * @param shaderVisibility Which stages of the rendering pipeline to bind this sampler to. Default it to bind it to every shader stage.
         */
        virtual void SetStaticSampler( uint32_t slotID, std::shared_ptr<Sampler> sampler, uint32_t shaderRegister, ShaderType shaderVisibility = ShaderType::All, uint32_t registerSpace = 0 ) = 0;
        virtual std::shared_ptr<Sampler> GetStaticSampler( uint32_t slotID ) const = 0;

        /**
         * Return the number of static samplers in the root signature.
         */
        virtual uint32_t GetNumStaticSamplers() const = 0;
    };
}