#include <GamePCH.h>

#include <ConfigurationSettings.h>

using boost::serialization::make_nvp;

ConfigurationSettings::ConfigurationSettings()
    : WindowWidth( 800 )
    , WindowHeight( 600 )
    , FullScreen( false )
    , VSync( true )
    , MultiSampleEnable( true )
    , SceneFileName( L"" )
    , LoadingScreenFileName( L"" )
    , SceneScaleFactor( 1.0f )
    , CameraPosition( 0.0f )
    , CameraRotation()
    // Light generation properties.
    , LightsMinBounds( -1.0f )
    , LightsMaxBounds( 1.0f )
    , MinSpotAngle( 1.0f )
    , MaxSpotAngle( 60.0f )
    , MinRange( 0.1f )
    , MaxRange( 100.0f )
    , NumPointLights( 50 )
    , NumSpotLights( 10 )
    , NumDirectionalLights( 1 )
    , LoadingProgressTotal( 100.0f )
{
    // Must contain at least 1 (default) light.
    PointLights.resize( 1 );
    SpotLights.resize( 1 );
    DirectionalLights.resize( 1 );
}

bool ConfigurationSettings::Load( const std::wstring& fileName )
{
    std::ifstream configInputStream( fileName, std::ios::in );
    if ( configInputStream.is_open() )
    {
        m_Filename = fileName;

        boost::archive::xml_iarchive ia( configInputStream );
        ia >> make_nvp( "ConfigurationSettings", *this );

        return true;
    }

    return false;
}

bool ConfigurationSettings::Reload()
{
    if ( !m_Filename.empty() )
    {
        return Load( m_Filename );
    }

    return false;
}

bool ConfigurationSettings::Save( const std::wstring& fileName )
{
    if ( !fileName.empty() )
    {
        m_Filename = fileName;
    }

    std::ofstream configOutputStream( m_Filename, std::ios::out );
    if ( configOutputStream.is_open() )
    {
        boost::archive::xml_oarchive oa( configOutputStream );
        oa << make_nvp( "ConfigurationSettings", *this );

        return true;
    }

    return false;
}

std::vector<fs::path> ConfigurationSettings::GetAbsoluteSearchPaths() const
{
    std::vector<fs::path> absolutePaths( SearchPaths.size() );

    fs::path filePath( m_Filename );
    size_t i = 0;
    for ( fs::path searchPath : SearchPaths )
    {
        absolutePaths[i] = fs::canonical( filePath.parent_path() / searchPath );
    }

    return absolutePaths;
}
