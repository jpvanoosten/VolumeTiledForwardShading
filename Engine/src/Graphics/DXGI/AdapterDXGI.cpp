#include <EnginePCH.h>

#include <Graphics/DXGI/AdapterDXGI.h>
#include <Graphics/DXGI/DisplayDXGI.h>

#include <LogManager.h>

using namespace Microsoft::WRL;
using namespace Graphics;

inline DXGI_MEMORY_SEGMENT_GROUP TranslateVideoMemorySegmentGroup( VideoMemorySegment videoMemorySegment )
{
    DXGI_MEMORY_SEGMENT_GROUP dxgiMemorySegmentGroup;

    switch ( videoMemorySegment )
    {
    case VideoMemorySegment::Local:
        dxgiMemorySegmentGroup = DXGI_MEMORY_SEGMENT_GROUP_LOCAL;
        break;
    case VideoMemorySegment::NonLocal:
        dxgiMemorySegmentGroup = DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL;
        break;
    }

    return dxgiMemorySegmentGroup;
}

inline VideoMemoryUsage TranslateVideoMemoryInfo( const DXGI_QUERY_VIDEO_MEMORY_INFO& dxgiVideoMemoryInfo )
{
    VideoMemoryUsage videoMemoryUsage;

    videoMemoryUsage.Budget = dxgiVideoMemoryInfo.Budget;
    videoMemoryUsage.CurrentUsage = dxgiVideoMemoryInfo.CurrentReservation;
    videoMemoryUsage.AvailableForReservation = dxgiVideoMemoryInfo.AvailableForReservation;
    videoMemoryUsage.CurrentReservation = dxgiVideoMemoryInfo.CurrentReservation;

    return videoMemoryUsage;
}

AdapterDXGI::AdapterDXGI( Microsoft::WRL::ComPtr<IDXGIAdapter3> adapter )
    : m_dxgiAdapter( adapter )
{
    assert( m_dxgiAdapter != nullptr );
    if ( FAILED( m_dxgiAdapter->GetDesc2( &m_AdapterDescription ) ) )
    {
        LOG_ERROR( "Failed query adapter description." );
    }

    m_Description = m_AdapterDescription.Description;
    switch ( m_AdapterDescription.Flags )
    {
    case DXGI_ADAPTER_FLAG_SOFTWARE:
        m_Type = AdapterType::Software;
        break;
    default:
        m_Type = AdapterType::Hardware;
        break;
    }


    ComPtr<IDXGIOutput> dxgiOutput;
    for ( UINT i = 0; m_dxgiAdapter->EnumOutputs( i, &dxgiOutput ) != DXGI_ERROR_NOT_FOUND; ++i )
    {
        ComPtr<IDXGIOutput4> dxgiOutput4;
        if ( FAILED( dxgiOutput.As(&dxgiOutput4) ) )
        {
            LOG_ERROR( "Failed to query DXGIOutput4 interface." );
        }
        else
        {
            std::shared_ptr<DisplayDXGI> pDisplay = std::make_shared<DisplayDXGI>( dxgiOutput4 );
            m_Displays.push_back( pDisplay );
        }
    }
}

AdapterDXGI::~AdapterDXGI()
{
    // The DXGI adapter is held by a ComPtr. It will be released automatically.
}

const std::wstring& AdapterDXGI::GetDescription() const
{
    return m_Description;
}

Graphics::AdapterType AdapterDXGI::GetType() const
{
    return m_Type;
}

uint32_t AdapterDXGI::GetVendorID() const
{
    return m_AdapterDescription.VendorId;
}

uint32_t AdapterDXGI::GetDeviceID() const
{
    return m_AdapterDescription.DeviceId;
}

uint64_t AdapterDXGI::GetDedicatedVideoMemory() const
{
    return m_AdapterDescription.DedicatedVideoMemory;
}

uint64_t AdapterDXGI::GetDedicatedSystemMemory() const
{
    return m_AdapterDescription.DedicatedSystemMemory;
}

uint64_t AdapterDXGI::GetSharedSystemMemory() const
{
    return m_AdapterDescription.SharedSystemMemory;
}

VideoMemoryUsage AdapterDXGI::QueryVideoMemoryUsage( VideoMemorySegment videoMemorySegment ) const
{
    DXGI_MEMORY_SEGMENT_GROUP dxgiMemorySegmentGroup = TranslateVideoMemorySegmentGroup( videoMemorySegment );
    DXGI_QUERY_VIDEO_MEMORY_INFO dxgiQueryVideoMemoryInfo = {};

    if ( FAILED( m_dxgiAdapter->QueryVideoMemoryInfo( 0, dxgiMemorySegmentGroup, &dxgiQueryVideoMemoryInfo ) ) )
    {
        LOG_ERROR( "Failed to query video memory info." );
    }

    return TranslateVideoMemoryInfo( dxgiQueryVideoMemoryInfo );
}

void AdapterDXGI::SetVideoMemoryReservation( VideoMemorySegment videoMemorySegment, uint64_t bytes )
{
    DXGI_MEMORY_SEGMENT_GROUP dxgiMemorySegmentGroup = TranslateVideoMemorySegmentGroup( videoMemorySegment );
    
    if ( FAILED( m_dxgiAdapter->SetVideoMemoryReservation( 0, dxgiMemorySegmentGroup, bytes ) ) )
    {
        LOG_ERROR( "Failed to set video memory reservation." );
    }
}

const GraphicsDisplayList& AdapterDXGI::GetDisplays() const
{
    return m_Displays;
}

Microsoft::WRL::ComPtr<IDXGIAdapter3> AdapterDXGI::GetDXGIAdapter() const
{
    return m_dxgiAdapter;
}
