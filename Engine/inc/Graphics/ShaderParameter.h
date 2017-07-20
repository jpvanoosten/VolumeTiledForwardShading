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
 *  @file ShaderParameter.h
 *  @date March 16, 2016
 *  @author jeremiah
 *
 *  @brief A shader parameter defines a single entry for a shader signature.
 */

#include "../EngineDefines.h"
#include "GraphicsEnums.h"

namespace Graphics
{
    class ShaderSignature;
    class ConstantBuffer;
    class StructuredBuffer;
    class ByteAddressBuffer;
    class Texture;
    class Sampler;


    class ENGINE_DLL ShaderParameter
    {
    public:
        /**
        * Table parameters have a set of nested parameters.
        * Use this function to access the nested parameters.
        */
        using NestedParameters = std::vector<ShaderParameter>;

        ShaderParameter( ParameterType parameterType = ParameterType::Invalid );
        ShaderParameter( ParameterType parameterType, uint32_t shaderRegister, uint32_t numEntries = 1, ShaderType shaderVisibility = ShaderType::All, uint32_t registerSpace = 0 );
        ShaderParameter( const ShaderParameter& copy );

        ShaderParameter& operator=( const ShaderParameter& rhs );

        /**
         * The shader parameter type.
         */
        ParameterType GetType()  const;
        void SetType( ParameterType parameterType );

        /**
         * Shader visibility determines which stages of the rendering pipeline can access this shader parameter.
         */
        ShaderType GetShaderVisibility() const;
        void SetShaderVisibility( ShaderType shaderStages );

        /**
         * The base shader register that this parameter is bound to.
         * Not valid for table parameters.
         */
        uint32_t GetBaseRegister() const;
        void SetBaseRegister( uint32_t baseRegister );

        /**
         * For table parameters, this is the number of nested parameters.
         * For nested parameters of a table parameter, we can 
         * specify the number of parameters in the table slot (the number of descriptors in a descriptor range).
         * For the last paramter in a table parameter, you can specify UINT_MAX (or std::numeric_limits<uint32>::max) to specify an unbounded range.
         * For 32-bit constants defined in the shader signature, this parameter
         * defines the number of 32-bit constants that can be bound to the shader signature slot.
         */
        uint32_t GetNumEntries() const;
        void SetNumEntries( uint32_t numEntries );

        /**
         * The shader register space that the parameter is bound to.
         * Specifying different shader register spaces allows you 
         * to bind several parameters to the same shader register.
         * Not valid for table parameters.
         */
        uint32_t GetRegisterSpace() const;
        void SetRegisterSpace( uint32_t space );

        /**
         * Index operators for accessing nested shader parameters
         * in a Table parameter.
         * Not valid for non-table parameter types.
         */
        ShaderParameter& operator[]( const unsigned int index );
        const ShaderParameter& operator[]( const unsigned int index ) const;

        const NestedParameters& GetNestedParameters() const;
        void SetNestedParameters( const NestedParameters& nestedParameters );

    private:
        ParameterType m_ParameterType;

        uint32_t m_BaseRegister;
        // Number of shader registers for table ranges or number of 32-bit constants for constant parameters.
        // Number of registers for descriptor ranges (nested parameters of a table parameter).
        uint32_t m_NumEntries;

        ShaderType m_ShaderVisibility;

        // Register space allows you to bind several descriptors to the same registers. As long as they are in different spaces.
        uint32_t m_RegisterSpace;

        // Table parameters can have multiple table ranges.
        // Table ranges are defined as a set of nested parameters.
        NestedParameters m_NestedParameters;
    };
}