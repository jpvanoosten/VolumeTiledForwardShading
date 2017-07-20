#include <EnginePCH.h>
#include <Graphics/Camera.h>

using namespace Graphics;

Camera::Camera()
    : m_Translate( 0 )
    , m_PivotDistance( 0 )
    , m_bViewDirty( true )
    , m_bViewProjectionInverseDirty( true )
{}

void Camera::SetViewport( const Viewport& viewport )
{
    m_Viewport = viewport;
}

const Viewport& Camera::GetViewport() const
{
    return m_Viewport;
}

// Calculate right-handed perspective matrix. The DirectX way (with NDCz = [0 .. 1])
glm::mat4 PerspectiveRH( float fovy, float aspect, float zNear, float zFar )
{
    float yScale = 1.0f / tanf( fovy * 0.5f );
    float xScale = yScale / aspect;

    glm::mat4 result = {
        xScale, 0,              0,                       0,
        0,      yScale,         0,                       0,
        0,      0,      zFar / ( zNear - zFar ),        -1,
        0,      0,      zNear * zFar / ( zNear - zFar ), 0
    };

    return result;
}

void Camera::SetProjection( float fovy, float aspect, float zNear, float zFar )
{
    m_fVFOV = fovy;
    m_fAspect = aspect;
    m_fNear = zNear;
    m_fFar = zFar;
    //m_ProjectionMatrix = glm::perspective( glm::radians( fovy ), aspect, zNear, zFar );
    m_ProjectionMatrix = PerspectiveRH( glm::radians( fovy ), aspect, zNear, zFar );

    m_bViewProjectionInverseDirty = true;
}

float Camera::GetNearClipPlane() const
{
    return m_fNear;
}

float Camera::GetFarClipPlane() const
{
    return m_fFar;
}

float Camera::GetFOV() const
{
    return m_fVFOV;
}

void Camera::SetOrthographic( float left, float right, float top, float bottom )
{
    m_ProjectionMatrix = glm::ortho( left, right, bottom, top );
    m_bViewProjectionInverseDirty = true;
}

void Camera::AddPitch( float fPitch, Space space )
{
    switch ( space )
    {
    case Space::Local:
        m_Rotate = glm::angleAxis( glm::radians( fPitch ), m_Rotate * glm::vec3( 1, 0, 0 ) ) * m_Rotate;
        break;
    case Space::World:
        m_Rotate = glm::angleAxis( glm::radians( fPitch ), glm::vec3( 1, 0, 0 ) ) * m_Rotate;
    }

    m_bViewDirty = true;
}

void Camera::AddYaw( float fYaw, Space space )
{
    switch ( space )
    {
    case Space::Local:
        m_Rotate = glm::angleAxis( glm::radians( fYaw ), m_Rotate * glm::vec3( 0, 1, 0 ) ) * m_Rotate;
        break;
    case Space::World:
        m_Rotate = glm::angleAxis( glm::radians( fYaw ), glm::vec3( 0, 1, 0 ) ) * m_Rotate;
        break;
    }

    m_bViewDirty = true;
}

void Camera::AddRoll( float fRoll, Space space )
{
    switch ( space )
    {
    case Space::Local:
        m_Rotate = glm::angleAxis( glm::radians( fRoll ), m_Rotate * glm::vec3( 0, 0, 1 ) ) * m_Rotate;
        break;
    case Space::World:
        m_Rotate = glm::angleAxis( glm::radians( fRoll ), glm::vec3( 0, 0, 1 ) ) * m_Rotate;
        break;
    }
    m_bViewDirty = true;
}

void Camera::SetEulerAngles( const glm::vec3& eulerAngles )
{
    m_Rotate = glm::quat( glm::radians( eulerAngles ) );
    m_bViewDirty = true;
}

void Camera::AddRotation( const glm::quat& deltaRot )
{
    m_Rotate = m_Rotate * deltaRot;
    m_bViewDirty = true;
}

void Camera::TranslateX( float x, Space space )
{
    switch ( space )
    {
    case Camera::Space::Local:
        m_Translate += m_Rotate * glm::vec3( x, 0, 0 );
        break;
    case Camera::Space::World:
        m_Translate += glm::vec3( x, 0, 0 );
        break;
    }
    m_bViewDirty = true;
}

