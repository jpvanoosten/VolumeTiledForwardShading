#pragma once

/**
 * Serialization helpers for boost serialization library.
 */

#include "Graphics/PointLight.h"
#include "Graphics/SpotLight.h"
#include "Graphics/DirectionalLight.h"

namespace boost
{
    namespace serialization
    {
        template<class Archive>
        void serialize( Archive& ar, glm::vec3& v, const unsigned int version )
        {
            ar & make_nvp( "X", v.x );
            ar & make_nvp( "Y", v.y );
            ar & make_nvp( "Z", v.z );
        }

        template<class Archive>
        void serialize( Archive& ar, glm::vec4& v, const unsigned int version )
        {
            ar & make_nvp( "X", v.x );
            ar & make_nvp( "Y", v.y );
            ar & make_nvp( "Z", v.z );
            ar & make_nvp( "W", v.w );
        }

        template<class Archive>
        void serialize( Archive& ar, glm::quat& q, const unsigned int version )
        {
            ar & make_nvp( "X", q.x );
            ar & make_nvp( "Y", q.y );
            ar & make_nvp( "Z", q.z );
            ar & make_nvp( "W", q.w );
        }

        template<class Archive>
        void serialize( Archive& ar, Graphics::PointLight& light, const unsigned int version )
        {
            ar & make_nvp( "Position", light.m_PositionWS );
            ar & make_nvp( "Color", light.m_Color );
            ar & make_nvp( "Range", light.m_Range );
            ar & make_nvp( "Intensity", light.m_Intensity );
            ar & make_nvp( "Enabled", light.m_Enabled );
        }

        template<class Archive>
        void serialize( Archive& ar, Graphics::SpotLight& light, const unsigned int version )
        {
            ar & make_nvp( "Position", light.m_PositionWS );
            ar & make_nvp( "Direction", light.m_DirectionWS );
            ar & make_nvp( "Color", light.m_Color );
            ar & make_nvp( "SpotlightAngle", light.m_SpotlightAngle );
            ar & make_nvp( "Range", light.m_Range );
            ar & make_nvp( "Intensity", light.m_Intensity );
            ar & make_nvp( "Enabled", light.m_Enabled );
        }

        template<class Archive>
        void serialize( Archive& ar, Graphics::DirectionalLight& light, const unsigned int version )
        {
            ar & make_nvp( "Direction", light.m_DirectionWS );
            ar & make_nvp( "Color", light.m_Color );
            ar & make_nvp( "Intensity", light.m_Intensity );
            ar & make_nvp( "Enabled", light.m_Enabled );
        }
    }
}

BOOST_CLASS_VERSION( Graphics::PointLight, 0 );
BOOST_CLASS_VERSION( Graphics::SpotLight, 0 );
BOOST_CLASS_VERSION( Graphics::DirectionalLight, 0 );

