#include <EnginePCH.h>

#include <Graphics/DX12/ApplicationDX12.h>
#include <Graphics/DXGI/AdapterDXGI.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/WindowDX12.h>
#include <Graphics/DX12/GraphicsCommandQueueDX12.h>
#include <Graphics/DX12/TextureDX12.h>

#include <Graphics/Profiler.h>

#include <HighResolutionTimer.h>
#include <ThreadSafeQueue.h>
#include <LogManager.h>

#include "../resource.h"

// Constants
constexpr auto RENDER_WINDOW_CLASS_NAME = "RenderWindowDX12Class";

// Statics
using WindowHandleMap = std::map<HWND, std::shared_ptr<Graphics::WindowDX12>>;
static WindowHandleMap gs_WindowHandles;
std::mutex gs_WindowHandlesMutex;

// For debug capturing.
Microsoft::WRL::ComPtr<IDXGraphicsAnalysis> gs_dxGraphicsAnalysis;

// For joystick handling.
XINPUT_STATE gs_PreviousInputState[XUSER_MAX_COUNT];
// If a joystick is not connected, don't poll it again for some duration.
double g_JoystickPollingTimeouts[XUSER_MAX_COUNT] = {};

using MessageFunc = std::function<void()>;
Core::ThreadSafeQueue< MessageFunc > gs_MessageQueue;

// Defined in DLLMain.cpp
extern HINSTANCE g_DLLHandle;

// Global Window Procedure callback function
static LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

using namespace std::chrono;
using namespace Microsoft::WRL;
using namespace Graphics;
using namespace Core;

#if defined(PROFILE)
const DWORD MS_VC_EXCEPTION = 0x406D1388;

// Set the name of a running thread (for debugging)
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName( std::thread& thread, const char* threadName )
{
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = threadName;
    info.dwThreadID = GetThreadId( reinterpret_cast<HANDLE>( thread.native_handle() ) );
    info.dwFlags = 0;

    __try
    {
        RaiseException( MS_VC_EXCEPTION, 0, sizeof( info ) / sizeof( ULONG_PTR ), (ULONG_PTR*)&info );
    } __except ( EXCEPTION_EXECUTE_HANDLER )
    {}
}
#endif

// FreeImage error handler.
static void FreeImageErrorHandler( FREE_IMAGE_FORMAT fif, const char* message )
{
    LogManager::LogError( message );
}



// Assimp error log stream
class ErrorLogStream : public Assimp::LogStream
{
public:

    virtual void write( const char* message )
    {
        // Remove the carriage return at the end of the Assimp message.
        // The log manager appends this automatically anyways to all logged messages.
        size_t msgLen = strnlen_s(message, 256);
        char msg[256];
        strncpy_s( msg, message, msgLen - 1 );
        LogManager::LogError( "ASSIMP: ", msg );
    }
};

// Assimp error log stream
class WarnLogStream : public Assimp::LogStream
{
public:

    virtual void write( const char* message )
    {
        // Remove the carriage return at the end of the Assimp message.
        // The log manager appends this automatically anyways to all logged messages.
        size_t msgLen = strnlen_s( message, 256 );
        char msg[256];
        strncpy_s( msg, message, msgLen - 1 );
        LogManager::LogWarning( "ASSIMP: ", msg );
    }
};

// Assimp error log stream
class InfoLogStream : public Assimp::LogStream
{
public:

    virtual void write( const char* message )
    {
        // Remove the carriage return at the end of the Assimp message.
        // The log manager appends this automatically anyways to all logged messages.
        size_t msgLen = strnlen_s( message, 256 );
        char msg[256];
        strncpy_s( msg, message, msgLen - 1 );
        LogManager::LogInfo( "ASSIMP: ", msg );
    }
};

