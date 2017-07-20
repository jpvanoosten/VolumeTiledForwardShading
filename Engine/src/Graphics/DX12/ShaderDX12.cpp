#include <EnginePCH.h>

#include <Application.h>
#include <Graphics/DX12/ShaderDX12.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/ShaderSignatureDX12.h>
#include <Graphics/ShaderParameter.h>

#include <LogManager.h>

using namespace Core;
using namespace Graphics;
using namespace Microsoft::WRL;

// Determine the DXGI_FORMAT for a shader input parameter.
DXGI_FORMAT GetDXGIFormat( const D3D12_SIGNATURE_PARAMETER_DESC& paramDesc );

class D3DInclude : public ID3DInclude
{
public:
    D3DInclude( const fs::path& shaderPath )
        : m_ShaderPath( shaderPath )
    {
        m_PathStack.push( m_ShaderPath.parent_path() );
    }

    virtual ~D3DInclude()
    {
        m_PathStack.pop();
        assert( m_PathStack.empty() );
    }

    virtual HRESULT Close( LPCVOID pData )
    {
        delete pData; // free( (void*)pData );
        m_PathStack.pop();
        return S_OK;
    }

    virtual HRESULT Open( D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName,
                          LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes )
    {
        // Decompose the file path.
        fs::path currentPath = m_PathStack.top();

        fs::path filePath = currentPath / pFileName;
        fs::path fileName = filePath.filename();
        fs::path parentPath = filePath.parent_path();

        m_PathStack.push( parentPath );
        
        if ( fs::exists( filePath ) )
        {
            std::ifstream ifs( filePath );
            std::string str( ( std::istreambuf_iterator<char>( ifs ) ), std::istreambuf_iterator<char>() );
            ifs.close();

            UINT numBytes = static_cast<UINT>( str.size() );

            char* pData;
            pData = new char[numBytes + 1]; // (char*)malloc( numBytes+1 ); // Add 1 for the null terminator that is appended by strncpy_s.
            strncpy_s( pData, numBytes+1, str.data(), str.size() );

            *ppData = pData;
            *pBytes = numBytes;

            return S_OK;
        }

        return S_FALSE;
    }

private:
    // Keep track of the current path of the currently included file in the 
    // shader compilation process.
    using path_stack = std::stack< fs::path >;
    path_stack m_PathStack;

    // The path to the shader file being compiled.
    // Include paths should be expressed relative to this file.
    fs::path m_ShaderPath;
    // The parent path of the shader being compiled.
    fs::path m_CurrentPath;
};

ShaderDX12::ShaderDX12( std::shared_ptr<DeviceDX12> device )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
    , m_ShaderType( ShaderType::Unknown )
{
    m_Connections.push_back( m_DependencyTracker.FileChanged += boost::bind( &ShaderDX12::OnFileChanged, this, _1 ) );
}

void ShaderDX12::ClearInputLayout()
{
    // Make sure the input layout doesn't leak memory
    for ( auto inputElement : m_d3d12InputElements )
    {
        delete[] inputElement.SemanticName;
    }
    m_d3d12InputElements.clear();
}

ShaderDX12::~ShaderDX12()
{
    ClearInputLayout();
}

ShaderType ShaderDX12::GetType() const
{
    return m_ShaderType;
}

bool ShaderDX12::LoadShaderFromString( ShaderType type, const std::string& source, const std::wstring& _sourceFileName, const std::string& entryPoint, const ShaderMacros& shaderMacros, const std::string& _profile )
{
    ComPtr<ID3DBlob> d3dShaderBlob;
    ComPtr<ID3DBlob> d3dErrorBlob;

    std::string profile = _profile;
    if ( _profile == "latest" )
    {
        profile = GetLatestProfile( type );
        if ( profile.empty() )
        {
            LOG_ERROR( "Could not resolve latest profile." );
            return false;
        }
    }

    std::vector<D3D_SHADER_MACRO> macros;
    for ( const auto& shaderMacro : shaderMacros )
    {
        macros.push_back( { shaderMacro.first.c_str(), shaderMacro.second.c_str() } );
    }
    macros.push_back( { nullptr, nullptr } );

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG) || defined(PROFILE)
    flags |= D3DCOMPILE_DEBUG;
