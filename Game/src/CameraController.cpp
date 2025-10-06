#include <GamePCH.h>

#include <CameraController.h>
#include <Graphics/Camera.h>
#include <Graphics/Profiler.h>
#include <LogManager.h>
#include <GUI/GUI.h>

using namespace Core;
using namespace Graphics;

void Smooth( double& x0, double& x1, double deltaTime )
{
    double x;
    if ( glm::abs( x0 ) < glm::abs( x1 ) )
    {
        x = glm::mix( x1, x0, glm::pow( 0.6f, deltaTime * 60.0 ) );
    }
    else
    {
        x = glm::mix( x1, x0, glm::pow( 0.8f, deltaTime * 60.0 ) );
    }

    x0 = x;
    x1 = x;
}

CameraController::CameraController( std::shared_ptr<Graphics::Camera> camera, uint8_t joystickID )
    : m_Camera( camera )
    , m_JoystickID( joystickID )
    , m_BoostMovement( false )
    , m_BoostRotation( false )
    , m_InvertY( 1.0 )
    , m_PreviousForward( 0.0f )
    , m_PreviousRight( 0.0f )
    , m_PreviousUp( 0.0f )
    , m_PreviousPitch( 0.0f )
    , m_PreviousYaw( 0.0f )
    , m_CurrentPitch( 0.0f )
    , m_CurrentYaw( 0.0f )
    , m_MouseX( 0 )
    , m_MouseY( 0 )
    , m_MouseWheel( 0 )
    , m_MouseButtonLeft( false )
    , m_MouseButtonRight( false )
    , m_MouseButtonMiddle( false )
    , m_KeyW( 0 )
    , m_KeyA( 0 )
    , m_KeyS( 0 )
    , m_KeyD( 0 )
    , m_KeyQ( 0 )
    , m_KeyE( 0 )
    , m_KeyLeft( 0 )
    , m_KeyRight( 0 )
    , m_KeyUp( 0 )
    , m_KeyDown( 0 )
    , m_IsShiftPressed( false )
    , m_LeftThumbX( 0 )
    , m_LeftThumbY( 0 )
    , m_RightThumbX( 0 )
    , m_RightThumbY( 0 )
    , m_LeftTrigger( 0 )
    , m_RightTrigger( 0 )
    , m_A( false )
    , m_B( false )
    , m_X( false )
    , m_Y( false )
    , m_LB( false )
    , m_RB( false )
    , m_LS( false )
    , m_RS( false )
    , m_Back( false )
    , m_Start( false )
{
    SetCameraRotation( m_Camera->GetRotation() );
}

void CameraController::SetCameraRotation( const glm::quat& rot )
{
    m_CurrentPitch = glm::degrees( glm::pitch( rot ) );
    while ( m_CurrentPitch > 90.0 )
    {
        m_CurrentPitch -= 180.0;
    }
    while ( m_CurrentPitch < -90.0 )
    {
        m_CurrentPitch += 180.0;
    }

    m_CurrentYaw = glm::degrees( glm::yaw( rot ) );
}

void CameraController::SetInvertY(bool invertY)
{
    m_InvertY = invertY ? -1.0 : 1.0;
}