ApplicationDX12::ApplicationDX12()
    : m_bIsRunning( false )
    , m_RequestQuit( false )
    , m_UpdateFrame( 0 )
    , m_RenderFrame( 0 )
{
    m_hInstance = g_DLLHandle;
    if ( !m_hInstance )
    {
        m_hInstance = ::GetModuleHandle( NULL );
    }

    // Create an asset importer log file.
    Assimp::Logger::LogSeverity logSeverity = Assimp::Logger::NORMAL;
    unsigned int logStreams = aiDefaultLogStream_FILE;

#if defined(_DEBUG)
    logSeverity = Assimp::Logger::VERBOSE;
    logStreams |= aiDefaultLogStream_DEBUGGER | aiDefaultLogStream_STDOUT;
#endif

    Assimp::DefaultLogger::create( "", logSeverity, 0 );
    Assimp::DefaultLogger::get()->attachStream( new WarnLogStream(), Assimp::Logger::Err ); // Display Assimp errors as warnings as these are probably not worth displaying a error dialog message for.
    Assimp::DefaultLogger::get()->attachStream( new WarnLogStream(), Assimp::Logger::Warn );
    Assimp::DefaultLogger::get()->attachStream( new InfoLogStream(), Assimp::Logger::Info );

    // Initialize FreeImage and register error handler.
    FreeImage_Initialise();
    FreeImage_SetOutputMessage( &FreeImageErrorHandler );

    ZeroMemory( gs_PreviousInputState, sizeof( gs_PreviousInputState ) );

    // Register a window class for creating our render windows with.
    WNDCLASSEX renderWindowClass;

    renderWindowClass.cbSize = sizeof( WNDCLASSEX );
    renderWindowClass.style = CS_HREDRAW | CS_VREDRAW;
    renderWindowClass.lpfnWndProc = &WndProc;
    renderWindowClass.cbClsExtra = 0;
    renderWindowClass.cbWndExtra = 0;
    renderWindowClass.hInstance = m_hInstance;
    renderWindowClass.hIcon = ::LoadIcon( m_hInstance, MAKEINTRESOURCE( APPLICATION_ICON ) );
    renderWindowClass.hCursor = ::LoadCursor( NULL, IDC_ARROW );
    renderWindowClass.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
    renderWindowClass.lpszMenuName = NULL;
    renderWindowClass.lpszClassName = RENDER_WINDOW_CLASS_NAME;
    renderWindowClass.hIconSm = ::LoadIcon( m_hInstance, MAKEINTRESOURCE( APPLICATION_ICON ) );

    if ( !::RegisterClassEx( &renderWindowClass ) )
    {
        LOG_ERROR( "Failed to register the render window class." );
    }

#if defined(_DEBUG)
    ComPtr<ID3D12Debug> debugInterface;
    if ( FAILED( D3D12GetDebugInterface( IID_PPV_ARGS( &debugInterface ) ) ) )
    {
        LOG_ERROR( "Failed to retrieve D3D12 debug interface." );
    }
    else
    {
        debugInterface->EnableDebugLayer();
    }
#endif

    if ( FAILED( DXGIGetDebugInterface1( 0, IID_PPV_ARGS( &gs_dxGraphicsAnalysis ) ) ) )
    {
        LOG_ERROR( "Failed to get graphics analysis debug interface. Make sure you are running in graphics diagonstic mode (Alt+F5 in Visual Studio)" );
    }
}

ApplicationDX12::~ApplicationDX12()
{
    FreeImage_DeInitialise();

    Assimp::DefaultLogger::kill();

    gs_WindowHandles.clear();

    if ( !::UnregisterClass( RENDER_WINDOW_CLASS_NAME, m_hInstance ) )
    {
        LOG_ERROR( "Failed to unregister render window class" );
    }

    ImGui::Shutdown();

    gs_dxGraphicsAnalysis.Reset();

    m_Adapters.clear();

#if defined(_DEBUG)
    ComPtr<IDXGIDebug> dxgiDebug;
    DXGIGetDebugInterface1( 0, IID_PPV_ARGS( &dxgiDebug ) );
    if ( dxgiDebug )
    {
        dxgiDebug->ReportLiveObjects( DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL );
    }
#endif
}

int32_t ApplicationDX12::Run()
{
    assert( !m_bIsRunning );
    m_bIsRunning = true;

    std::thread updateThread = std::thread( &ApplicationDX12::UpdateThread, this );

#if defined(PROFILE)
    SetThreadName( updateThread, "Update" );
#endif

    HighResolutionTimer timer;
    double totalTime = 0.0f;

    MSG msg = {};
    while ( msg.message != WM_QUIT )
    {
        // Wait till the message queue empties.
        //std::unique_lock<std::mutex> msgQueueLock( m_MessageQueueMutex );
        //m_MessageQueueConditionVar.wait( msgQueueLock, [] { return gs_MessageQueue.Empty(); } );

        ImGuiIO& io = ImGui::GetIO();

        if ( io.MouseDrawCursor != io.WantCaptureMouse )
        {
            io.MouseDrawCursor = io.WantCaptureMouse;
            // Hide OS mouse cursor if ImGui is drawing it
            ShowCursor( io.MouseDrawCursor ? FALSE : TRUE );
        }

        // Check to see of the application wants to quit.
        if ( m_RequestQuit )
        {
            ::PostQuitMessage( 0 );
            m_RequestQuit = false;
        }

        while ( ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            ::TranslateMessage( &msg );
            ::DispatchMessage( &msg );
        }

        std::this_thread::yield();
    }

    m_bIsRunning = false;

    if ( updateThread.joinable() )
    {
        updateThread.join();
    }

    EventArgs eventArgs( *this );
    OnExit( eventArgs );

    return static_cast<int32_t>( msg.wParam );
}

void ApplicationDX12::Stop()
{
    // When called from another thread other than the main thread,
    // the WM_QUIT message goes to that thread and will not be handled 
    // in the main thread. To circumvent this, we also set a boolean flag
    // to indicate that the user has requested to quit the application.
    m_RequestQuit = true;
}