#endif
#if defined(_DEBUG)
    flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    std::string sourceFileName = Core::ConvertString( _sourceFileName );

    D3DInclude d3dInclude( sourceFileName );

    if ( FAILED( D3DCompile( reinterpret_cast<LPCVOID>( source.c_str() ),
                             source.size(),
                             sourceFileName.c_str(),
                             macros.data(),
//                             D3D_COMPILE_STANDARD_FILE_INCLUDE,
                             &d3dInclude,
                             entryPoint.c_str(),
                             profile.c_str(),
                             flags, 0,
                             &d3dShaderBlob,
                             &d3dErrorBlob ) ) )
    {
        if ( d3dErrorBlob )
        {
            Core::LogManager::LogError( reinterpret_cast<char*>( d3dErrorBlob->GetBufferPointer() ) );
        }
        return false;
    }

    // For vertex shaders, we need to define an input layout.
    ComPtr<ID3D12ShaderReflection> d3d12ShaderReflection;
    if ( FAILED( D3DReflect( d3dShaderBlob->GetBufferPointer(), d3dShaderBlob->GetBufferSize(), IID_PPV_ARGS( &d3d12ShaderReflection ) ) ) )
    {
        LOG_ERROR( "Failed to reflect shader." );
        return false;
    }

    D3D12_SHADER_DESC d3d12ShaderDesc;
    if ( FAILED( d3d12ShaderReflection->GetDesc( &d3d12ShaderDesc ) ) )
    {
        LOG_ERROR( "Failed to query shader description." );
        return false;
    }

    // Make sure out input layout doesn't leak.
    ClearInputLayout();

    D3D12_INPUT_ELEMENT_DESC d3d12InputElementDesc;
    d3d12InputElementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    d3d12InputElementDesc.InputSlot = 0; // Assume packed vertex arrays.
    // Only vertex data supported. To use instancing, pass the per-instance data 
    // in a structured buffer and use the SV_InstanceID system value semantic to 
    // query the per-instance data in the structured buffer.
    d3d12InputElementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA; 
    d3d12InputElementDesc.InstanceDataStepRate = 0;

    for ( UINT i = 0; i < d3d12ShaderDesc.InputParameters; ++i )
    {
        D3D12_SIGNATURE_PARAMETER_DESC d3d12SignatureParameterDesc;
        d3d12ShaderReflection->GetInputParameterDesc( i, &d3d12SignatureParameterDesc );

        size_t strLen = strnlen( d3d12SignatureParameterDesc.SemanticName, 255 );
        char* semanticName = new char[strLen+1];
        strcpy_s( semanticName, strLen+1, d3d12SignatureParameterDesc.SemanticName );

        d3d12InputElementDesc.SemanticName = semanticName;

        d3d12InputElementDesc.SemanticIndex = d3d12SignatureParameterDesc.SemanticIndex;
        d3d12InputElementDesc.Format = GetDXGIFormat( d3d12SignatureParameterDesc );

        // Make sure it is a valid format.
        assert( d3d12InputElementDesc.Format != DXGI_FORMAT_UNKNOWN );

        m_d3d12InputElements.push_back( d3d12InputElementDesc );
    }

    for ( UINT i = 0; i < d3d12ShaderDesc.BoundResources; ++i )
    {
        D3D12_SHADER_INPUT_BIND_DESC bindDesc;
        d3d12ShaderReflection->GetResourceBindingDesc( i, &bindDesc );

        std::string resourceName = bindDesc.Name;
        ParameterType parameterType = ParameterType::Invalid;

        switch ( bindDesc.Type )
        {
        case D3D_SIT_CBUFFER:
            parameterType = ParameterType::ConstantBuffer;
            break;
        case D3D_SIT_TEXTURE:
            parameterType = ParameterType::Texture;
            break;
        case D3D_SIT_UAV_RWTYPED:
            parameterType = ParameterType::RWTexture;
            break;
        case D3D_SIT_SAMPLER:
            parameterType = ParameterType::Sampler;
            break;
        case D3D_SIT_STRUCTURED:
        case D3D_SIT_BYTEADDRESS:
            parameterType = ParameterType::Buffer;
            break;
        case D3D_SIT_UAV_RWSTRUCTURED:
        case D3D_SIT_UAV_RWBYTEADDRESS:
        case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
                parameterType = ParameterType::RWBuffer;
            break;
        }

        ShaderParameter shaderParameter = ShaderParameter( parameterType, bindDesc.BindPoint, bindDesc.BindCount, m_ShaderType, bindDesc.Space );
    }

    // Get the root signature from the compiled shader (if there is one)
    ComPtr<ID3DBlob> d3dRootSignatureBlob;
    if ( SUCCEEDED( D3DGetBlobPart( d3dShaderBlob->GetBufferPointer(), d3dShaderBlob->GetBufferSize(), D3D_BLOB_ROOT_SIGNATURE, 0, &d3dRootSignatureBlob ) ) )
    {
        m_ShaderSignature = std::make_shared<ShaderSignatureDX12>( m_Device.lock(), d3dRootSignatureBlob );
    }

    m_ShaderType = type;
    m_d3dShaderBlob = d3dShaderBlob;

    return true;
}

