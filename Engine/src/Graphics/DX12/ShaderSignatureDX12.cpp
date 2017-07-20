#include <EnginePCH.h>

#include <Graphics/DX12/ShaderSignatureDX12.h>
#include <Graphics/ShaderParameter.h>
#include <Graphics/DX12/SamplerDX12.h>
#include <Graphics/DX12/DeviceDX12.h>

#include <LogManager.h>

using namespace Graphics;
using namespace Microsoft::WRL;

static std::map< size_t, ID3D12RootSignature* > gs_RootSignatureHashMap;
static std::mutex gs_RootSignatureHashMapMutex;

std::size_t hash_value( const D3D12_DESCRIPTOR_RANGE& descriptorTable )
{
    std::size_t seed = 0;

    boost::hash_combine( seed, descriptorTable.RangeType );
    boost::hash_combine( seed, descriptorTable.NumDescriptors );
    boost::hash_combine( seed, descriptorTable.BaseShaderRegister );
    boost::hash_combine( seed, descriptorTable.RegisterSpace );
    boost::hash_combine( seed, descriptorTable.OffsetInDescriptorsFromTableStart );

    return seed;
}

std::size_t hash_value( const D3D12_ROOT_DESCRIPTOR_TABLE& descriptorTable )
{
    std::size_t seed = 0;

    for ( uint32_t i = 0; i < descriptorTable.NumDescriptorRanges; ++i )
    {
        boost::hash_combine( seed, descriptorTable.pDescriptorRanges[i] );
    }

    return seed;
}

std::size_t hash_value( const D3D12_ROOT_CONSTANTS& rootConstants )
{
    std::size_t seed = 0;

    boost::hash_combine( seed, rootConstants.ShaderRegister );
    boost::hash_combine( seed, rootConstants.RegisterSpace );
    boost::hash_combine( seed, rootConstants.Num32BitValues );

    return seed;
}

std::size_t hash_value( const D3D12_ROOT_DESCRIPTOR& rootDescriptor )
{
    std::size_t seed = 0;

    boost::hash_combine( seed, rootDescriptor.RegisterSpace );
    boost::hash_combine( seed, rootDescriptor.ShaderRegister );

    return seed;
}

std::size_t hash_value( const D3D12_ROOT_PARAMETER& rootParamter )
{
    std::size_t seed = 0;

    switch ( rootParamter.ParameterType )
    {
    case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
        boost::hash_combine( seed, rootParamter.DescriptorTable );
        break;
    case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
        boost::hash_combine( seed, rootParamter.Constants );
        break;
    case D3D12_ROOT_PARAMETER_TYPE_CBV:
    case D3D12_ROOT_PARAMETER_TYPE_SRV:
    case D3D12_ROOT_PARAMETER_TYPE_UAV:
        boost::hash_combine( seed, rootParamter.Descriptor );
        break;
    }

    boost::hash_combine( seed, rootParamter.ShaderVisibility );

    return seed;
}

std::size_t hash_value( const D3D12_STATIC_SAMPLER_DESC& samplerDesc )
{
    std::size_t seed = 0;

    boost::hash_combine( seed, samplerDesc.Filter );
    boost::hash_combine( seed, samplerDesc.AddressU );
    boost::hash_combine( seed, samplerDesc.AddressV );
    boost::hash_combine( seed, samplerDesc.AddressW );
    boost::hash_combine( seed, samplerDesc.MipLODBias );
    boost::hash_combine( seed, samplerDesc.MaxAnisotropy );
    boost::hash_combine( seed, samplerDesc.ComparisonFunc );
    boost::hash_combine( seed, samplerDesc.BorderColor );
    boost::hash_combine( seed, samplerDesc.MinLOD );
    boost::hash_combine( seed, samplerDesc.MaxLOD );
    boost::hash_combine( seed, samplerDesc.ShaderRegister );
    boost::hash_combine( seed, samplerDesc.RegisterSpace );
    boost::hash_combine( seed, samplerDesc.ShaderVisibility );

    return seed;
}

std::size_t hash_value( const D3D12_ROOT_SIGNATURE_DESC& rootSignatureDesc )
{
    std::size_t seed = 0;

    for ( uint32_t i = 0; i < rootSignatureDesc.NumParameters; ++i )
    {
        boost::hash_combine( seed, rootSignatureDesc.pParameters[i] );
    }
    for ( uint32_t i = 0; i < rootSignatureDesc.NumStaticSamplers; ++i )
    {
        boost::hash_combine( seed, rootSignatureDesc.pStaticSamplers[i] );
    }

    boost::hash_combine( seed, rootSignatureDesc.Flags );

    return seed;
}