const AdapterList& ApplicationDX12::GetAdapters() const
{
    m_Adapters.clear();

    ComPtr<IDXGIFactory4> dxgiFactory;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    if ( FAILED( CreateDXGIFactory2( createFactoryFlags, IID_PPV_ARGS( &dxgiFactory ) ) ) )
    {
        LOG_ERROR( "Failed to create DXGIFactory." );
    }
    else
    {
        ComPtr<IDXGIAdapter1> dxgiAdapter1;
        for ( UINT i = 0; dxgiFactory->EnumAdapters1( i, &dxgiAdapter1 ) != DXGI_ERROR_NOT_FOUND; ++i )
        {
            ComPtr<IDXGIAdapter3> dxgiAdapter3;
            if ( FAILED( dxgiAdapter1.As( &dxgiAdapter3 ) ) )
            {
                LOG_ERROR( "Could not query IDXGIAdapter3 interface." );
            }
            else
            {
                DXGI_ADAPTER_DESC2 dxgiDesc2 = {};
                if ( FAILED( dxgiAdapter3->GetDesc2( &dxgiDesc2 ) ) )
                {
                    LOG_ERROR( "Failed to query dxgi description." );
                }
                else
                {
                    Microsoft::WRL::ComPtr<ID3D12Device> pDevice;
                    if ( dxgiDesc2.Flags != DXGI_ADAPTER_FLAG_SOFTWARE && 
                         SUCCEEDED( D3D12CreateDevice( dxgiAdapter3.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice) ) ) )
                    {
                        std::shared_ptr<Graphics::AdapterDXGI> graphicsAdapter = std::make_shared<Graphics::AdapterDXGI>( dxgiAdapter3 );
                        m_Adapters.push_back( std::static_pointer_cast<Graphics::Adapter>( graphicsAdapter ) );
                    }
                }
            }
        }

        // Sort adapters based on the amount of dedicated video memory.
        // It is commonly accepted that adapters with more dedicated video memory 
        // are a better choice if there are multiple adapters in a system.
        std::sort( m_Adapters.begin(), m_Adapters.end(), [] ( std::shared_ptr<Adapter> a, std::shared_ptr<Adapter> b )
        {
            return a->GetDedicatedVideoMemory() >= b->GetDedicatedVideoMemory();
        } );
    }

    return m_Adapters;
}

std::shared_ptr<Graphics::Adapter> ApplicationDX12::GetWarpAdapter() const
{
    std::shared_ptr<Graphics::AdapterDXGI> graphicsAdapter;

    ComPtr<IDXGIFactory4> dxgiFactory;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    if ( FAILED( CreateDXGIFactory2( createFactoryFlags, IID_PPV_ARGS( &dxgiFactory ) ) ) )
    {
        LOG_ERROR( "Failed to create DXGIFactory." );
    }
    else
    {
        ComPtr<IDXGIAdapter1> dxgiAdapter1;
        if ( FAILED( dxgiFactory->EnumWarpAdapter( IID_PPV_ARGS( &dxgiAdapter1 ) ) ) )
        {
            LOG_ERROR( "Failed to enumerate warp adapter." );
        }
        else
        {
            ComPtr<IDXGIAdapter3> dxgiAdapter3;
            if ( FAILED( dxgiAdapter1.As( &dxgiAdapter3 ) ) )
            {
                LOG_ERROR( "Could not query IDXGIAdapter3 interface." );
            }
            else
            {
                graphicsAdapter = std::make_shared<Graphics::AdapterDXGI>( dxgiAdapter3 );
            }
        }
    }

    return graphicsAdapter;
}


std::shared_ptr<Graphics::Device> ApplicationDX12::CreateDevice( std::shared_ptr<Graphics::Adapter> _adapter )
{
    std::shared_ptr<Graphics::DeviceDX12> renderDevice;
    std::shared_ptr<Graphics::AdapterDXGI> adapter = std::dynamic_pointer_cast<Graphics::AdapterDXGI>( _adapter );

    ComPtr<IDXGIAdapter3> dxgiAdapter;
    if ( !adapter )
    {
        const AdapterList& adapters = GetAdapters();
        if ( !adapters.empty() )
        {
            adapter = std::dynamic_pointer_cast<Graphics::AdapterDXGI>( adapters[0] );
        }
        else
        {
            LOG_WARNING( "No valid adapters found. Falling back to WARP adapter." );
            adapter = std::dynamic_pointer_cast<Graphics::AdapterDXGI>( GetWarpAdapter() );
        }
    }

    assert( adapter );
    dxgiAdapter = adapter->GetDXGIAdapter();
    
    ComPtr<ID3D12Device> d3d12Device;
    if ( FAILED( D3D12CreateDevice( dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &d3d12Device ) ) ) )
    {
        std::wstringstream ss;
        ss << "Failed to create D3D12 device using [";
        if ( adapter )
        {
            ss << adapter->GetDescription() << "]";
        }
        else
        {
            ss << "NULL]";
        }

        ss << " adapter.";

        LOG_ERROR( ss.str() );

        return nullptr;
    }
    else
    {
#if defined(_DEBUG)
        ComPtr<ID3D12InfoQueue> pInfoQueue = nullptr;
        if ( SUCCEEDED( d3d12Device.As( &pInfoQueue ) ) )
        {
            pInfoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE );
            pInfoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_ERROR, TRUE );
            pInfoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_WARNING, TRUE );

            // Suppress whole categories of messages
            //D3D12_MESSAGE_CATEGORY Categories[] = {};

            // Suppress messages based on their severity level
            D3D12_MESSAGE_SEVERITY Severities[] =
            {
                D3D12_MESSAGE_SEVERITY_INFO
            };

            // Suppress individual messages by their ID
            D3D12_MESSAGE_ID DenyIds[] = {
                D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
                D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
                D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
                D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,                     // This happens if an invalid descriptor is copied to a descriptor heap. This error occurs even if the shader never access the descriptor.
                (D3D12_MESSAGE_ID)1008,                                         // RESOURCE_BARRIER_DUPLICATE_SUBRESOURCE_TRANSITIONS -- There is no define for this message ID. I guess it was added in a later SDK. It happens if a resource is transitioned multiple times in a single resource barrier. This happens when generating mipmaps.
            };

            D3D12_INFO_QUEUE_FILTER NewFilter = {};
            //NewFilter.DenyList.NumCategories = _countof(Categories);
            //NewFilter.DenyList.pCategoryList = Categories;
            NewFilter.DenyList.NumSeverities = _countof( Severities );
            NewFilter.DenyList.pSeverityList = Severities;
            NewFilter.DenyList.NumIDs = _countof( DenyIds );
            NewFilter.DenyList.pIDList = DenyIds;

            pInfoQueue->PushStorageFilter( &NewFilter );
        }

        //if ( FAILED( d3d12Device->SetStablePowerState( TRUE ) ) )
        //{
        //    LOG_ERROR( "Failed to set stable power state." );
        //}
