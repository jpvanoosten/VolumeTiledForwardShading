#pragma once

/*
 *  Copyright(c) 2015 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

/**
 *  @file CameraController.h
 *  @date March 4, 2016
 *  @author jeremiah
 *
 *  @brief Camera Controller.
 */

#include <Events.h>

namespace Graphics
{
    class Camera;
}

class CameraController
{
public:
    CameraController( std::shared_ptr<Graphics::Camera> camera, uint8_t joystickID = 0xff );

    void SetCameraRotation( const glm::quat& rot );
    void SetInvertY(bool invertY);

    void OnUpdate( Core::UpdateEventArgs& e );

    // Keyboard events
    void OnKeyboard( Core::KeyEventArgs& e );

    // Mouse events
    void OnMouseMotion( Core::MouseMotionEventArgs& e );
    void OnMouseButton( Core::MouseButtonEventArgs& e );
    void OnMouseWheel( Core::MouseWheelEventArgs& e );

    void OnJoystickButton( Core::JoystickButtonEventArgs& e );
    void OnJoystickPOV( Core::JoystickPOVEventArgs& e );
    void OnJoystickAxis( Core::JoystickAxisEventArgs& e );

protected:

private:
    std::shared_ptr<Graphics::Camera> m_Camera;
    uint8_t m_JoystickID;

    bool m_BoostMovement;
    bool m_BoostRotation;
    double m_InvertY;

    double m_PreviousForward;
    double m_PreviousRight;
    double m_PreviousUp;

    double m_PreviousPitch;
    double m_PreviousYaw;

    double m_CurrentPitch;
    double m_CurrentYaw;

    // Mouse deltas
    double m_MouseX;
    double m_MouseY;
    // Mouse wheel delta
    float m_MouseWheel;

    // Mouse buttons.
    bool m_MouseButtonLeft;
    bool m_MouseButtonRight;
    bool m_MouseButtonMiddle;

    // Keyboard keys.
    float m_KeyW;
    float m_KeyA;
    float m_KeyS;
    float m_KeyD;
    float m_KeyQ;
    float m_KeyE;
    float m_KeyLeft;
    float m_KeyRight;
    float m_KeyUp;
    float m_KeyDown;

    bool m_IsShiftPressed;

    // Joystick analog inputs
    float m_LeftThumbX;
    float m_LeftThumbY;
    float m_RightThumbX;
    float m_RightThumbY;
    float m_LeftTrigger;
    float m_RightTrigger;

    // Joystick buttons
    bool m_A;
    bool m_B;
    bool m_X;
    bool m_Y;
    bool m_LB;
    bool m_RB;
    bool m_LS;
    bool m_RS;
    bool m_Back;
    bool m_Start;

    // POV (Direction pad)
    struct DPad
    {
        bool Up;
        bool Right;
        bool Down;
        bool Left;

        DPad()
            : Up(false)
            , Right(false)
            , Down(false)
            , Left(false)
        {}

        void Reset()
        {
            Up = Down = Left = Right = false;
        }

    } m_DPad;
};