void CameraController::OnUpdate( Core::UpdateEventArgs& e )
{
    CPU_MARKER( __FUNCTION__ );

    const double MOVE_SPEED = 10.0;
    const double LOOK_SENSITIVITY = 180.0;
    const double MOUSE_SENSITIVITY = 0.1;

    if ( m_LS )
    {
        m_BoostMovement = !m_BoostMovement;
        m_LS = false;
    }

    if ( m_RS )
    {
        m_BoostRotation = !m_BoostRotation;
        m_RS = false;
    }

    double speedScale = ( m_BoostMovement || m_IsShiftPressed ) ? 1.0 : 0.1;
    double rotateScale = m_BoostRotation ? 1.0 : 0.5;

    double forward = ( m_KeyS - m_KeyW - m_LeftThumbY ) * MOVE_SPEED * speedScale * e.ElapsedTime;
    double right = ( m_KeyD - m_KeyA + m_LeftThumbX ) * MOVE_SPEED * speedScale * e.ElapsedTime;
    double up = ( m_KeyE - m_KeyQ + m_RightTrigger - m_LeftTrigger ) * MOVE_SPEED * speedScale * e.ElapsedTime;

    double yaw = ( m_KeyLeft - m_KeyRight - m_RightThumbX ) * LOOK_SENSITIVITY * rotateScale * e.ElapsedTime;
    double pitch = ( m_KeyUp -  m_KeyDown + m_RightThumbY ) * m_InvertY * LOOK_SENSITIVITY * rotateScale * e.ElapsedTime;

    Smooth( m_PreviousForward, forward, e.ElapsedTime );
    Smooth( m_PreviousRight, right, e.ElapsedTime );
    Smooth( m_PreviousUp, up, e.ElapsedTime );
    Smooth( m_PreviousPitch, pitch, e.ElapsedTime );
    Smooth( m_PreviousYaw, yaw, e.ElapsedTime );

    yaw += m_MouseX * MOUSE_SENSITIVITY;
    pitch += m_MouseY * MOUSE_SENSITIVITY;

    m_CurrentPitch += pitch;
    m_CurrentYaw += yaw;

    m_CurrentPitch = glm::clamp( m_CurrentPitch, -90.0, 90.0 );

    m_MouseX = m_MouseY = 0.0f;

    m_Camera->TranslateX( static_cast<float>( right ) );
    m_Camera->TranslateY( static_cast<float>( up ) );
    m_Camera->TranslateZ( static_cast<float>( forward ) );

    m_Camera->SetRotate( glm::vec3( m_CurrentPitch, m_CurrentYaw, 0.0f ) );
}

void CameraController::OnKeyboard( Core::KeyEventArgs& e )
{
    if ( ImGui::GetIO().WantCaptureKeyboard ) return;
    if ( e.Control || e.Alt ) return;

    switch ( e.Key )
    {
        case KeyCode::W:
            m_KeyW = ( e.State == KeyState::Pressed ) ? 1.0f : 0.0f;
            break;
        case KeyCode::A:
            m_KeyA = ( e.State == KeyState::Pressed ) ? 1.0f : 0.0f;
            break;
        case KeyCode::S:
            m_KeyS = ( e.State == KeyState::Pressed ) ? 1.0f : 0.0f;
            break;
        case KeyCode::D:
            m_KeyD = ( e.State == KeyState::Pressed ) ? 1.0f : 0.0f;
            break;
        case KeyCode::Q:
            m_KeyQ = ( e.State == KeyState::Pressed ) ? 1.0f : 0.0f;
            break;
        case KeyCode::E:
            m_KeyE = ( e.State == KeyState::Pressed ) ? 1.0f : 0.0f;
            break;
        case KeyCode::Up:
            m_KeyUp = ( e.State == KeyState::Pressed ) ? 1.0f : 0.0f;
            break;
        case KeyCode::Down:
            m_KeyDown = ( e.State == KeyState::Pressed ) ? 1.0f : 0.0f;
            break;
        case KeyCode::Left:
            m_KeyLeft = ( e.State == KeyState::Pressed ) ? 1.0f : 0.0f;
            break;
        case KeyCode::Right:
            m_KeyRight = ( e.State == KeyState::Pressed ) ? 1.0f : 0.0f;
            break;
        case KeyCode::ShiftKey:
        case KeyCode::LShiftKey:
        case KeyCode::RShiftKey:
            m_IsShiftPressed = ( e.State == KeyState::Pressed );
            break;
    }
}

void CameraController::OnMouseMotion( Core::MouseMotionEventArgs& e )
{
    if ( ImGui::GetIO().WantCaptureMouse ) return;

    if ( m_MouseButtonLeft )
    {
        m_MouseX += static_cast<double>( e.RelX );
        m_MouseY += static_cast<double>( e.RelY );
    }
}

