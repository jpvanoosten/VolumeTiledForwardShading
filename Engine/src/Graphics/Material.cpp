#include <EnginePCH.h>

#include <Graphics/Device.h>
#include <Graphics/Texture.h>
#include <Graphics/ConstantBuffer.h>

#include <Graphics/Material.h>

using namespace Graphics;

Material::Material( std::shared_ptr<Device> device )
    : m_Device( device )
    , m_Dirty( false )
{
    m_pProperties = (MaterialProperties*)_aligned_malloc( sizeof( MaterialProperties ), 16 );
    // Construct default material properties.
    *m_pProperties = MaterialProperties();

    // Create an empty texture to use as the default texture when no texture is bound to a 
    // material property.
    m_DefaultTexture = device->CreateTexture2D( 1, 1, 1, TextureFormat::R8G8B8A8_UNORM );
}

Material::~Material()
{
    if ( m_pProperties) 
    {
        _aligned_free( m_pProperties );
        m_pProperties = nullptr;
    }
}

const glm::vec4& Material::GetGlobalAmbientColor() const
{
    return m_pProperties->m_GlobalAmbient;
}

void Material::SetGlobalAmbientColor( const glm::vec4& globalAmbient )
{
    m_pProperties->m_GlobalAmbient = globalAmbient;
    m_Dirty = true;
}

const glm::vec4& Material::GetAmbientColor() const
{
    return m_pProperties->m_AmbientColor;
}

void Material::SetAmbientColor( const glm::vec4& ambient )
{
    m_pProperties->m_AmbientColor = ambient;
    m_Dirty = true;
}

const glm::vec4& Material::GetDiffuseColor() const
{
	return m_pProperties->m_DiffuseColor;
}

void Material::SetDiffuseColor( const glm::vec4& diffuse )
{
    m_pProperties->m_DiffuseColor = diffuse;
    m_Dirty = true;
}

const glm::vec4& Material::GetEmissiveColor() const
{
	return m_pProperties->m_EmissiveColor;
}

void Material::SetEmissiveColor( const glm::vec4& emissive )
{
    m_pProperties->m_EmissiveColor = emissive;
    m_Dirty = true;
}

const glm::vec4& Material::GetSpecularColor() const
{
	return m_pProperties->m_SpecularColor;
}

void Material::SetSpecularColor( const glm::vec4& specular )
{
    m_pProperties->m_SpecularColor = specular;
    m_Dirty = true;
}

float Material::GetSpecularPower() const
{
	return m_pProperties->m_SpecularPower;
}

const float Material::GetOpacity() const
{
    return m_pProperties->m_Opacity;
}

void Material::SetOpacity( float Opacity )
{
    m_pProperties->m_Opacity = Opacity;
    m_Dirty = true;
}

void Material::SetSpecularPower( float phongPower )
{
    m_pProperties->m_SpecularPower = phongPower;
    m_Dirty = true;
}

const glm::vec4& Material::GetReflectance() const
{
    return m_pProperties->m_Reflectance;
}

void Material::SetReflectance( const glm::vec4& reflectance )
{
    m_pProperties->m_Reflectance = reflectance;
    m_Dirty = true;
}

float Material::GetIndexOfRefraction() const
{
    return m_pProperties->m_IndexOfRefraction;
}

void Material::SetIndexOfRefraction( float indexOfRefraction )
{
    m_pProperties->m_IndexOfRefraction = indexOfRefraction;
    m_Dirty = true;
}

float Material::GetBumpIntensity() const
{
    return m_pProperties->m_BumpIntensity;
}
void Material::SetBumpIntensity( float bumpIntensity )
{
    m_pProperties->m_BumpIntensity = bumpIntensity;
}

std::shared_ptr<Texture> Material::GetTexture( TextureType type ) const
{
    TextureMap::const_iterator itr = m_Textures.find( type );
    if ( itr != m_Textures.end() )
    {
        return itr->second;
    }
    else
    {
        return m_DefaultTexture;
    }
}

void Material::SetTexture( TextureType type, std::shared_ptr<Texture> texture )
{
    m_Textures[type] = texture;

    switch ( type )
    {
    case TextureType::Ambient:
    {
        m_pProperties->m_HasAmbientTexture = ( texture != nullptr );
    }
    break;
    case TextureType::Emissive:
    {
        m_pProperties->m_HasEmissiveTexture = ( texture != nullptr );
    }
    break;
    case TextureType::Diffuse:
    {
        m_pProperties->m_HasDiffuseTexture = ( texture != nullptr );
    }
    break;
    case TextureType::Specular:
    {
        m_pProperties->m_HasSpecularTexture = ( texture != nullptr );
    }
    break;
    case TextureType::SpecularPower:
    {
        m_pProperties->m_HasSpecularPowerTexture = ( texture != nullptr );
    }
    break;
    case TextureType::Normal:
    {
        m_pProperties->m_HasNormalTexture = ( texture != nullptr );
    }
    break;
    case TextureType::Bump:
    {
        m_pProperties->m_HasBumpTexture = ( texture != nullptr );
    }
    break;
    case TextureType::Opacity:
    {
        m_pProperties->m_HasOpacityTexture = ( texture != nullptr );
    }
    break;
    }

    m_Dirty = true;
}

bool Material::IsTransparent() const
{
    return ( m_pProperties->m_Opacity < 1.0f ||
             m_pProperties->m_HasOpacityTexture ||
             ( m_pProperties->m_HasDiffuseTexture && GetTexture( TextureType::Diffuse )->IsTransparent() ) || 
             m_pProperties->m_AlphaThreshold <= 0.0f ); // Objects with an alpha threshold > 0 should be drawn in the opaque pass.
}

const MaterialProperties& Material::GetMaterialProperties() const
{
    return *m_pProperties;
}
