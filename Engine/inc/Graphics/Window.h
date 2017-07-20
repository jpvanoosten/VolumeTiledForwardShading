#pragma once

#include <Object.h>
#include <Events.h>

#include <Graphics/TextureFormat.h>

#include <cstdint>
// For JOYINFOEX structure
#include <joystickapi.h>

// Constants
constexpr uint32_t NUM_SUPPORTED_JOYSTICKS = 16;

// Forward declarations
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM ); // Defined in the Application class.

namespace Core
{
    class Application;
}

namespace Graphics
{
    class RenderTarget;

    class ENGINE_DLL Window : public Core::Object
    {
    public:
        using super = Core::Object;

        // Show this window if it is hidden.
        virtual void ShowWindow() = 0;
        // Hide the window. The window will not be destroyed and can be 
        // shown again using the ShowWindow() function.
        virtual void HideWindow() = 0;

        // Destroy and close the window.
        virtual void CloseWindow() = 0;

        const std::string& GetWindowName() const;

        uint32_t GetWindowWidth() const;
        uint32_t GetWindowHeight() const;

        virtual bool IsVSync() const;
        virtual void SetVSync( bool vSync );

        virtual bool IsFullScreen() const;
        virtual void SetFullScreen( bool fullscreen );

        /**
         * Set the texture format of the color buffer associated to this window's
         * render target
         */
        virtual void SetColorFormat( const TextureFormat& colorFormat ) = 0;

        /**
         * Set the texture format of the depth/stencil buffer associated to this
         * windows render target.
         */
        virtual void SetDepthStencilFormat( const TextureFormat& depthStencilFormat ) = 0;

        // Get the render target associated with the window.
        virtual std::shared_ptr<RenderTarget> GetRenderTarget() const = 0;

        // Present the back buffers
        virtual void Present() = 0;

        // Get the render target of this render window.
    //    virtual std::shared_ptr<RenderTarget> GetRenderTarget() = 0;

        // Invoked when the window is initialized.
        Core::Event             Initialize;
        Core::Event             Terminate;

        // Window is closing
        Core::WindowCloseEvent  Close;

        // Window events
        // Window gets input focus
        Core::Event             InputFocus;
        // Window loses input focus
        Core::Event             InputBlur;
        // Window is minimized.
        Core::Event             Minimize;
        // Window is restored.
        Core::Event             Restore;

        // Invoked when the window contents should be repainted.
        // (Part of the window is now "exposed".
        Core::Event           Expose;

        // Update event is called when the application
        // will be updated before rendering.
        // This this callback to update your game logic.
        Core::UpdateEvent         Update;

        // Invoked when the window needs to be redrawn.
        // Pre-render events are useful for things like updating 
        // effect parameters.
        Core::RenderEvent         PreRender;
        Core::RenderEvent         Render;
        // Post-render events are useful for things like drawing the GUI
        // interface or performing post-process effects.
        Core::RenderEvent         PostRender;

        // Keyboard events
        Core::KeyboardEvent       KeyPressed;
        Core::KeyboardEvent       KeyReleased;
        // Window gained keyboard focus
        Core::Event               KeyboardFocus;
        // Window lost keyboard focus.
        Core::Event               KeyboardBlur;

        // The mouse was moved over the window
        Core::MouseMotionEvent    MouseMoved;
        // A mouse button was pressed over the window
        Core::MouseButtonEvent    MouseButtonPressed;
        // A mouse button was released over the window.
        Core::MouseButtonEvent    MouseButtonReleased;
        // The mouse wheel was changed
        Core::MouseWheelEvent     MouseWheel;
        // The mouse left the client area.
        Core::Event               MouseLeave;
        // The window has gained the mouse focus.
        Core::Event               MouseFocus;
        // The window has lost mouse focus.
        Core::Event               MouseBlur;

        // Joystick events
        Core::JoystickButtonEvent JoystickButtonPressed;
        Core::JoystickButtonEvent JoystickButtonReleased;
        // Event is fired when the POV on the joystick changes.
        Core::JoystickPOVEvent JoystickPOV;
        // Event is fired if any of the axes of the joystick change.
        // No thresholding is applied.
        Core::JoystickAxisEvent JoystickAxis;

