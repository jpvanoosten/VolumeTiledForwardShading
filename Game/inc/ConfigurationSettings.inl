#include <Serialization.h>

template<class Archive>
void ConfigurationSettings::serialize( Archive& ar, const unsigned int version )
{
    ar & BOOST_SERIALIZATION_NVP( WindowWidth );
    ar & BOOST_SERIALIZATION_NVP( WindowHeight );
    ar & BOOST_SERIALIZATION_NVP( FullScreen );
    if ( version > 0 )
    {
        ar & BOOST_SERIALIZATION_NVP( VSync );
    }
    if ( version > 1 )
    {
        ar & BOOST_SERIALIZATION_NVP( MultiSampleEnable );
    }
    if ( version > 3 )
    {
        ar & BOOST_SERIALIZATION_NVP( LoadingScreenFileName );
    }
    ar & BOOST_SERIALIZATION_NVP( SceneFileName );

    if (version > 4)
    {
        ar & BOOST_SERIALIZATION_NVP( SearchPaths );
    }

    ar & BOOST_SERIALIZATION_NVP( SceneScaleFactor );
    ar & BOOST_SERIALIZATION_NVP( CameraPosition );
    ar & BOOST_SERIALIZATION_NVP( CameraRotation );
    ar & BOOST_SERIALIZATION_NVP( CameraPivotDistance );
    ar & BOOST_SERIALIZATION_NVP( PointLights );
    ar & BOOST_SERIALIZATION_NVP( SpotLights );
    ar & BOOST_SERIALIZATION_NVP( DirectionalLights );

    // Light generation properties.
    ar & BOOST_SERIALIZATION_NVP( LightsMinBounds );
    ar & BOOST_SERIALIZATION_NVP( LightsMaxBounds );
    ar & BOOST_SERIALIZATION_NVP( MinSpotAngle );
    ar & BOOST_SERIALIZATION_NVP( MaxSpotAngle );
    ar & BOOST_SERIALIZATION_NVP( MinRange );
    ar & BOOST_SERIALIZATION_NVP( MaxRange );
    ar & BOOST_SERIALIZATION_NVP( NumPointLights );
    ar & BOOST_SERIALIZATION_NVP( NumSpotLights );
    ar & BOOST_SERIALIZATION_NVP( NumDirectionalLights );

    if ( version > 2 )
    {
        ar & BOOST_SERIALIZATION_NVP( LoadingProgressTotal );
    }
}