#endif

        renderDevice = std::make_shared<Graphics::DeviceDX12>( d3d12Device, adapter );
        renderDevice->Init();
    }
    
    return renderDevice;
}

std::shared_ptr<Graphics::Window> ApplicationDX12::CreateWindow( std::shared_ptr<Graphics::Device> device, 
                                                                 const std::string& title, 
                                                                 uint32_t _width, uint32_t _height, 
                                                                 bool fullscreen, bool vsync, 
                                                                 const Graphics::TextureFormat& colorFormat,
                                                                 const Graphics::TextureFormat& depthStencilFormat)
{
    int screenWidth = ::GetSystemMetrics( SM_CXSCREEN );
    int screenHeight = ::GetSystemMetrics( SM_CYSCREEN );

    RECT windowRect = { 0, 0, static_cast<LONG>(_width), static_cast<LONG>(_height) };

    ::AdjustWindowRect( &windowRect, WS_OVERLAPPEDWINDOW, FALSE );

    uint32_t width = windowRect.right - windowRect.left;
    uint32_t height = windowRect.bottom - windowRect.top;

    int windowX = std::max<int>( 0, ( screenWidth - (int)width ) / 2 );
    int windowY = std::max<int>( 0, ( screenHeight - (int)height ) / 2 );

    HWND hWindow = ::CreateWindowEx( NULL,
                                   RENDER_WINDOW_CLASS_NAME,
                                   title.c_str(),
                                   WS_OVERLAPPEDWINDOW,
                                   windowX,
                                   windowY,
                                   width,
                                   height,
                                   NULL,
                                   NULL,
                                   m_hInstance,
                                   NULL );

    if ( !hWindow )
    {
        LOG_ERROR( "Failed to create render window." );
        return nullptr;
    }

    TRACKMOUSEEVENT trackMouseEvent = {};
    trackMouseEvent.cbSize = sizeof( TRACKMOUSEEVENT );
    trackMouseEvent.hwndTrack = hWindow;
    trackMouseEvent.dwFlags = TME_LEAVE;
    TrackMouseEvent( &trackMouseEvent );

    std::shared_ptr<DeviceDX12> deviceDX12 = std::dynamic_pointer_cast<DeviceDX12>( device );
    assert( deviceDX12 );

    std::shared_ptr<Graphics::WindowDX12> pRenderWindow;
    pRenderWindow = std::make_shared<Graphics::WindowDX12>( *this, deviceDX12, hWindow, title, _width, _height, fullscreen, vsync, colorFormat, depthStencilFormat );

    gs_WindowHandles.insert( WindowHandleMap::value_type( hWindow, pRenderWindow ) );

    ::UpdateWindow( hWindow );

    return pRenderWindow;
}

void ApplicationDX12::ProcessMessages()
{
    CPU_MARKER( __FUNCTION__ );

    MessageFunc f;
    while ( gs_MessageQueue.TryPop( f ) )
    {
        f();
    }
    m_MessageQueueConditionVar.notify_one();
}

void ApplicationDX12::UpdateThread()
{
    HighResolutionTimer timer;

    double totalTime = 0.0f;
    
    Profiler& profiler = Profiler::Get();

    while ( m_bIsRunning )
    {
        CPU_MARKER( __FUNCTION__ );

        timer.Tick();

        double elapsedTime = timer.ElapsedSeconds();
        totalTime += elapsedTime;

        ProcessJoysticks( elapsedTime );
        ProcessMessages();

        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = static_cast<float>( elapsedTime );

        ImGui::NewFrame();

        Core::UpdateEventArgs updateEventArgs( *this, elapsedTime, totalTime, m_UpdateFrame );
        OnUpdate( updateEventArgs );
        Core::RenderEventArgs renderEventArgs( *this, elapsedTime, totalTime, m_UpdateFrame );
        OnRender( renderEventArgs );

        ++m_UpdateFrame;

        if ( !profiler.IsPaused() )
        {
            profiler.SetCurrentFrame( m_UpdateFrame );
        }

        std::this_thread::yield();
    }
}