bool ShaderDX12::LoadShaderFromFile( ShaderType type, const std::wstring& fileName, const std::string& entryPoint, const ShaderMacros& shaderMacros, const std::string& profile )
{
    fs::path filePath( fileName );
    if ( fs::exists( filePath ) && fs::is_regular_file( filePath ) )
    {

        Application::Get().SetLoadingMessage( fileName );

        m_ShaderType = type;
        m_ShaderFile = fileName;
        m_ShaderMacros = shaderMacros;
        m_EntryPoint = entryPoint;
        m_Profile = profile;

        // Create a dependency tracker so we can reload the shader if either the 
        // shader file or any of it's dependencies have changed on disk.
        m_DependencyTracker = DependencyTracker( fileName );
        if ( !m_DependencyTracker.Load() )
        {
            // Save the configuration file for the dependency tracker (if it doesn't exist 
            // for the loaded asset).
            m_DependencyTracker.Save();
        }

        // Update the last load time (even if shader compilation fails).
        // Not doing this will result in the shaders being reloaded every frame
        // even if shader compilation fails.
        m_DependencyTracker.SetLastLoadTime();

        std::ifstream inputFile( fileName );
        std::string source( ( std::istreambuf_iterator<char>( inputFile ) ), std::istreambuf_iterator<char>() );

        return LoadShaderFromString( type, source, fileName, entryPoint, shaderMacros, profile );
    }
    else
    {
        LOG_ERROR( "Failed to load shader \"", fileName, "\". File does not exist or is not a regular file." );
    }

    return false;
}

std::string ShaderDX12::GetLatestProfile( ShaderType type )
{
    // Query the maximum feature level:
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_12_1
    };

    D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevel = { _countof( featureLevels ), featureLevels, D3D_FEATURE_LEVEL_11_0 };
    m_d3d12Device->CheckFeatureSupport( D3D12_FEATURE_FEATURE_LEVELS, &featureLevel, sizeof( featureLevel ) );

    switch ( type )
    {
    case ShaderType::Vertex:
        switch ( featureLevel.MaxSupportedFeatureLevel )
        {
        case D3D_FEATURE_LEVEL_12_1:
        case D3D_FEATURE_LEVEL_12_0:
            return "vs_5_1";
            break;
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "vs_5_0";
            break;
        }
        break;
    case ShaderType::TessellationControl:
        switch ( featureLevel.MaxSupportedFeatureLevel )
        {
        case D3D_FEATURE_LEVEL_12_1:
        case D3D_FEATURE_LEVEL_12_0:
            return "ds_5_1";
            break;
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "ds_5_0";
            break;
        }
        break;
    case ShaderType::TessellationEvaluation:
        switch ( featureLevel.MaxSupportedFeatureLevel )
        {
        case D3D_FEATURE_LEVEL_12_1:
        case D3D_FEATURE_LEVEL_12_0:
            return "hs_5_1";
            break;
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "hs_5_0";
            break;
        }
        break;
    case ShaderType::Geometry:
        switch ( featureLevel.MaxSupportedFeatureLevel )
        {
        case D3D_FEATURE_LEVEL_12_1:
        case D3D_FEATURE_LEVEL_12_0:
            return "gs_5_1";
            break;
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "gs_5_0";
            break;
        }
        break;
    case ShaderType::Pixel:
        switch ( featureLevel.MaxSupportedFeatureLevel )
        {
        case D3D_FEATURE_LEVEL_12_1:
        case D3D_FEATURE_LEVEL_12_0:
            return "ps_5_1";
            break;
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "ps_5_0";
            break;
        }
        break;
    case ShaderType::Compute:
        switch ( featureLevel.MaxSupportedFeatureLevel )
        {
        case D3D_FEATURE_LEVEL_12_1:
        case D3D_FEATURE_LEVEL_12_0:
            return "cs_5_1";
            break;
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "cs_5_0";
            break;
        }
    } // switch( type )

      // Throw an exception?
    return "";
}