ShaderType TranslateShaderVisibility( D3D12_SHADER_VISIBILITY shaderVisibility )
{
    ShaderType shaderType = ShaderType::All;

    switch ( shaderVisibility )
    {
    case D3D12_SHADER_VISIBILITY_VERTEX:
        shaderType = ShaderType::Vertex;
        break;
    case D3D12_SHADER_VISIBILITY_HULL:
        shaderType = ShaderType::TessellationControl;
        break;
    case D3D12_SHADER_VISIBILITY_DOMAIN:
        shaderType = ShaderType::TessellationEvaluation;
        break;
    case D3D12_SHADER_VISIBILITY_GEOMETRY:
        shaderType = ShaderType::Geometry;
        break;
    case D3D12_SHADER_VISIBILITY_PIXEL:
        shaderType = ShaderType::Pixel;
        break;
    }

    return shaderType;
}

ShaderParameter TranslateDescriptorRange( const D3D12_DESCRIPTOR_RANGE& descriptorRange )
{
    ShaderParameter shaderParameter = ShaderParameter( ParameterType::Invalid, descriptorRange.BaseShaderRegister, descriptorRange.NumDescriptors, ShaderType::All, descriptorRange.RegisterSpace );

    switch ( descriptorRange.RangeType )
    {
    case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
        shaderParameter.SetType( ParameterType::ConstantBuffer );
        break;
    case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
        shaderParameter.SetType( ParameterType::Texture );
        break;
    case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
        shaderParameter.SetType( ParameterType::RWTexture );
        break;
    case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
        shaderParameter.SetType( ParameterType::Sampler );
        break;
    }

    return shaderParameter;
}

ShaderParameter TranslateTableParameter( const D3D12_ROOT_DESCRIPTOR_TABLE& rootDescriptorTable, ShaderType shaderVisibility )
{
    ShaderParameter tableParameter( ParameterType::Table, 0, 0, shaderVisibility );
    
    for ( unsigned int i = 0; i < rootDescriptorTable.NumDescriptorRanges; ++i )
    {
        tableParameter[i] = TranslateDescriptorRange( rootDescriptorTable.pDescriptorRanges[i] );
    }

    return tableParameter;
}

ShaderSignatureDX12::ShaderSignatureDX12( std::shared_ptr<DeviceDX12> device )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
    , m_d3d12RootSignatureFlags( D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT )
    , m_IsDirty( true )
{
    // Reserve some slots in advance.
    m_RootParameters.reserve( 16 );
    m_StaticSamplers.reserve( 4 );
}

