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
 *  @file Shader.h
 *  @date February 16, 2016
 *  @author Jeremiah
 *
 *  @brief Shader.
 */

#include "../EngineDefines.h"
#include "../Events.h"
#include "GraphicsEnums.h"


namespace Graphics
{
    class ShaderSignature;

    /**
     * A shader macro consists of a macro name and a definition.
     * Use this to pass macro definitions to the shader compiler.
     */
    using ShaderMacros = std::map< std::string, std::string >;

    class ENGINE_DLL Shader
    {
    public:
        
        /**
         * The type of shader this is.
         */
        virtual ShaderType GetType() const = 0;

        /**
         * Load a shader file from a string.
         * @param type: The type of shader to load.
         * @param source: The Shader source code in string format.
         * @param sourceFileName: The file path of the original file if it exists. This is used to determine include paths.
         * @param entryPoint: The name of the entry-point function to be used by this shader.
         * @param profile: The shader profile to use to compile this shader.
         * To use the latest supported profile, specify "latest" here.
         * @return True if the shader was loaded correctly, or False otherwise.
         */
        virtual bool LoadShaderFromString( ShaderType type, const std::string& source, const std::wstring& sourceFileName = L"", const std::string& entryPoint = "main", const ShaderMacros& shaderMacros = ShaderMacros(), const std::string& profile = "latest" ) = 0;

        /**
         * Load a shader from a file.
         * @param type: The type of shader to load.
         * @param fileName: The path to the shader file to load.
         * @param entryPoint: The name of the entry-point function to be used by this shader.
         * @param profile: The shader profile to use to compile this shader.
         * To use the latest supported profile, specify "latest" here.
         * @return True if the shader was loaded correctly, or False otherwise.
         */
        virtual bool LoadShaderFromFile( ShaderType type, const std::wstring& fileName, const std::string& entryPoint = "main", const ShaderMacros& shaderMacros = ShaderMacros(), const std::string& profile = "latest" ) = 0;

        /**
         * Query for the latest supported shader profile.
         * @param type: The type of shader to query.
         * @return The supported shader profile or an empty string if no profile could be
         * determined for the specified shader type.
         */
        virtual std::string GetLatestProfile( ShaderType type ) = 0;

        /**
        * Shaders can define a shader signature in the shader file.
        * Retrieve the shader signature that was defined in the shader code.
        * @return The shader signature that is defined in the shader code.
        * If no shader signature is defined in the shader source code, this function
        * returns an empty (null) pointer.
        */
        virtual std::shared_ptr<ShaderSignature> GetShaderSignature() = 0;

        // This event is fired if the shader is modified on disc.
        Core::FileChangeEvent FileChanged;

    protected:
        virtual void OnFileChanged( Core::FileChangeEventArgs& e );

    };
}
