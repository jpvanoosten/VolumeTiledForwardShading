#include <EnginePCH.h>

#include <Graphics/Window.h>
#include <Application.h>
#include <LogManager.h>

using namespace Graphics;

Window::Window( Core::Application& app, const std::string& windowName, uint32_t windowWidth, uint32_t windowHeight, bool fullScreen, bool vSync )
    : m_Application( app )
    , m_sWindowName( windowName )
    , m_iWindowWidth( windowWidth )
    , m_iWindowHeight( windowHeight )
    , m_vSync( vSync )
    , m_FullScreen( fullScreen )
    , m_PreviousMousePosition( 0, 0 )
    , m_bInClientRect( false )
    , m_bHasKeyboardFocus( false )
{
    // Clear previous joystick info array
    memset( m_PreviousJoystickInfo, 0, sizeof( m_PreviousJoystickInfo ) );
    // Clear previous joystick pov state.
    for ( int i = 0; i < NUM_SUPPORTED_JOYSTICKS; i++ )
    {
        m_PreviousJoystickInfo[i].dwPOV = JOY_POVCENTERED;
        m_PreviousJoystickInfo[i].dwRpos = 0x7fff;
        //        m_PreviousJoystickInfo[i].dwUpos = 0x7fff;
        //        m_PreviousJoystickInfo[i].dwVpos = 0x7fff;
        m_PreviousJoystickInfo[i].dwXpos = 0x7fff;
        m_PreviousJoystickInfo[i].dwYpos = 0x7fff;
        m_PreviousJoystickInfo[i].dwZpos = 0x7fff;
    }

    // Register initialize/terminate events.
    m_EventConnections.push_back( app.Initialize += boost::bind( &Window::OnInitialize, this, _1 ) );
    m_EventConnections.push_back( app.Terminate += boost::bind( &Window::OnTerminate, this, _1 ) );

    // Register update/render events.
    m_EventConnections.push_back( app.Update += boost::bind( &Window::OnUpdate, this, _1 ) );
}

Window::~Window()
{
    // Disconnect events.
    for ( auto& eventConnection : m_EventConnections )
    {
        eventConnection.disconnect();
    }
}

void Window::ShowWindow()
{}

void Window::HideWindow()
{}

void Window::CloseWindow()
{
    // Disconnect events.
    for ( auto& eventConnection : m_EventConnections )
    {
        eventConnection.disconnect();
    }
    m_EventConnections.clear();
}

Core::Application& Window::GetApplication()
{
    return m_Application;
}

const std::string& Window::GetWindowName() const
{
    return m_sWindowName;
}

uint32_t Window::GetWindowWidth() const
{
    return m_iWindowWidth;
}

uint32_t Window::GetWindowHeight() const
{
    return m_iWindowHeight;
}

bool Window::IsVSync() const
{
    return m_vSync;
}

void Window::SetVSync( bool vSync )
{
    m_vSync = vSync;
}

bool Window::IsFullScreen() const
{
    return m_FullScreen;
}

void Window::SetFullScreen( bool fullscreen )
{
    m_FullScreen = fullscreen;
}

void Window::OnInitialize( Core::EventArgs& e )
{
    Initialize( e );
}

void Window::OnTerminate( Core::EventArgs& e )
{
    Terminate( e );
}
// The window has received focus
void Window::OnInputFocus( Core::EventArgs& e )
{
    InputFocus( e );
}

// The RenderWindow window has lost focus
void Window::OnInputBlur( Core::EventArgs& e )
{
    InputBlur( e );
}

// The RenderWindow window has been minimized
void Window::OnMinimize( Core::EventArgs& e )
{
    Minimize( e );
}

// The RenderWindow window has been restored
void Window::OnRestore( Core::EventArgs& e )
{
    Restore( e );
}

// The RenderWindow window has be resized
void Window::OnResize( Core::ResizeEventArgs& e )
{
    m_iWindowWidth = e.Width;
    m_iWindowHeight = e.Height;

    Resize( e );
}

// The window contents should be repainted
void Window::OnExpose( Core::EventArgs& e )
{
    Expose( e );
}

// Normalize a value in the range [min - max]
template<typename T, typename U>
inline T NormalizeRange( U x, U min, U max )
{
    return T( x - min ) / T( max - min );
}

// Shift and bias a value into another range.
template<typename T, typename U>
inline T ShiftBias( U x, U shift, U bias )
{
    return T( x * bias ) + T( shift );
}

