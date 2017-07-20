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
 *  @file ShaderDX12.h
 *  @date February 16, 2016
 *  @author Jeremiah
 *
 *  @brief DirectX 12 Shader implementation.
 */

#include "../Shader.h"
#include "../../Events.h"
#include "../../DependencyTracker.h"

namespace Graphics
{
    class DeviceDX12;
    class ShaderSignatureDX12;

    class ShaderDX12 : public Shader
    {
    public:
        using base = Shader;

        ShaderDX12( std::shared_ptr<DeviceDX12> device );
        virtual ~ShaderDX12();

        /**
         * The type of shader this is.
         */
        virtual ShaderType GetType() const override;

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
        virtual bool LoadShaderFromString( ShaderType type, const std::string& source, const std::wstring& sourceFileName = L"", const std::string& entryPoint = "main", const ShaderMacros& shaderMacros = ShaderMacros(), const std::string& profile = "latest" ) override;

        /**
         * Load a shader from a file.
         * @param type: The type of shader to load.
         * @param fileName: The path to the shader file to load.
         * @param entryPoint: The name of the entry-point function to be used by this shader.
         * @param profile: The shader profile to use to compile this shader.
         * To use the latest supported profile, specify "latest" here.
         * @return True if the shader was loaded correctly, or False otherwise.
         */
        virtual bool LoadShaderFromFile( ShaderType type, const std::wstring& fileName, const std::string& entryPoint = "main", const ShaderMacros& shaderMacros = ShaderMacros(), const std::string& profile = "latest" ) override;

        /**
         * Query for the latest supported shader profile.
         * @param type: The type of shader to query.
         * @return The supported shader profile or an empty string if no profile could be
         * determined for the specified shader type.
         */
        virtual std::string GetLatestProfile( ShaderType type ) override;

        /**
         * Shaders can define a shader signature in the shader file.
         * Retrieve the shader signature that was defined in the shader code.
         * @returns The shader signature that is defined in the shader code.
         * If no shader signature is defined in the shader source code, this function
         * return an empty (null) pointer.
         */
        virtual std::shared_ptr<ShaderSignature> GetShaderSignature() override;

        Microsoft::WRL::ComPtr<ID3DBlob> GetD3DShaderBlob() const;
        /**
         * If the input layout of the shader is different that the input
         * layout of the vertex data, the default input layout can be 
         * overridden by specifying an input layout.
         */
        void SetInputLayout( const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout );
        const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout() const;

        /**
         * Fire an event to notify the pipeline state object that 
         * the shader has been modified.
         */
        Core::FileChangeEvent FileChanged;

    protected:
       
        virtual void OnFileChanged( Core::FileChangeEventArgs& e );

    private:
        void ClearInputLayout();

        std::weak_ptr<DeviceDX12> m_Device;

        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;
        Microsoft::WRL::ComPtr<ID3DBlob> m_d3dShaderBlob;

        std::shared_ptr<ShaderSignatureDX12> m_ShaderSignature;

        ShaderType m_ShaderType;

        std::vector<D3D12_INPUT_ELEMENT_DESC> m_d3d12InputElements;

        ShaderMacros m_ShaderMacros;
        std::string m_EntryPoint;
        std::string m_Profile;
        std::wstring m_ShaderFile;

        DependencyTracker m_DependencyTracker;
        Core::Event::ScopedConnections m_Connections;
    };
}
