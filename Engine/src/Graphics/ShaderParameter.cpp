#include <EnginePCH.h>

#include <Graphics/ShaderParameter.h>

using namespace Graphics;

ShaderParameter::ShaderParameter( ParameterType parameterType )
    : m_ParameterType( parameterType )
    , m_ShaderVisibility( ShaderType::Unknown )
    , m_BaseRegister( 0 )
    , m_RegisterSpace( 0 )
    , m_NumEntries( 0 )
{}

ShaderParameter::ShaderParameter( ParameterType parameterType, uint32_t shaderRegister, uint32_t numEntries, ShaderType shaderVisibility, uint32_t registerSpace )
    : m_ParameterType( parameterType )
    , m_BaseRegister( shaderRegister )
    , m_NumEntries( numEntries )
    , m_NestedParameters( numEntries )
    , m_ShaderVisibility( shaderVisibility )
    , m_RegisterSpace( registerSpace )
{}

ShaderParameter::ShaderParameter( const ShaderParameter& copy )
    : m_ParameterType( copy.m_ParameterType )
    , m_BaseRegister( copy.m_BaseRegister )
    , m_NumEntries( copy.m_NumEntries )
    , m_ShaderVisibility( copy.m_ShaderVisibility )
    , m_RegisterSpace( copy.m_RegisterSpace )
    , m_NestedParameters( copy.m_NestedParameters )
{}

ShaderParameter& ShaderParameter::operator=( const ShaderParameter& rhs )
{
    if ( this != &rhs )
    {
        m_ParameterType = rhs.m_ParameterType;
        m_BaseRegister = rhs.m_BaseRegister;
        m_NumEntries = rhs.m_NumEntries;
        m_ShaderVisibility = rhs.m_ShaderVisibility;
        m_RegisterSpace = rhs.m_RegisterSpace;
        m_NestedParameters = rhs.m_NestedParameters;
    }

    return *this;
}

ParameterType ShaderParameter::GetType() const
{
    return m_ParameterType;
}

void ShaderParameter::SetType( ParameterType parameterType )
{
    m_ParameterType = parameterType;
}

Graphics::ShaderType ShaderParameter::GetShaderVisibility() const
{
    return m_ShaderVisibility;
}

void ShaderParameter::SetShaderVisibility( Graphics::ShaderType shaderStages )
{
    m_ShaderVisibility = shaderStages;
}

uint32_t ShaderParameter::GetBaseRegister() const
{
    return m_BaseRegister;
}

void ShaderParameter::SetBaseRegister( uint32_t baseRegister )
{
    m_BaseRegister = baseRegister;
}

uint32_t ShaderParameter::GetNumEntries() const
{
    return m_NumEntries;
}

void ShaderParameter::SetNumEntries( uint32_t numEntries )
{
    m_NumEntries = numEntries;
    m_NestedParameters.resize( m_NumEntries );
}

uint32_t ShaderParameter::GetRegisterSpace() const
{
    return m_RegisterSpace;
}

void ShaderParameter::SetRegisterSpace( uint32_t space )
{
    m_RegisterSpace = space;
}

ShaderParameter& ShaderParameter::operator[]( const unsigned int index )
{
    // Make sure there is enough storage to store the parameter.
    if ( m_NestedParameters.size() < index + 1 )
    {
        m_NestedParameters.resize( index + 1 );
    }
    return m_NestedParameters[index];
}

const ShaderParameter& ShaderParameter::operator[]( const unsigned int index ) const
{
    assert( index < m_NestedParameters.size() );
    return m_NestedParameters[index];
}

const ShaderParameter::NestedParameters& ShaderParameter::GetNestedParameters() const
{
    return m_NestedParameters;
}

void ShaderParameter::SetNestedParameters( const NestedParameters& nestedParameters )
{
    m_NestedParameters = nestedParameters;
}