        // The window was resized.
        Core::ResizeEvent         Resize;

    protected:
        friend LRESULT CALLBACK ::WndProc( HWND, UINT, WPARAM, LPARAM );

        // Only the application can create windows.
        Window( Core::Application& theApp, const std::string& windowName, uint32_t windowWidth, uint32_t windowHeight, bool fullScreen = false, bool vSync = true );
        virtual ~Window();

        // Get the application that was used to create the window.
        Core::Application& GetApplication();

        virtual void OnInitialize( Core::EventArgs& e );

        // The application window has received focus
        virtual void OnInputFocus( Core::EventArgs& e );
        // The application window has lost focus
        virtual void OnInputBlur( Core::EventArgs& e );


        // The application window has been minimized
        virtual void OnMinimize( Core::EventArgs& e );
        // The application window has been restored
        virtual void OnRestore( Core::EventArgs& e );


        // The application window has been resized
        virtual void OnResize( Core::ResizeEventArgs& e );
        // The window contents should be repainted
        virtual void OnExpose( Core::EventArgs& e );

        // The user requested to exit the application
        virtual void OnClose( Core::WindowCloseEventArgs& e );

        virtual void OnUpdate( Core::UpdateEventArgs& e );
        virtual void OnPreRender( Core::RenderEventArgs& e );
        virtual void OnRender( Core::RenderEventArgs& e );
        virtual void OnPostRender( Core::RenderEventArgs& e );

        // A keyboard key was pressed
        virtual void OnKeyPressed( Core::KeyEventArgs& e );
        // A keyboard key was released
        virtual void OnKeyReleased( Core::KeyEventArgs& e );
        // Window gained keyboard focus
        virtual void OnKeyboardFocus( Core::EventArgs& e );
        // Window lost keyboard focus
        virtual void OnKeyboardBlur( Core::EventArgs& e );

        // The mouse was moved
        virtual void OnMouseMoved( Core::MouseMotionEventArgs& e );
        // A button on the mouse was pressed
        virtual void OnMouseButtonPressed( Core::MouseButtonEventArgs& e );
        // A button on the mouse was released
        virtual void OnMouseButtonReleased( Core::MouseButtonEventArgs& e );
        // The mouse wheel was moved.
        virtual void OnMouseWheel( Core::MouseWheelEventArgs& e );
        // The mouse left the client are of the window.
        virtual void OnMouseLeave( Core::EventArgs& e );
        // The application window has received mouse focus
        virtual void OnMouseFocus( Core::EventArgs& e );
        // The application window has lost mouse focus
        virtual void OnMouseBlur( Core::EventArgs& e );

        virtual void OnJoystickButtonPressed( Core::JoystickButtonEventArgs& e );
        virtual void OnJoystickButtonReleased( Core::JoystickButtonEventArgs& e );
        virtual void OnJoystickPOV( Core::JoystickPOVEventArgs& e );
        virtual void OnJoystickAxis( Core::JoystickAxisEventArgs& e );

        // The application is terminating.
        virtual void OnTerminate( Core::EventArgs& e );

    private:
        // Process joystick input and fire joystick events.
        void ProcessJoysticks();

        Core::Application& m_Application;

        uint32_t m_iWindowWidth;
        uint32_t m_iWindowHeight;

        bool m_vSync;
        bool m_FullScreen;

        std::string m_sWindowName;

        // Used to compute relative mouse motion in this window.
        glm::ivec2 m_PreviousMousePosition;

        // The state of the joystick buttons the last time we polled the joystick events.
        JOYINFOEX m_PreviousJoystickInfo[NUM_SUPPORTED_JOYSTICKS];

        // This is true when the mouse is inside the window's client rect.
        bool m_bInClientRect;

        // This is set to true when the window receives keyboard focus.
        bool m_bHasKeyboardFocus;

        // Keep track of the event connections so that I can disconnect them if the
        // window is closed.
        // Also automatically closes any registered events when the window is destroyed.
        Core::Event::ScopedConnections m_EventConnections;

    };
}