std::shared_ptr<ShaderSignature> ShaderDX12::GetShaderSignature()
{
    return m_ShaderSignature;
}

Microsoft::WRL::ComPtr<ID3DBlob> ShaderDX12::GetD3DShaderBlob() const
{
    return m_d3dShaderBlob;
}

void ShaderDX12::SetInputLayout( const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout )
{
    ClearInputLayout();
    m_d3d12InputElements.resize( inputLayout.size() );

    for ( size_t i = 0; i < inputLayout.size(); ++i )
    {
        const D3D12_INPUT_ELEMENT_DESC& inputElement = inputLayout[i];
        size_t strLen = strnlen( inputElement.SemanticName, 255 );
        char* semanticName = new char[strLen + 1];
        strcpy_s( semanticName, strLen + 1, inputElement.SemanticName );

        m_d3d12InputElements[i] = inputElement;
        m_d3d12InputElements[i].SemanticName = semanticName;
    }
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>& ShaderDX12::GetInputLayout() const
{
    return m_d3d12InputElements;
}

// Inspired by: http://takinginitiative.net/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/
DXGI_FORMAT GetDXGIFormat( const D3D12_SIGNATURE_PARAMETER_DESC& paramDesc )
{
    DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;

    if ( paramDesc.Mask == 1 )
    {
        switch ( paramDesc.ComponentType )
        {
        case D3D_REGISTER_COMPONENT_UINT32:
            dxgiFormat = DXGI_FORMAT_R32_UINT;
            break;
        case D3D_REGISTER_COMPONENT_SINT32:
            dxgiFormat = DXGI_FORMAT_R32_SINT;
            break;
        case D3D_REGISTER_COMPONENT_FLOAT32:
            dxgiFormat = DXGI_FORMAT_R32_FLOAT;
            break;
        }
    }
    else if ( paramDesc.Mask <= 3 )
    {
        switch ( paramDesc.ComponentType )
        {
        case D3D_REGISTER_COMPONENT_UINT32:
            dxgiFormat = DXGI_FORMAT_R32G32_UINT;
            break;
        case D3D_REGISTER_COMPONENT_SINT32:
            dxgiFormat = DXGI_FORMAT_R32G32_SINT;
            break;
        case D3D_REGISTER_COMPONENT_FLOAT32:
            dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
            break;
        }
    }
    else if ( paramDesc.Mask <= 7 )
    {
        switch ( paramDesc.ComponentType )
        {
        case D3D_REGISTER_COMPONENT_UINT32:
            dxgiFormat = DXGI_FORMAT_R32G32B32_UINT;
            break;
        case D3D_REGISTER_COMPONENT_SINT32:
            dxgiFormat = DXGI_FORMAT_R32G32B32_SINT;
            break;
        case D3D_REGISTER_COMPONENT_FLOAT32:
            dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        }
    }
    else if ( paramDesc.Mask <= 15 )
    {
        switch ( paramDesc.ComponentType )
        {
        case D3D_REGISTER_COMPONENT_UINT32:
            dxgiFormat = DXGI_FORMAT_R32G32B32A32_UINT;
            break;
        case D3D_REGISTER_COMPONENT_SINT32:
            dxgiFormat = DXGI_FORMAT_R32G32B32A32_SINT;
            break;
        case D3D_REGISTER_COMPONENT_FLOAT32:
            dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
        }
    }

    return dxgiFormat;
}

void ShaderDX12::OnFileChanged( Core::FileChangeEventArgs& e )
{
    if ( LoadShaderFromFile(m_ShaderType, m_ShaderFile, m_EntryPoint, m_ShaderMacros, m_Profile) )
    {
        base::OnFileChanged( e );
    }
}