void CameraController::OnMouseButton( Core::MouseButtonEventArgs& e )
{
    if ( ImGui::GetIO().WantCaptureMouse ) return;

    switch ( e.Button )
    {
    case MouseButton::Left:
        m_MouseButtonLeft = ( e.State == ButtonState::Pressed );
        break;
    case MouseButton::Right:
        m_MouseButtonRight = ( e.State == ButtonState::Pressed );
        break;
    case MouseButton::Middel:
        m_MouseButtonMiddle = ( e.State == ButtonState::Pressed );
        break;
    }
}

void CameraController::OnMouseWheel( Core::MouseWheelEventArgs& e )
{
    if ( ImGui::GetIO().WantCaptureMouse ) return;

//    m_Camera->SetPivotDistance( m_Camera->GetPivotDistance() - e.WheelDelta );

    m_MouseWheel = e.WheelDelta;
}

void CameraController::OnJoystickButton( Core::JoystickButtonEventArgs& e )
{
    if ( ( m_JoystickID & ( 1 << e.JoystickID ) ) == 0 ) return;

    switch ( e.ButtonID )
    {
    case 1:
        m_A = ( e.State == ButtonState::Pressed );
        break;
    case 2:
        m_B = ( e.State == ButtonState::Pressed );
        break;
    case 3:
        m_X = ( e.State == ButtonState::Pressed );
        break;
    case 4:
        m_Y = ( e.State == ButtonState::Pressed );
        break;
    case 5:
        m_LB = ( e.State == ButtonState::Pressed );
        break;
    case 6:
        m_RB = ( e.State == ButtonState::Pressed );
        break;
    case 7:
        m_Back = ( e.State == ButtonState::Pressed );
        break;
    case 8:
        m_Start = ( e.State == ButtonState::Pressed );
        break;
    case 9:
        m_LS = ( e.State == ButtonState::Pressed );
        break;
    case 10:
        m_RS = ( e.State == ButtonState::Pressed );
        break;
    }
}

void CameraController::OnJoystickPOV( Core::JoystickPOVEventArgs& e )
{
    if ( ( m_JoystickID & ( 1 << e.JoystickID ) ) == 0 ) return;

    m_DPad.Reset();

    switch ( e.Direction )
    {
    case POVDirection::Up:
        m_DPad.Up = true;
        break;
    case POVDirection::UpRight:
        m_DPad.Up = true;
        m_DPad.Right = true;
        break;
    case POVDirection::Right:
        m_DPad.Right = true;
        break;
    case POVDirection::DownRight:
        m_DPad.Down = true;
        m_DPad.Right = true;
        break;
    case POVDirection::Down:
        m_DPad.Down = true;
        break;
    case POVDirection::DownLeft:
        m_DPad.Down = true;
        m_DPad.Left = true;
        break;
    case POVDirection::Left:
        m_DPad.Left = true;
        break;
    case POVDirection::UpLeft:
        m_DPad.Up = true;
        m_DPad.Left = true;
        break;
    }

}

void CameraController::OnJoystickAxis( Core::JoystickAxisEventArgs& e )
{
    if ( ( m_JoystickID & ( 1 << e.JoystickID ) ) == 0 ) return;

    switch ( e.ChangedAxis )
    {
    case JoystickAxis::ZAxis:
        m_LeftTrigger = e.Axis;
        break;
    case JoystickAxis::RAxis:
        m_RightTrigger = e.Axis;
        break;
    case JoystickAxis::XAxis:
        m_LeftThumbX = e.Axis;
        break;
    case JoystickAxis::YAxis:
        m_LeftThumbY = e.Axis;
        break;
    case JoystickAxis::UAxis:
        m_RightThumbX = e.Axis;
        break;
    case JoystickAxis::VAxis:
        m_RightThumbY = e.Axis;
        break;
    }
}
