#pragma once

#include "../EngineDefines.h"
#include "../Events.h"
#include "Viewport.h"
#include "Ray.h"

namespace Graphics
{
    class ENGINE_DLL Camera
    {
    public:
        enum class Space
        {
            Local,
            World
        };

        Camera();
        virtual ~Camera()
        {};

        // TODO: Viewports belong in the rasterizer state.. not the camera.
        // Set view port parameters
        void SetViewport( const Graphics::Viewport& viewport );
        const Graphics::Viewport& GetViewport() const;

        // Set projection parameters
        void SetProjection( float fovy, float aspect, float zNear, float zFar );

        float GetNearClipPlane() const;
        float GetFarClipPlane() const;
        float GetFOV() const;

        // Set an orthographic projection using screen space coordinates.
        void SetOrthographic( float left, float right, float top, float bottom );

        // Add this pitch (rotation about the X-axis) in degrees
        // to the current camera's pitch 
        void AddPitch( float fPitch, Space space = Space::Local );
        // Add this yaw (rotation about the Y-axis) in degrees
        // to the current camera's yaw
        void AddYaw( float fYaw, Space space = Space::Local );
        // Add this roll (rotation about the Z-axis) in degrees 
        // to the current camera's roll
        void AddRoll( float fRoll, Space space = Space::Local );

        // Set Euler angles (in degrees)
        void SetEulerAngles( const glm::vec3& eulerAngles );

        // Add rotation to the current rotation (always in local space)
        void AddRotation( const glm::quat& rot );

        // Translate the the camera's X-axis
        void TranslateX( float x, Space space = Space::Local );
        // Translate the camera's Y-axis
        void TranslateY( float y, Space space = Space::Local );
        // Translate the camera's Z-axis
        void TranslateZ( float z, Space space = Space::Local );

        // Set the current translation of the camera, replacing the current translation.
        void SetTranslate( const glm::vec3& translate );
        // Set the current rotation from Euler angles in degrees, replacing the current rotation.
        void SetRotate( float pitch, float yaw, float roll );
        // Set the current rotation from Euler angles in degrees, replacing the current rotation.
        void SetRotate( const glm::vec3& rotate );
        void SetRotate( const glm::quat& rot );

        // If the pivot distance > 0 then the camera will rotate around a pivot point
        // that is pivotDistance along the lookAt vector of the camera.
        void SetPivotDistance( float pivotDistance );
        float GetPivotDistance() const;

        // Get the camera's pivot point in world space
        glm::vec3 GetPivotPoint() const;

        glm::vec3 GetTranslation() const;
        glm::quat GetRotation() const;

        // Get the current rotation of the camera in Euler angles (degrees)
        glm::vec3 GetEulerAngles() const;

        // Directly set the view matrix
        void        SetViewMatrix( const glm::mat4& viewMatrix );
        glm::mat4   GetViewMatrix() const;
        glm::mat4   GetInverseViewMatrix() const;

        // Directly set the projection matrix
        void        SetProjectionMatrix( const glm::mat4& projectionMatrix );
        glm::mat4   GetProjectionMatrix() const;

        // Get the view projection inverse matrix (useful for picking)
        glm::mat4   GetViewProjectionInverseMatrix() const;

        // Converts a screen point to a ray in world space.
        Graphics::Ray   ScreenPointToRay( const glm::vec2& screenPoint ) const;

        // For arcball camera, call this function with client-space coordinates
        // when the mouse is clicked on the screen.
        void OnMousePressed( Core::MouseButtonEventArgs& e );
        void OnMouseMoved( Core::MouseMotionEventArgs& e );

    protected:

        glm::vec3 ProjectOntoUnitSphere( glm::ivec2 screenPos );

        virtual void UpdateViewMatrix();
        virtual void UpdateViewProjectionInverse();

        // View port parameters
        Graphics::Viewport    m_Viewport;

        // Projection parameters
        float       m_fVFOV;
        float       m_fAspect;
        float       m_fNear;
        float       m_fFar;

        // World space parameters
        glm::vec3   m_Translate;
        glm::quat   m_Rotate;

        // Used for arcball camera
        glm::vec3   m_PreviousPoint;
        float       m_PivotDistance;

        // View matrix
        glm::mat4   m_ViewMatrix;
        // Inverse view matrix
        glm::mat4   m_InverseViewMatrix;
        // Projection matrix.
        glm::mat4   m_ProjectionMatrix;

        glm::mat4	m_ViewProjectionInverse;

        // True if the view matrix needs to be updated.
        bool        m_bViewDirty;
        // True if the view projection inverse matrix needs to be updated.
        bool        m_bViewProjectionInverseDirty;

    private:

    };
}