void Window::ProcessJoysticks()
{
    // Process joystick input 
    // and fire appropriate events.
    JOYINFOEX joystickInfo;
    joystickInfo.dwSize = sizeof( JOYINFOEX );
    joystickInfo.dwFlags = JOY_RETURNALL | JOY_RETURNPOVCTS;

    JOYCAPS joystickCapabilities;


    for ( UINT joystickID = 0; joystickID < NUM_SUPPORTED_JOYSTICKS; joystickID++ )
    {
        // Skip joysticks that generate errors.
        if ( joyGetPosEx( joystickID, &joystickInfo ) != JOYERR_NOERROR ) continue;
        joyGetDevCaps( joystickID, &joystickCapabilities, sizeof( JOYCAPS ) );

        DWORD currentButtons = joystickInfo.dwButtons;
        DWORD previousButtons = m_PreviousJoystickInfo[joystickID].dwButtons;
        bool buttonStates[32];
        int i;
        DWORD buttonMask;

        for ( i = 0, buttonMask = 1; i < 32; i++, buttonMask = buttonMask << 1 )
        {
            buttonStates[i] = ( currentButtons & buttonMask ) != 0;
        }

        // Which buttons are now pressed this frame.
        DWORD buttonPressed = currentButtons & ~previousButtons;
        // Which buttons are still being held since the last frame.
        DWORD buttonHeld = currentButtons & previousButtons;
        // Which buttons were released this frame.
        DWORD buttonReleased = previousButtons & ~currentButtons;

        // Fire an event for every button that is now "pressed"
        for ( i = 0, buttonMask = 1; i < 32; i++, buttonMask = buttonMask << 1 )
        {
            if ( ( buttonPressed & buttonMask ) != 0 )
            {
                Core::JoystickButtonEventArgs joyEventArgs( *this, joystickID, Core::ButtonState::Pressed, i, buttonStates );
                OnJoystickButtonPressed( joyEventArgs );
            }
        }
        // Fire an event for every button that is now "released"
        for ( i = 0, buttonMask = 1; i < 32; i++, buttonMask = buttonMask << 1 )
        {
            if ( ( buttonReleased & buttonMask ) != 0 )
            {
                Core::JoystickButtonEventArgs joyEventArgs( *this, joystickID, Core::ButtonState::Released, i, buttonStates );
                OnJoystickButtonReleased( joyEventArgs );
            }
        }

        // Check Point of View hat
        // Both continuous and discrete values are handled similarly.
        if ( m_PreviousJoystickInfo[joystickID].dwPOV != joystickInfo.dwPOV )
        {
            float angle = joystickInfo.dwPOV / 100.0f;
            // Compute the discreet interval.
            Core::POVDirection dir = Core::POVDirection::Centered;
            if ( angle >= 0.0f && angle < 360.0f )
            {
                // Convert the continuous angle value into a discreet interval.
                dir = ( Core::POVDirection )( (int)( glm::round<int>( (int)( ( angle / 360.0f ) * 8.0f ) * 45 ) ) );
            }
            else
            {
                angle = -1.0f;
            }

            Core::JoystickPOVEventArgs joyEventArgs( *this, joystickID, angle, dir, buttonStates );
            OnJoystickPOV( joyEventArgs );
        }

        if ( m_PreviousJoystickInfo[joystickID].dwRpos != joystickInfo.dwRpos )
        {
            float axis = ShiftBias<float>( NormalizeRange<float, UINT>( joystickInfo.dwRpos, joystickCapabilities.wRmin, joystickCapabilities.wRmax ), -1.0f, 2.0f );
            Core::JoystickAxisEventArgs joyEventArgs( *this, joystickID, Core::JoystickAxis::RAxis, axis, buttonStates );
            OnJoystickAxis( joyEventArgs );
        }
        if ( m_PreviousJoystickInfo[joystickID].dwUpos != joystickInfo.dwUpos )
        {
            float axis = ShiftBias<float>( NormalizeRange<float, UINT>( joystickInfo.dwUpos, joystickCapabilities.wUmin, joystickCapabilities.wUmax ), -1.0f, 2.0f );
            Core::JoystickAxisEventArgs joyEventArgs( *this, joystickID, Core::JoystickAxis::UAxis, axis, buttonStates );
            OnJoystickAxis( joyEventArgs );
        }
        if ( m_PreviousJoystickInfo[joystickID].dwVpos != joystickInfo.dwVpos )
        {
            float axis = ShiftBias<float>( NormalizeRange<float, UINT>( joystickInfo.dwVpos, joystickCapabilities.wVmin, joystickCapabilities.wVmax ), -1.0f, 2.0f );
            Core::JoystickAxisEventArgs joyEventArgs( *this, joystickID, Core::JoystickAxis::VAxis, axis, buttonStates );
            OnJoystickAxis( joyEventArgs );
        }
        if ( m_PreviousJoystickInfo[joystickID].dwXpos != joystickInfo.dwXpos )
        {
            float axis = ShiftBias<float>( NormalizeRange<float, UINT>( joystickInfo.dwXpos, joystickCapabilities.wXmin, joystickCapabilities.wXmax ), -1.0f, 2.0f );
            Core::JoystickAxisEventArgs joyEventArgs( *this, joystickID, Core::JoystickAxis::XAxis, axis, buttonStates );
            OnJoystickAxis( joyEventArgs );
        }
        if ( m_PreviousJoystickInfo[joystickID].dwYpos != joystickInfo.dwYpos )
        {
            float axis = ShiftBias<float>( NormalizeRange<float, UINT>( joystickInfo.dwYpos, joystickCapabilities.wYmin, joystickCapabilities.wYmax ), -1.0f, 2.0f );
            Core::JoystickAxisEventArgs joyEventArgs( *this, joystickID, Core::JoystickAxis::YAxis, axis, buttonStates );
            OnJoystickAxis( joyEventArgs );
        }
        if ( m_PreviousJoystickInfo[joystickID].dwZpos != joystickInfo.dwZpos )
        {
            float axis = ShiftBias<float>( NormalizeRange<float, UINT>( joystickInfo.dwZpos, joystickCapabilities.wZmin, joystickCapabilities.wZmax ), -1.0f, 2.0f );
            Core::JoystickAxisEventArgs joyEventArgs( *this, joystickID, Core::JoystickAxis::ZAxis, axis, buttonStates );
            OnJoystickAxis( joyEventArgs );
        }

        // Store the current joystick info for the next frame.
        m_PreviousJoystickInfo[joystickID] = joystickInfo;
    }

}