// Translate the XInput button IDs to Joystick button IDs.
unsigned int TranslateButtonID( unsigned int xInputButtonID )
{
    unsigned int buttonID = 0;
    switch ( xInputButtonID )
    {
    case XINPUT_GAMEPAD_A:
        buttonID = 1;
        break;
    case XINPUT_GAMEPAD_B:
        buttonID = 2;
        break;
    case XINPUT_GAMEPAD_X:
        buttonID = 3;
        break;
    case XINPUT_GAMEPAD_Y:
        buttonID = 4;
        break;
    case XINPUT_GAMEPAD_LEFT_SHOULDER:
        buttonID = 5;
        break;
    case XINPUT_GAMEPAD_RIGHT_SHOULDER:
        buttonID = 6;
        break;
    case XINPUT_GAMEPAD_BACK:
        buttonID = 7;
        break;
    case XINPUT_GAMEPAD_START:
        buttonID = 8;
        break;
    case XINPUT_GAMEPAD_LEFT_THUMB:
        buttonID = 9;
        break;
    case XINPUT_GAMEPAD_RIGHT_THUMB:
        buttonID = 10;
        break;
    }

    return buttonID;
}

float FilterAnalogInput( int val, int deadZone )
{
    if ( val < 0 )
    {
        if ( val > -deadZone )
            return 0.0f;
        else
            return ( val + deadZone ) / ( 32768.0f - deadZone );
    }
    else
    {
        if ( val < deadZone )
            return 0.0f;
        else
            return ( val - deadZone ) / ( 32767.0f - deadZone );
    }
}

void ApplicationDX12::ProcessJoysticks( double deltaTime )
{
    CPU_MARKER( __FUNCTION__ );

    // If a joystick is not connected, don't poll it again for N seconds.
    static const double POLLING_TIMEOUT = 5.0;

    for ( DWORD id = 0; id < XUSER_MAX_COUNT; ++id )
    {
        // If we are still waiting for the polling timeout, skip this input.
        g_JoystickPollingTimeouts[id] -= deltaTime;
        if ( g_JoystickPollingTimeouts[id] > 0.0 ) continue;

        const XINPUT_STATE& previousInputState = gs_PreviousInputState[id];
        XINPUT_STATE currentInputState;

        if ( XInputGetState( id, &currentInputState ) == ERROR_DEVICE_NOT_CONNECTED )
        {
            g_JoystickPollingTimeouts[id] = POLLING_TIMEOUT;
            continue;
        }

        // If the packet numbers are the same, then no changes have been detected.
        if ( previousInputState.dwPacketNumber == currentInputState.dwPacketNumber )
        {
            continue;
        }

        bool buttonStates[32] = {};
        int i;
        WORD buttonMask;
        for ( i = 0, buttonMask = 1; i < 16; ++i, buttonMask <<= 1 )
        {
            buttonStates[i] = ( currentInputState.Gamepad.wButtons & buttonMask ) != 0;
        }

        WORD buttonsPressed = currentInputState.Gamepad.wButtons & ~previousInputState.Gamepad.wButtons;
        WORD buttonsHeld = currentInputState.Gamepad.wButtons & previousInputState.Gamepad.wButtons;
        WORD buttonsReleased = ~currentInputState.Gamepad.wButtons & previousInputState.Gamepad.wButtons;

        // Fire an event for every button that is now "pressed"
        for ( i = 1, buttonMask = XINPUT_GAMEPAD_START; i <= 10; ++i, buttonMask <<= 1 )
        {
            if ( ( buttonsPressed & buttonMask ) != 0 )
            {
                JoystickButtonEventArgs joyEventArgs( *this, id, ButtonState::Pressed, TranslateButtonID( buttonMask ), buttonStates );
                gs_MessageQueue.Push( std::bind( &ApplicationDX12::OnJoystickButtonPressed, this, joyEventArgs ) );
            }
        }

        // Fire an event for every button that is now "released"
        for ( i = 1, buttonMask = XINPUT_GAMEPAD_START; i <= 10; ++i, buttonMask <<= 1 )
        {
            if ( ( buttonsReleased & buttonMask ) != 0 )
            {
                JoystickButtonEventArgs joyEventArgs( *this, id, ButtonState::Released, TranslateButtonID( buttonMask ), buttonStates );
                gs_MessageQueue.Push( std::bind( &ApplicationDX12::OnJoystickButtonReleased, this, joyEventArgs ) );
            }
        }

        static const WORD povButtonMask = XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_DOWN | XINPUT_GAMEPAD_DPAD_LEFT | XINPUT_GAMEPAD_DPAD_RIGHT;
        WORD previousPOV = previousInputState.Gamepad.wButtons & povButtonMask;
        WORD currentPOV = currentInputState.Gamepad.wButtons & povButtonMask;

        // Check Point of View hat.
        if ( previousPOV != currentPOV )
        {
            POVDirection povDir = POVDirection::Centered;
            float angle = -1.0f;

            if ( ( currentPOV & povButtonMask ) != 0 )
            {
                float x = ( ( currentPOV & XINPUT_GAMEPAD_DPAD_UP ) != 0 ? 1.0f : 0.0f ) - ( ( currentPOV & XINPUT_GAMEPAD_DPAD_DOWN ) != 0 ? 1.0f : 0.0f );
                float y = ( ( currentPOV & XINPUT_GAMEPAD_DPAD_RIGHT ) != 0 ? 1.0f : 0.0f ) - ( ( currentPOV & XINPUT_GAMEPAD_DPAD_LEFT ) != 0 ? 1.0f : 0.0f );

                angle = atan2( y, x );
                if ( angle < 0.0f )
                {
                    angle += Math::_2PI;
                }

                // Convert to degrees.
                angle = Math::Degrees( angle );
                // Convert to discrete angle.
                povDir = static_cast<POVDirection>( std::lroundf( ( angle / 360.0f ) * 8.0f ) * 45 );
            }

            JoystickPOVEventArgs joyPoVEventArgs( *this, id, angle, povDir, buttonStates );
            gs_MessageQueue.Push( std::bind( &ApplicationDX12::OnJoystickPOV, this, joyPoVEventArgs ) );

        }

        // Check left trigger
        if ( previousInputState.Gamepad.bLeftTrigger != currentInputState.Gamepad.bLeftTrigger )
        {
            float axis = Math::NormalizeRange<float, BYTE>( currentInputState.Gamepad.bLeftTrigger, 0, 255 );
            JoystickAxisEventArgs joyAxisEventArgs( *this, id, JoystickAxis::ZAxis, axis, buttonStates );
            gs_MessageQueue.Push( std::bind( &ApplicationDX12::OnJoystickAxis, this, joyAxisEventArgs ) );
        }

        // Check right trigger
        if ( previousInputState.Gamepad.bRightTrigger != currentInputState.Gamepad.bRightTrigger )
        {
            float axis = Math::NormalizeRange<float, BYTE>( currentInputState.Gamepad.bRightTrigger, 0, 255 );
            JoystickAxisEventArgs joyAxisEventArgs( *this, id, JoystickAxis::RAxis, axis, buttonStates );
            gs_MessageQueue.Push( std::bind( &ApplicationDX12::OnJoystickAxis, this, joyAxisEventArgs ) );
        }

        // Check left X thumb stick.
        if ( previousInputState.Gamepad.sThumbLX != currentInputState.Gamepad.sThumbLX )
        {
            float axis = FilterAnalogInput( currentInputState.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE );
            JoystickAxisEventArgs joyAxisEventArgs( *this, id, JoystickAxis::XAxis, axis, buttonStates );
            gs_MessageQueue.Push( std::bind( &ApplicationDX12::OnJoystickAxis, this, joyAxisEventArgs ) );
        }

        // Check left Y thumb stick.
        if ( previousInputState.Gamepad.sThumbLY != currentInputState.Gamepad.sThumbLY )
        {
            float axis = FilterAnalogInput( currentInputState.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE );
            JoystickAxisEventArgs joyAxisEventArgs( *this, id, JoystickAxis::YAxis, axis, buttonStates );
            gs_MessageQueue.Push( std::bind( &ApplicationDX12::OnJoystickAxis, this, joyAxisEventArgs ) );
        }

        // Check right X thumb stick.
        if ( previousInputState.Gamepad.sThumbRX != currentInputState.Gamepad.sThumbRX )
        {
            float axis = FilterAnalogInput( currentInputState.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE );
            JoystickAxisEventArgs joyAxisEventArgs( *this, id, JoystickAxis::UAxis, axis, buttonStates );
            gs_MessageQueue.Push( std::bind( &ApplicationDX12::OnJoystickAxis, this, joyAxisEventArgs ) );
        }

        // Check right Y thumb stick.
        if ( previousInputState.Gamepad.sThumbRY != currentInputState.Gamepad.sThumbRY )
        {
            float axis = FilterAnalogInput( currentInputState.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE );
            JoystickAxisEventArgs joyAxisEventArgs( *this, id, JoystickAxis::VAxis, axis, buttonStates );
            gs_MessageQueue.Push( std::bind( &ApplicationDX12::OnJoystickAxis, this, joyAxisEventArgs ) );
        }

        gs_PreviousInputState[id] = currentInputState;
    }
}