ShaderSignatureDX12::ShaderSignatureDX12( std::shared_ptr<DeviceDX12> device, Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
    , m_d3d12RootSignatureFlags( D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT )
    , m_IsDirty( false )
{
    // Deserialize the root signature
    ComPtr<ID3D12RootSignatureDeserializer> rootSignatureDeserializer;
    if ( FAILED( D3D12CreateRootSignatureDeserializer( rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS( &rootSignatureDeserializer ) ) ) )
    {
        LOG_ERROR( "Failed to create root signature deserializer." );
    }
    else
    {
        const D3D12_ROOT_SIGNATURE_DESC* pRootSignatureDesc = rootSignatureDeserializer->GetRootSignatureDesc();
        m_RootParameters.resize( pRootSignatureDesc->NumParameters );
        for ( unsigned int i = 0; i < pRootSignatureDesc->NumParameters; ++i )
        {
            const D3D12_ROOT_PARAMETER& rootParameter = pRootSignatureDesc->pParameters[i];
            ShaderType shaderVisibility = TranslateShaderVisibility( rootParameter.ShaderVisibility );
            switch ( rootParameter.ParameterType )
            {
            case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
                m_RootParameters[i] = ShaderParameter( ParameterType::Constants, rootParameter.Constants.ShaderRegister, rootParameter.Constants.Num32BitValues, shaderVisibility, rootParameter.Constants.RegisterSpace );
                break;
            case D3D12_ROOT_PARAMETER_TYPE_CBV:
                m_RootParameters[i] = ShaderParameter( ParameterType::ConstantBuffer, rootParameter.Descriptor.ShaderRegister, 1, shaderVisibility, rootParameter.Descriptor.RegisterSpace );
                break;
            case D3D12_ROOT_PARAMETER_TYPE_SRV:
                m_RootParameters[i] = ShaderParameter( ParameterType::Buffer, rootParameter.Descriptor.ShaderRegister, 1, shaderVisibility, rootParameter.Descriptor.RegisterSpace );
                break;
            case D3D12_ROOT_PARAMETER_TYPE_UAV:
                m_RootParameters[i] = ShaderParameter( ParameterType::RWBuffer, rootParameter.Descriptor.ShaderRegister, 1, shaderVisibility, rootParameter.Descriptor.RegisterSpace );
                break;
            case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
                m_RootParameters[i] = TranslateTableParameter( rootParameter.DescriptorTable, shaderVisibility );
                break;
            }
        }

        m_StaticSamplers.resize( pRootSignatureDesc->NumStaticSamplers );
        for ( unsigned int i = 0; i < pRootSignatureDesc->NumStaticSamplers; ++i )
        {
            StaticSampler staticSampler;
            staticSampler.StaticSamplerDesc = pRootSignatureDesc->pStaticSamplers[i];
            staticSampler.Sampler = std::make_shared<SamplerDX12>( device, staticSampler.StaticSamplerDesc );
            m_StaticSamplers[i] = staticSampler;
        }

        m_d3d12RootSignatureFlags = pRootSignatureDesc->Flags;

        m_d3d12RootSignature = CreateRootSignature( *pRootSignatureDesc );
    }
}

ShaderSignatureDX12::~ShaderSignatureDX12()
{}

ShaderSignature& ShaderSignatureDX12::operator=( const ShaderSignature& rhs )
{
    const ShaderSignatureDX12& shaderSignatureDX12 = dynamic_cast<const ShaderSignatureDX12&>( rhs );
    if ( &shaderSignatureDX12 != this )
    {
        m_RootParameters = shaderSignatureDX12.m_RootParameters;
        m_StaticSamplers = shaderSignatureDX12.m_StaticSamplers;
        m_IsDirty = true;
    }

    return *this;
}

const ShaderSignature::ParameterList& ShaderSignatureDX12::GetParameters() const
{
    return m_RootParameters;
}

void ShaderSignatureDX12::SetParameters( const ParameterList& parameterList )
{
    m_IsDirty = true;
    m_RootParameters = parameterList;
}

const ShaderParameter& ShaderSignatureDX12::GetParameter( unsigned int index ) const
{
    return operator[](index);
}

void ShaderSignatureDX12::SetParameter( unsigned int index, const ShaderParameter& parameter )
{
    operator[]( index ) = parameter;
}

uint32_t ShaderSignatureDX12::GetNumParameters() const
{
    return static_cast<uint32_t>( m_RootParameters.size() );
}

ShaderParameter& ShaderSignatureDX12::operator[]( unsigned int index )
{
    if ( m_RootParameters.size() < index + 1 )
    {
        m_RootParameters.resize( index + 1 );
    }
    // Passing a non-const reference of a parameter could invalidate the shader signature.
    // Set the dirty flag to rebuild the shader signature.
    m_IsDirty = true;
    return m_RootParameters[index];
}

const ShaderParameter& ShaderSignatureDX12::operator[]( unsigned int index ) const
{
    assert( index < m_RootParameters.size() );
    return m_RootParameters[index];
}

D3D12_SHADER_VISIBILITY TranslateShaderVisibility( ShaderType shaderVisibility )
{
    // In DirectX12, you can set the shader visibility to either all (which is required for compute shader signatures)
    // or 1 of the other shader stages. It's not possible to use a bitfield representing the various shader stages. (Like in Vulkan).
    D3D12_SHADER_VISIBILITY d3d12ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    switch ( shaderVisibility )
    {
    case ShaderType::Vertex:
        d3d12ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        break;
    case ShaderType::TessellationControl:
        d3d12ShaderVisibility = D3D12_SHADER_VISIBILITY_HULL;
        break;
    case ShaderType::TessellationEvaluation:
        d3d12ShaderVisibility = D3D12_SHADER_VISIBILITY_DOMAIN;
        break;
    case ShaderType::Geometry:
        d3d12ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
        break;
    case ShaderType::Pixel:
        d3d12ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        break;
    }

    return d3d12ShaderVisibility;
}

void ShaderSignatureDX12::SetStaticSampler( uint32_t slotID, std::shared_ptr<Sampler> sampler, uint32_t shaderRegister, ShaderType shaderVisibility, uint32_t registerSpace )
{
    // Make sure we have room to place the static sampler.
    if ( m_StaticSamplers.size() < slotID + 1 )
    {
        m_StaticSamplers.resize( slotID + 1 );
    }

    StaticSampler staticSampler;
    std::shared_ptr<SamplerDX12> samplerDX12 = std::dynamic_pointer_cast<SamplerDX12>( sampler );
    if ( samplerDX12 )
    {
        D3D12_STATIC_SAMPLER_DESC staticSamplerDesc = samplerDX12->GetStaticSamplerDesc();
        staticSamplerDesc.ShaderRegister = shaderRegister;
        staticSamplerDesc.RegisterSpace = registerSpace;
        staticSamplerDesc.ShaderVisibility = TranslateShaderVisibility( shaderVisibility );

        staticSampler.Sampler = samplerDX12;
        staticSampler.StaticSamplerDesc = staticSamplerDesc;
    }

    m_StaticSamplers[slotID] = staticSampler;

    m_IsDirty = true;
}

std::shared_ptr<Sampler> ShaderSignatureDX12::GetStaticSampler( uint32_t slotID ) const
{
    assert( slotID < m_StaticSamplers.size() );
    return m_StaticSamplers[slotID].Sampler;
}

uint32_t ShaderSignatureDX12::GetNumStaticSamplers() const
{
    return static_cast<uint32_t>( m_StaticSamplers.size() );
}


D3D12_ROOT_DESCRIPTOR TranslateDescriptorParameter( const ShaderParameter& shaderParameter )
{
    D3D12_ROOT_DESCRIPTOR rootDescriptor = {};
    
    rootDescriptor.ShaderRegister = shaderParameter.GetBaseRegister();
    rootDescriptor.RegisterSpace = shaderParameter.GetRegisterSpace();
    
    return rootDescriptor;
}

D3D12_ROOT_CONSTANTS TranslateConstantParameter( const ShaderParameter& shaderParameter )
{
    D3D12_ROOT_CONSTANTS rootConstants = {};

    rootConstants.Num32BitValues = shaderParameter.GetNumEntries();
    rootConstants.ShaderRegister = shaderParameter.GetBaseRegister();
    rootConstants.RegisterSpace = shaderParameter.GetRegisterSpace();

    return rootConstants;
}

D3D12_DESCRIPTOR_RANGE TranslateDescriptorRange( const ShaderParameter& shaderParameter )
{
    D3D12_DESCRIPTOR_RANGE descriptorRange = {};
    switch ( shaderParameter.GetType() )
    {
    case ParameterType::ConstantBuffer:
        descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        break;
    case ParameterType::Buffer:
    case ParameterType::Texture:
        descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        break;
    case ParameterType::RWBuffer:
    case ParameterType::RWTexture:
        descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        break;
    case ParameterType::Sampler:
        descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        break;
    default:
        LOG_ERROR( "Invalid shader parameter type for a descriptor range." );
        break;
    }

    descriptorRange.NumDescriptors = shaderParameter.GetNumEntries();
    descriptorRange.BaseShaderRegister = shaderParameter.GetBaseRegister();
    descriptorRange.RegisterSpace = shaderParameter.GetRegisterSpace();
    descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    return descriptorRange;
}

D3D12_ROOT_DESCRIPTOR_TABLE TranslateTableParameter( const ShaderParameter& shaderParameter )
{
    D3D12_ROOT_DESCRIPTOR_TABLE rootDescriptorTable = {};

    const ShaderParameter::NestedParameters& parameterTable = shaderParameter.GetNestedParameters();
    if ( parameterTable.size() > 0 )
    {
        rootDescriptorTable.NumDescriptorRanges = static_cast<UINT>( parameterTable.size() );
        D3D12_DESCRIPTOR_RANGE* pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[rootDescriptorTable.NumDescriptorRanges];
        unsigned int descriptorRangeIndex = 0;
        for ( const auto& descriptorRange : parameterTable )
        {
            pDescriptorRanges[descriptorRangeIndex++] = TranslateDescriptorRange( descriptorRange );
        }

        rootDescriptorTable.pDescriptorRanges = pDescriptorRanges;
    }

    return rootDescriptorTable;
}

D3D12_ROOT_PARAMETER TranslateShaderParameter( const ShaderParameter& shaderParameter )
{
    D3D12_ROOT_PARAMETER rootParameter = {};
    rootParameter.ShaderVisibility = TranslateShaderVisibility( shaderParameter.GetShaderVisibility() );

    switch ( shaderParameter.GetType() )
    {
    case ParameterType::Table:
        rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameter.DescriptorTable = TranslateTableParameter( shaderParameter );
        break;
    case ParameterType::Constants:
        rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        rootParameter.Constants = TranslateConstantParameter( shaderParameter );
        break;
    case ParameterType::ConstantBuffer:
        rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameter.Descriptor = TranslateDescriptorParameter( shaderParameter );
        break;
    case ParameterType::Buffer:
    case ParameterType::Texture:
        rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        rootParameter.Descriptor = TranslateDescriptorParameter( shaderParameter );
        break;
    case ParameterType::RWBuffer:
    case ParameterType::RWTexture:
        rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
        rootParameter.Descriptor = TranslateDescriptorParameter( shaderParameter );
        break;
    }

    return rootParameter;
}

/**
 * Delete the heap allocated memory for the root signature description.
 */
void DeleteRootSignatureDescription( const D3D12_ROOT_SIGNATURE_DESC& rootSignature )
{
    for ( unsigned int i = 0; i < rootSignature.NumParameters; ++i )
    {
        const D3D12_ROOT_PARAMETER& rootParameter = rootSignature.pParameters[i];
        if ( rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE )
        {
            delete [] rootParameter.DescriptorTable.pDescriptorRanges;
        }
    }

    delete [] rootSignature.pParameters;
    delete [] rootSignature.pStaticSamplers;
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> ShaderSignatureDX12::CreateRootSignature( const D3D12_ROOT_SIGNATURE_DESC& rootSignatureDesc )
{
    size_t hashCode = boost::hash<D3D12_ROOT_SIGNATURE_DESC>{}( rootSignatureDesc );

    ComPtr<ID3D12RootSignature> rootSignature;

    scoped_lock lock( gs_RootSignatureHashMapMutex );

    auto iter = gs_RootSignatureHashMap.find( hashCode );
    if ( iter == gs_RootSignatureHashMap.end() )
    {
        ComPtr<ID3DBlob> rootSignatureBlob;
        ComPtr<ID3DBlob> errorBlob;
        HRESULT hr = D3D12SerializeRootSignature( &rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSignatureBlob, &errorBlob );

        if ( FAILED( hr ) )
        {
            LOG_ERROR( "Failed to serialize root signature\n", reinterpret_cast<char*>( errorBlob->GetBufferPointer() ) );
            return nullptr;
        }

        if ( FAILED( m_d3d12Device->CreateRootSignature( 1, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS( &rootSignature ) ) ) )
        {
            LOG_ERROR( "Failed to create root signature." );
            return nullptr;
        }

        gs_RootSignatureHashMap.insert( { hashCode, rootSignature.Get() } );
    }
    else
    {
        rootSignature = gs_RootSignatureHashMap[hashCode];
    }

    return rootSignature;
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> ShaderSignatureDX12::GetD3D12RootSignature()
{
    if ( m_IsDirty )
    {
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDescription = {};
        rootSignatureDescription.Flags = m_d3d12RootSignatureFlags;

        if ( m_RootParameters.size() > 0 )
        {
            rootSignatureDescription.NumParameters = static_cast<UINT>( m_RootParameters.size() );
            D3D12_ROOT_PARAMETER* pRootParameters = new D3D12_ROOT_PARAMETER[rootSignatureDescription.NumParameters];
            unsigned int rootParameterIndex = 0;
            for ( const auto& shaderParameter : m_RootParameters )
            {
                pRootParameters[rootParameterIndex++] = TranslateShaderParameter( shaderParameter );
            }
            rootSignatureDescription.pParameters = pRootParameters;
        }
        if ( m_StaticSamplers.size() > 0 )
        {
            rootSignatureDescription.NumStaticSamplers = static_cast<UINT>( m_StaticSamplers.size() );
            D3D12_STATIC_SAMPLER_DESC* pStaticSamplers = new D3D12_STATIC_SAMPLER_DESC[rootSignatureDescription.NumStaticSamplers];
            unsigned int staticSamplerIndex = 0;
            for ( const auto& staticSampler : m_StaticSamplers )
            {
                pStaticSamplers[staticSamplerIndex++] = staticSampler.StaticSamplerDesc;
            }
            rootSignatureDescription.pStaticSamplers = pStaticSamplers;
        }

        m_d3d12RootSignature = CreateRootSignature( rootSignatureDescription );

        // Free the heap allocated storage that was used to build the root signature description.
        DeleteRootSignatureDescription( rootSignatureDescription );

        m_IsDirty = false;
    }

    return m_d3d12RootSignature;
}

D3D12_ROOT_SIGNATURE_FLAGS ShaderSignatureDX12::GetD3D12RootSignatureFlags() const
{
    return m_d3d12RootSignatureFlags;
}