void Camera::TranslateY( float y, Space space )
{
    switch ( space )
    {
    case Space::Local:
        m_Translate += m_Rotate * glm::vec3( 0, y, 0 );
        break;
    case Space::World:
        m_Translate += glm::vec3( 0, y, 0 );
        break;
    }

    m_bViewDirty = true;
}

void Camera::TranslateZ( float z, Space space )
{
    switch ( space )
    {
    case Space::Local:
        m_Translate += m_Rotate * glm::vec3( 0, 0, z );
        break;
    case Space::World:
        m_Translate += glm::vec3( 0, 0, z );
        break;
    }

    m_bViewDirty = true;
}

void Camera::SetTranslate( const glm::vec3& translate )
{
    m_Translate = translate;
    m_bViewDirty = true;
}

void Camera::SetRotate( float pitch, float yaw, float roll )
{
    SetRotate( glm::vec3( pitch, yaw, roll ) );
}

void Camera::SetRotate( const glm::vec3& rotate )
{
    SetRotate( glm::quat( glm::radians( rotate ) ) );
}

void Camera::SetRotate( const glm::quat& rot )
{
    m_Rotate = rot;
    m_bViewDirty = true;
}

void Camera::SetPivotDistance( float pivotDistance )
{
    // Make sure pivot distance is always positive.
    m_PivotDistance = glm::max( pivotDistance, 0.0f );
    m_bViewDirty = true;
}

float Camera::GetPivotDistance() const
{
    return m_PivotDistance;
}

glm::vec3 Camera::GetPivotPoint() const
{
    // The camera actually pivots around its "translation" point.
    return m_Translate;
}


glm::vec3 Camera::GetTranslation() const
{
    return m_Translate;
}

glm::quat Camera::GetRotation() const
{
    return m_Rotate;
}

glm::vec3 Camera::GetEulerAngles() const
{
    return glm::degrees( glm::eulerAngles( m_Rotate ) );
}

void Camera::UpdateViewMatrix()
{
    if ( m_bViewDirty )
    {
        // Re-normalize the rotation quaternion.
        m_Rotate = glm::normalize( m_Rotate );
        glm::mat4 translateMatrix = glm::translate( m_Translate );
        glm::mat4 rotationMatrix = glm::toMat4( m_Rotate );
        glm::mat4 pivotMatrix = glm::translate( glm::vec3( 0, 0, m_PivotDistance ) );

        m_InverseViewMatrix = translateMatrix * rotationMatrix * pivotMatrix;
        m_ViewMatrix = glm::inverse( m_InverseViewMatrix );

        m_bViewProjectionInverseDirty = true;
        m_bViewDirty = false;
    }
}

void Camera::UpdateViewProjectionInverse()
{
    UpdateViewMatrix();

    if ( m_bViewProjectionInverseDirty )
    {
        m_ViewProjectionInverse = glm::inverse( m_ProjectionMatrix * m_ViewMatrix );
        m_bViewProjectionInverseDirty = false;
    }
}

void Camera::SetViewMatrix( const glm::mat4& viewMatrix )
{
    // Inverse the view matrix to get the world matrix of the camera
    glm::mat4 inverseView = glm::inverse( viewMatrix );

    // Extract the translation
    m_Translate = glm::vec3( inverseView[3] );

    // Extract the top-left 3x3 matrix to decompose the scale and rotation
    glm::mat3 tmp = glm::mat3( inverseView );

    // TODO: I don't know if any of the scales are negative.
    // I have to figure out how can I reliably determine if any of the scales are negative?
    float sx = glm::length( tmp[0] );
    float sy = glm::length( tmp[1] );
    float sz = glm::length( tmp[2] );

    glm::mat3 invScale = glm::mat3( glm::scale( glm::vec3( 1.0f / sx, 1.0f / sy, 1.0f / sz ) ) );
    // This will remove the scale factor (if there is one) so we can extract
    // the unit quaternion.
    tmp = tmp * invScale;
    m_Rotate = glm::toQuat( tmp );

    // The view matrix needs to be rebuilt from the rotation and translation components.
    m_bViewDirty = true;
}

