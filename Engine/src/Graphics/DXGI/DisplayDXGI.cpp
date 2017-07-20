#include <EnginePCH.h>

#include <Graphics/DXGI/DisplayDXGI.h>
#include <Graphics/DXGI/TextureFormatDXGI.h>

#include <LogManager.h>

using namespace Graphics;

DisplayDXGI::DisplayDXGI( Microsoft::WRL::ComPtr<IDXGIOutput4> output )
    : m_dxgiOutput( output )
{
    assert( m_dxgiOutput );

    if ( FAILED( m_dxgiOutput->GetDesc( &m_dxgiOutputDescription ) ) )
    {
        LOG_ERROR( "Failed to query output description." );
    }

    m_DisplayName = m_dxgiOutputDescription.DeviceName;
    RECT coords = m_dxgiOutputDescription.DesktopCoordinates;
    m_DesktopCoordinates = Rect( static_cast<float>( coords.left ),
                                 static_cast<float>( coords.top ),
                                 static_cast<float>( coords.right - coords.left ),
                                 static_cast<float>( coords.bottom - coords.top ) );

    switch ( m_dxgiOutputDescription.Rotation )
    {
    case DXGI_MODE_ROTATION_UNSPECIFIED:
    case DXGI_MODE_ROTATION_IDENTITY:
        m_Rotation = DisplayRotation::Rotate0;
        break;
    case DXGI_MODE_ROTATION_ROTATE90:
        m_Rotation = DisplayRotation::Rotate90;
        break;
    case DXGI_MODE_ROTATION_ROTATE180:
        m_Rotation = DisplayRotation::Rotate180;
        break;
    case DXGI_MODE_ROTATION_ROTATE270:
        m_Rotation = DisplayRotation::Rotate270;
        break;
    }
}

DisplayDXGI::~DisplayDXGI()
{

}

const std::wstring& DisplayDXGI::GetName() const
{
    return m_DisplayName;
}

const Rect& DisplayDXGI::GetDesktopCoordinates() const
{
    return m_DesktopCoordinates;
}

DisplayRotation DisplayDXGI::GetRotation() const
{
    return m_Rotation;
}

DisplayModeList DisplayDXGI::GetDisplayModes( const TextureFormat& textureFormat ) const
{
    DisplayModeList displayModes;

    DXGI_FORMAT dxgiFormat = ConvertTextureFormat( textureFormat );
    UINT numDisplayModes;
    UINT flags = DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING | DXGI_ENUM_MODES_STEREO;

    if ( FAILED( m_dxgiOutput->GetDisplayModeList1( dxgiFormat, flags, &numDisplayModes, nullptr ) ) )
    {
        LOG_ERROR( "Failed to query display modes." );
    }

    std::vector<DXGI_MODE_DESC1> dxgiDisplayModes(numDisplayModes);

    if ( FAILED( m_dxgiOutput->GetDisplayModeList1( dxgiFormat, flags, &numDisplayModes, dxgiDisplayModes.data() ) ) )
    {
        LOG_ERROR( "Failed to query display modes." );
    }
    
    for ( auto dxgiDisplayMode : dxgiDisplayModes )
    {
        DisplayMode displayMode;

        displayMode.Format = ConvertTextureFormat( dxgiDisplayMode.Format );
        displayMode.Width = dxgiDisplayMode.Width;
        displayMode.Height = dxgiDisplayMode.Height;
        displayMode.RefreshRate = dxgiDisplayMode.RefreshRate.Numerator / dxgiDisplayMode.RefreshRate.Denominator;
        displayMode.Stereo = dxgiDisplayMode.Stereo != 0;

        displayModes.push_back( displayMode );
    }

    return displayModes;
}

Microsoft::WRL::ComPtr<IDXGIOutput4> DisplayDXGI::GetDXGIOutput() const
{
    return m_dxgiOutput;
}