void Window::OnUpdate( Core::UpdateEventArgs& e )
{
    // Don't processes joystick events unless the window has keyboard focus
    // (The window is selected and can receive keyboard events)
    //    if ( m_bHasKeyboardFocus )
    {
//        ProcessJoysticks();
    }

    Update( e );
}

// The prepare the window for redraw (update shader parameters)
void Window::OnPreRender( Core::RenderEventArgs& e )
{
    Core::RenderEventArgs renderArgs( *this, e.ElapsedTime, e.TotalTime, e.FrameCounter, e.Camera, e.GraphicsCommandBuffer );
    PreRender( renderArgs );
}

// The window should be redrawn.
void Window::OnRender( Core::RenderEventArgs& e )
{
    Core::RenderEventArgs renderArgs( *this, e.ElapsedTime, e.TotalTime, e.FrameCounter, e.Camera, e.GraphicsCommandBuffer );
    Render( renderArgs );
}

// Handle any post-drawing events (like GUI)
void Window::OnPostRender( Core::RenderEventArgs& e )
{
    Core::RenderEventArgs renderArgs( *this, e.ElapsedTime, e.TotalTime, e.FrameCounter, e.Camera, e.GraphicsCommandBuffer );
    PostRender( renderArgs );
}

// A keyboard key was pressed
void Window::OnKeyPressed( Core::KeyEventArgs& e )
{
    KeyPressed( e );
}

// A keyboard key was released
void Window::OnKeyReleased( Core::KeyEventArgs& e )
{
    KeyReleased( e );
}

// Window gained keyboard focus
void Window::OnKeyboardFocus( Core::EventArgs& e )
{
    m_bHasKeyboardFocus = true;
    KeyboardFocus( e );
}

// Window lost keyboard focus
void Window::OnKeyboardBlur( Core::EventArgs& e )
{
    m_bHasKeyboardFocus = false;
    KeyboardBlur( e );
}

// The mouse was moved
void Window::OnMouseMoved( Core::MouseMotionEventArgs& e )
{
    if ( !m_bInClientRect )
    {
        m_PreviousMousePosition = glm::ivec2( e.X, e.Y );
        m_bInClientRect = true;
    }

    e.RelX = e.X - m_PreviousMousePosition.x;
    e.RelY = e.Y - m_PreviousMousePosition.y;

    m_PreviousMousePosition = glm::ivec2( e.X, e.Y );

    MouseMoved( e );
}

// A button on the mouse was pressed
void Window::OnMouseButtonPressed( Core::MouseButtonEventArgs& e )
{
    MouseButtonPressed( e );
}

// A button on the mouse was released
void Window::OnMouseButtonReleased( Core::MouseButtonEventArgs& e )
{
    MouseButtonReleased( e );
}

void Window::OnMouseWheel( Core::MouseWheelEventArgs& e )
{
    MouseWheel( e );
}

void Window::OnMouseLeave( Core::EventArgs& e )
{
    m_bInClientRect = false;
    MouseLeave( e );
}

// The window has received mouse focus
void Window::OnMouseFocus( Core::EventArgs& e )
{
    MouseFocus( e );
}

// The window has lost mouse focus
void Window::OnMouseBlur( Core::EventArgs& e )
{
    MouseBlur( e );
}

void Window::OnJoystickButtonPressed( Core::JoystickButtonEventArgs& e )
{
    JoystickButtonPressed( e );
}

void Window::OnJoystickButtonReleased( Core::JoystickButtonEventArgs& e )
{
    JoystickButtonReleased( e );
}

void Window::OnJoystickPOV( Core::JoystickPOVEventArgs& e )
{
    JoystickPOV( e );
}

void Window::OnJoystickAxis( Core::JoystickAxisEventArgs& e )
{
    JoystickAxis( e );
}

void Window::OnClose( Core::WindowCloseEventArgs& e )
{
    Close( e );
}