void ApplicationDX12::OnRender( RenderEventArgs& e )
{
    CPU_MARKER( __FUNCTION__ );
    std::lock_guard<std::mutex> lock( gs_WindowHandlesMutex );
    for ( auto window : gs_WindowHandles )
    {
        std::shared_ptr<WindowDX12> pWindow = window.second;
        pWindow->OnPreRender( e );
        pWindow->OnRender( e );
        pWindow->OnPostRender( e );
    }
}

// The user requested to exit the application
void ApplicationDX12::OnExit( EventArgs& e )
{
    base::OnExit( e );

    // Destroy any windows that are still hanging around.
    while ( !gs_WindowHandles.empty() )
    {
        ::DestroyWindow( ( *gs_WindowHandles.begin() ).first );
    }
}

void ApplicationDX12::BeginGraphicsAnalysis()
{
    if ( gs_dxGraphicsAnalysis )
    {
        gs_dxGraphicsAnalysis->BeginCapture();
    }
}

void ApplicationDX12::EndGraphicsAnalysis()
{
    if ( gs_dxGraphicsAnalysis )
    {
        gs_dxGraphicsAnalysis->EndCapture();
    }
}

// Convert the message ID into a MouseButton ID
static MouseButton DecodeMouseButton( UINT messageID )
{
    MouseButton mouseButton = MouseButton::None;
    switch ( messageID )
    {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    {
        mouseButton = MouseButton::Left;
    }
    break;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    {
        mouseButton = MouseButton::Right;
    }
    break;
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
    {
        mouseButton = MouseButton::Middel;
    }
    break;
    }

    return mouseButton;
}

