#pragma once

#include <Graphics/PointLight.h>
#include <Graphics/SpotLight.h>
#include <Graphics/DirectionalLight.h>

class ConfigurationSettings
{
public:
    ConfigurationSettings();

    uint16_t    WindowWidth;
    uint16_t    WindowHeight;
    bool        FullScreen;
    bool        VSync;
    bool        MultiSampleEnable;

    std::wstring SceneFileName;
    std::wstring LoadingScreenFileName;
    float       SceneScaleFactor;

    // Asset search paths.
    // Search paths are expressed relative to the configuration file.
    std::vector<std::wstring> SearchPaths;

    glm::vec3   CameraPosition;
    glm::quat   CameraRotation;
    float       CameraPivotDistance;

    // Lights
    std::vector<Graphics::PointLight> PointLights;
    std::vector<Graphics::SpotLight> SpotLights;
    std::vector<Graphics::DirectionalLight> DirectionalLights;

    // Light generation properties.
    glm::vec3       LightsMinBounds;
    glm::vec3       LightsMaxBounds;
    float           MinSpotAngle;
    float           MaxSpotAngle;
    float           MinRange;
    float           MaxRange;
    uint32_t        NumPointLights;
    uint32_t        NumSpotLights;
    uint32_t        NumDirectionalLights;

    float           LoadingProgressTotal;

    bool Load( const std::wstring& fileName );
    // Reload configuration settings from previously loaded file.
    bool Reload();
    bool Save( const std::wstring& fileName = L"" );

    std::vector<fs::path> GetAbsoluteSearchPaths() const;

protected:

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive& ar, const unsigned int version );

    // The file that is used to load/save this configuration.
    std::wstring m_Filename;

};

#include "ConfigurationSettings.inl"

BOOST_CLASS_VERSION( ConfigurationSettings, 5 );