glm::mat4 Camera::GetViewMatrix() const
{
    const_cast<Camera*>( this )->UpdateViewMatrix();
    return m_ViewMatrix;
}

glm::mat4 Camera::GetInverseViewMatrix() const
{
    const_cast<Camera*>( this )->UpdateViewMatrix();
    return m_InverseViewMatrix;
}

void Camera::SetProjectionMatrix( const glm::mat4& projectionMatrix )
{
    // TODO: Decompose the projection matrix?
    m_ProjectionMatrix = projectionMatrix;
    m_bViewProjectionInverseDirty = true;
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return m_ProjectionMatrix;
}

glm::mat4 Camera::GetViewProjectionInverseMatrix() const
{
    const_cast<Camera*>( this )->UpdateViewProjectionInverse();
    return m_ViewProjectionInverse;
}

Ray Camera::ScreenPointToRay( const glm::vec2& screenPoint ) const
{
    glm::mat4 clipToWorld = GetViewProjectionInverseMatrix();
    glm::vec4 clipPoint = glm::vec4( screenPoint, 1, 1 );
    clipPoint.x = ( screenPoint.x - m_Viewport.X ) / m_Viewport.Width;
    clipPoint.y = 1.0f - ( screenPoint.y - m_Viewport.Y ) / m_Viewport.Height;
    clipPoint = clipPoint * 2.0f - 1.0f;

    glm::vec3 p0 = m_Translate;
    glm::vec4 worldSpace = clipToWorld * clipPoint;
    glm::vec3 p1 = glm::vec3( worldSpace / worldSpace.w ); // glm::unProject( glm::vec3( screenPoint, 0), m_ViewMatrix, m_ProjectionMatrix, glm::vec4( m_Viewport.X, m_Viewport.Y, m_Viewport.Width, m_Viewport.Height ) );

    return Ray( p0, glm::normalize( p1 - p0 ) );
}

glm::vec3 Camera::ProjectOntoUnitSphere( glm::ivec2 screenPos )
{
    // Map the screen coordinates so that (0, 0) is the center of the viewport.
    screenPos -= glm::vec2( m_Viewport.Width, m_Viewport.Height ) * 0.5f;

    float x, y, z;
    // The radius of the unit sphere is 1/2 of the shortest dimension of the viewport.
    float radius = glm::min( m_Viewport.Width, m_Viewport.Height ) * 0.5f;

    // Now normalize the screen coordinates into the range [-1 .. 1].
    x = screenPos.x / radius;
    // The y-coordinate has to be inverted so that +1 is the top of the unit sphere
    // and -1 is the bottom of the unit sphere.
    y = -( screenPos.y / radius );

    float length_sqr = ( x * x ) + ( y * y );

    // If the screen point is mapped outside of the unit sphere
    if ( length_sqr > 1.0f )
    {
        float invLength = glm::inversesqrt( length_sqr );

        // Return the normalized point that is closest to the outside of the unit sphere.
        x *= invLength;
        y *= invLength;
        z = 0.0f;
    }
    else
    {
        // The point is on the inside of the unit sphere.
        z = glm::sqrt( 1.0f - length_sqr );
        // If we are "inside" the unit sphere, then 
        // invert the z component.
        // In a right-handed coordinate system, the "+z" axis of the unit
        // sphere points towards the viewer. If we are in the Unit sphere, we
        // want to project the point to the inside of the sphere and in this case
        // the z-axis we want to project on points away from the viewer (-z).
        if ( m_PivotDistance <= 0.0f )
        {
            z = -z;
        }
    }

    return glm::vec3( x, y, z );
}

void Camera::OnMousePressed( Core::MouseButtonEventArgs& e )
{
    m_PreviousPoint = ProjectOntoUnitSphere( glm::ivec2( e.X, e.Y ) );
}

void Camera::OnMouseMoved( Core::MouseMotionEventArgs& e )
{
    glm::vec3 currentPoint = ProjectOntoUnitSphere( glm::ivec2( e.X, e.Y ) );

    AddRotation( glm::quat( currentPoint, m_PreviousPoint ) );

    m_PreviousPoint = currentPoint;
}