// Convert the message ID into a ButtonState.
static ButtonState DecodeButtonState( UINT messageID )
{
    ButtonState buttonState;
    switch ( messageID )
    {
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
    {
        buttonState = ButtonState::Released;
    }
    break;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
    {
        buttonState = ButtonState::Pressed;
    }
    break;
    }

    return buttonState;
}

static LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    std::shared_ptr<Window> pRenderWindow;

    // This is scoped here otherwise the iterator gets invalidated 
    // if a window is destructed while we are in this message loop.
    {
        WindowHandleMap::const_iterator iter = gs_WindowHandles.find( hwnd );
        if ( iter != gs_WindowHandles.end() )
        {
            pRenderWindow = iter->second;
        }
    }

    if ( pRenderWindow )
    {
        switch ( message )
        {
        case WM_PAINT:
        {
            PAINTSTRUCT paintStruct;
            HDC hDC;

            hDC = BeginPaint( hwnd, &paintStruct );
            EndPaint( hwnd, &paintStruct );
        }
        break;
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        {
            MSG charMsg;

            // Get the Unicode character (UTF-16)
            unsigned int c = 0;
            // For printable characters, the next message will be WM_CHAR.
            // This message contains the character code we need to send the KeyPressed event.
            // Inspired by the SDL 1.2 implementation.
            if ( PeekMessage( &charMsg, hwnd, 0, 0, PM_NOREMOVE ) && charMsg.message == WM_CHAR )
            {
                GetMessage( &charMsg, hwnd, 0, 0 );
                c = static_cast<unsigned int>( charMsg.wParam );
            }

            bool shift = ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) != 0;
            bool control = ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) != 0;
            bool alt = ( GetAsyncKeyState( VK_MENU ) & 0x8000 ) != 0;

            KeyCode key = (KeyCode)wParam;
            unsigned int scanCode = ( lParam & 0x00FF0000 ) >> 16;
            KeyEventArgs keyEventArgs( *pRenderWindow, key, c, KeyState::Pressed, control, shift, alt );
            gs_MessageQueue.Push( std::bind( &Window::OnKeyPressed, pRenderWindow.get(), keyEventArgs ) );
        }
        break;
        case WM_SYSKEYUP:
        case WM_KEYUP:
        {
            bool shift = ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) != 0;
            bool control = ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) != 0;
            bool alt = ( GetAsyncKeyState( VK_MENU ) & 0x8000 ) != 0;

            KeyCode key = (KeyCode)wParam;
            unsigned int c = 0;
            unsigned int scanCode = ( lParam & 0x00FF0000 ) >> 16;

            // Determine which key was released by converting the key code and the scan code
            // to a printable character (if possible).
            // Inspired by the SDL 1.2 implementation.
            unsigned char keyboardState[256];
            GetKeyboardState( keyboardState );
            wchar_t translatedCharacters[4];
            if ( int result = ToUnicodeEx( (UINT)wParam, scanCode, keyboardState, translatedCharacters, 4, 0, NULL ) > 0 )
            {
                c = translatedCharacters[0];
            }

            KeyEventArgs keyEventArgs( *pRenderWindow, key, c, KeyState::Released, control, shift, alt );
            gs_MessageQueue.Push( std::bind( &Window::OnKeyReleased, pRenderWindow.get(), keyEventArgs ) );
        }
        break;
        case WM_KILLFOCUS:
        {
            // Window lost keyboard focus.
            EventArgs eventArgs( *pRenderWindow );
            gs_MessageQueue.Push( std::bind( &Window::OnKeyboardBlur, pRenderWindow.get(), eventArgs ) );
        }
        break;
        case WM_SETFOCUS:
        {
            EventArgs eventArgs( *pRenderWindow );
            gs_MessageQueue.Push( std::bind( &Window::OnKeyboardFocus, pRenderWindow.get(), eventArgs ) );
        }
        break;
        case WM_MOUSEMOVE:
        {
            bool lButton = ( wParam & MK_LBUTTON ) != 0;
            bool rButton = ( wParam & MK_RBUTTON ) != 0;
            bool mButton = ( wParam & MK_MBUTTON ) != 0;
            bool shift = ( wParam & MK_SHIFT ) != 0;
            bool control = ( wParam & MK_CONTROL ) != 0;

            int x = ( (int)(short)LOWORD( lParam ) );
            int y = ( (int)(short)HIWORD( lParam ) );

            MouseMotionEventArgs mouseMotionEventArgs( *pRenderWindow, lButton, mButton, rButton, control, shift, x, y );
            gs_MessageQueue.Push( std::bind( &Window::OnMouseMoved, pRenderWindow.get(), mouseMotionEventArgs ) );
        }
        break;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        {
            bool lButton = ( wParam & MK_LBUTTON ) != 0;
            bool rButton = ( wParam & MK_RBUTTON ) != 0;
            bool mButton = ( wParam & MK_MBUTTON ) != 0;
            bool shift = ( wParam & MK_SHIFT ) != 0;
            bool control = ( wParam & MK_CONTROL ) != 0;

            int x = ( (int)(short)LOWORD( lParam ) );
            int y = ( (int)(short)HIWORD( lParam ) );

            MouseButtonEventArgs mouseButtonEventArgs( *pRenderWindow, DecodeMouseButton( message ), ButtonState::Pressed, lButton, mButton, rButton, control, shift, x, y );
            gs_MessageQueue.Push( std::bind( &Window::OnMouseButtonPressed, pRenderWindow.get(), mouseButtonEventArgs ) );
        }
        break;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        {
            bool lButton = ( wParam & MK_LBUTTON ) != 0;
            bool rButton = ( wParam & MK_RBUTTON ) != 0;
            bool mButton = ( wParam & MK_MBUTTON ) != 0;
            bool shift = ( wParam & MK_SHIFT ) != 0;
            bool control = ( wParam & MK_CONTROL ) != 0;

            int x = ( (int)(short)LOWORD( lParam ) );
            int y = ( (int)(short)HIWORD( lParam ) );

            MouseButtonEventArgs mouseButtonEventArgs( *pRenderWindow, DecodeMouseButton( message ), ButtonState::Released, lButton, mButton, rButton, control, shift, x, y );
            gs_MessageQueue.Push( std::bind( &Window::OnMouseButtonReleased, pRenderWindow.get(), mouseButtonEventArgs ) );
        }
        break;
        case WM_MOUSEWHEEL:
        {
            // The distance the mouse wheel is rotated.
            // A positive value indicates the wheel was rotated to the right.
            // A negative value indicates the wheel was rotated to the left.
            float zDelta = ( (int)(short)HIWORD( wParam ) ) / (float)WHEEL_DELTA;
            short keyStates = (short)LOWORD( wParam );

            bool lButton = ( keyStates & MK_LBUTTON ) != 0;
            bool rButton = ( keyStates & MK_RBUTTON ) != 0;
            bool mButton = ( keyStates & MK_MBUTTON ) != 0;
            bool shift = ( keyStates & MK_SHIFT ) != 0;
            bool control = ( keyStates & MK_CONTROL ) != 0;

            int x = ( (int)(short)LOWORD( lParam ) );
            int y = ( (int)(short)HIWORD( lParam ) );

            // Convert the screen coordinates to client coordinates.
            POINT clientToScreenPoint;
            clientToScreenPoint.x = x;
            clientToScreenPoint.y = y;
            ::ScreenToClient( hwnd, &clientToScreenPoint );

            MouseWheelEventArgs mouseWheelEventArgs( *pRenderWindow, zDelta, lButton, mButton, rButton, control, shift, (int)clientToScreenPoint.x, (int)clientToScreenPoint.y );
            gs_MessageQueue.Push( std::bind( &Window::OnMouseWheel, pRenderWindow.get(), mouseWheelEventArgs ) );
        }
        break;
        // NOTE: Not really sure if these next set of messages are working correctly.
        // Not really sure HOW to get them to work correctly.
        // TODO: Try to fix these if I need them ;)
        case WM_CAPTURECHANGED:
        {
            EventArgs mouseBlurEventArgs( *pRenderWindow );
            gs_MessageQueue.Push( std::bind( &Window::OnMouseBlur, pRenderWindow.get(), mouseBlurEventArgs ) );
        }
        break;
        case WM_MOUSEACTIVATE:
        {
            EventArgs mouseFocusEventArgs( *pRenderWindow );
            gs_MessageQueue.Push( std::bind( &Window::OnMouseFocus, pRenderWindow.get(), mouseFocusEventArgs ) );
        }
        break;
        case WM_MOUSELEAVE:
        {
            EventArgs mouseLeaveEventArgs( *pRenderWindow );
            gs_MessageQueue.Push( std::bind( &Window::OnMouseLeave, pRenderWindow.get(), mouseLeaveEventArgs ) );
        }
        break;
        case WM_SIZE:
        {
            int width = ( (int)(short)LOWORD( lParam ) );
            int height = ( (int)(short)HIWORD( lParam ) );

            ResizeEventArgs resizeEventArgs( *pRenderWindow, width, height );
            gs_MessageQueue.Push( std::bind( &Window::OnResize, pRenderWindow.get(), resizeEventArgs ) );
        }
        break;
        case WM_CLOSE:
        {
            // This message cannot be queued on the message queue because we 
            // need the response from the event arguments to determine if the window
            // should really be closed or not.
            WindowCloseEventArgs windowCloseEventArgs( *pRenderWindow );
            pRenderWindow->OnClose( windowCloseEventArgs );

            if ( windowCloseEventArgs.ConfirmClose )
            {
                //DestroyWindow( hwnd );
                // Just hide the window.
                // Windows will be destroyed when the application quits.
                ::ShowWindow( hwnd, SW_HIDE );
            }
        }
        break;
        case WM_DESTROY:
        {
            std::lock_guard<std::mutex> lock( gs_WindowHandlesMutex );
            WindowHandleMap::iterator iter = gs_WindowHandles.find( hwnd );
            if ( iter != gs_WindowHandles.end() )
            {
                gs_WindowHandles.erase( iter );
            }
        }
        break;
        default:
        {
            return ::DefWindowProc( hwnd, message, wParam, lParam );
        }
        break;
        }
    }
    else
    {
        switch ( message )
        {
        case WM_CREATE:
            break;
        default:
        {
            return ::DefWindowProc( hwnd, message, wParam, lParam );
        }
        break;
        }
    }
    
    return 0;
}
