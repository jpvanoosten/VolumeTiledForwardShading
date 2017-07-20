#include <EnginePCH.h>

#include <GUI/GUI.h>
#include <Graphics/Window.h>

extern void BindDevice( std::shared_ptr<Graphics::Device> device, std::shared_ptr<Graphics::Window> window );

using namespace Core;
using namespace GUI;
using namespace Graphics;

Event::ScopedConnections g_ScopedConnections;

void OnResize( Core::ResizeEventArgs& e )
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2( static_cast<float>( e.Width ), static_cast<float>( e.Height ) );
}

void OnKeyPressed( Core::KeyEventArgs& e )
{
    ImGuiIO& io = ImGui::GetIO();

    io.AddInputCharacter( e.Char );
}

void OnKey( Core::KeyEventArgs& e )
{
    ImGuiIO& io = ImGui::GetIO();

    io.KeyCtrl = e.Control;
    io.KeyShift = e.Shift;
    io.KeyAlt = e.Alt;

    uint32_t keyCode = static_cast<uint32_t>( e.Key );

    if ( keyCode < 512 )
    {
        io.KeysDown[keyCode] = ( e.State == KeyState::Pressed );
    }
}

void OnMouseButton( Core::MouseButtonEventArgs& e )
{
    ImGuiIO& io = ImGui::GetIO();

    switch ( e.Button )
    {
    case MouseButton::Left:
        io.MouseDown[0] = ( e.State == ButtonState::Pressed );
        break;
    case MouseButton::Right:
        io.MouseDown[1] = ( e.State == ButtonState::Pressed );
        break;
    case MouseButton::Middel:
        io.MouseDown[2] = ( e.State == ButtonState::Pressed );
        break;
    }
}

void OnMouseMoved( Core::MouseMotionEventArgs& e )
{
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2( static_cast<float>( e.X ), static_cast<float>( e.Y ) );
}

void OnMouseWheel( Core::MouseWheelEventArgs& e )
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel += e.WheelDelta;
}


void GUI::Init( std::shared_ptr<Graphics::Device> device, std::shared_ptr<Graphics::Window> window )
{
    BindDevice( device, window );

    ImGuiIO& io = ImGui::GetIO();
    
    io.KeyMap[ImGuiKey_Tab] = static_cast<int>( KeyCode::Tab );                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_LeftArrow] = static_cast<int>( KeyCode::Left );
    io.KeyMap[ImGuiKey_RightArrow] = static_cast<int>( KeyCode::Right );
    io.KeyMap[ImGuiKey_UpArrow] = static_cast<int>( KeyCode::Up );
    io.KeyMap[ImGuiKey_DownArrow] = static_cast<int>( KeyCode::Down );
    io.KeyMap[ImGuiKey_PageUp] = static_cast<int>( KeyCode::PageUp );
    io.KeyMap[ImGuiKey_PageDown] = static_cast<int>( KeyCode::PageDown );
    io.KeyMap[ImGuiKey_Home] = static_cast<int>( KeyCode::Home );
    io.KeyMap[ImGuiKey_End] = static_cast<int>( KeyCode::End );
    io.KeyMap[ImGuiKey_Delete] = static_cast<int>( KeyCode::Delete );
    io.KeyMap[ImGuiKey_Backspace] = static_cast<int>( KeyCode::Back );
    io.KeyMap[ImGuiKey_Enter] = static_cast<int>( KeyCode::Enter );
    io.KeyMap[ImGuiKey_Escape] = static_cast<int>( KeyCode::Escape );
    io.KeyMap[ImGuiKey_A] = static_cast<int>( KeyCode::A );
    io.KeyMap[ImGuiKey_C] = static_cast<int>( KeyCode::C );
    io.KeyMap[ImGuiKey_V] = static_cast<int>( KeyCode::V );
    io.KeyMap[ImGuiKey_X] = static_cast<int>( KeyCode::X );
    io.KeyMap[ImGuiKey_Y] = static_cast<int>( KeyCode::Y );
    io.KeyMap[ImGuiKey_Z] = static_cast<int>( KeyCode::Z );

    io.KeyRepeatDelay = 1.0f;
    io.KeyRepeatRate = 0.5f;

    g_ScopedConnections.clear();
    if ( window )
    {
        g_ScopedConnections.push_back( window->Resize += &OnResize );
        g_ScopedConnections.push_back( window->KeyPressed += &OnKeyPressed );
        g_ScopedConnections.push_back( window->KeyPressed += &OnKey );
        g_ScopedConnections.push_back( window->KeyReleased += &OnKey );
        g_ScopedConnections.push_back( window->MouseButtonPressed += &OnMouseButton );
        g_ScopedConnections.push_back( window->MouseButtonReleased += &OnMouseButton );
        g_ScopedConnections.push_back( window->MouseMoved += &OnMouseMoved );
        g_ScopedConnections.push_back( window->MouseWheel += &OnMouseWheel );
    }
}
