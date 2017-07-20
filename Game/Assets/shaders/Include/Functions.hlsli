#ifndef __COMMON_INCLUDE_HLSL__
#error Do not include this header directly. Only include this file via CommonInclude.hlsli.
#endif

float3 ExpandNormal( float3 n )
{
    return n * 2.0f - 1.0f;
}

float4 DoNormalMapping( float3x3 TBN, Texture2D tex, sampler s, float2 uv )
{
    float3 normal = tex.Sample( s, uv ).xyz;
    normal = ExpandNormal( normal );

    // Transform normal from tangent space to view space.
    normal = mul( normal, TBN );
    return normalize( float4( normal, 0 ) );
}

float4 DoBumpMapping( float3x3 TBN, Texture2D tex, sampler s, float2 uv, float bumpScale )
{
    // Sample the heightmap at the current texture coordinate.
    float height_00 = tex.Sample( s, uv ).r * bumpScale;
    // Sample the heightmap in the U texture coordinate direction.
    float height_10 = tex.Sample( s, uv, int2( 1, 0 ) ).r * bumpScale;
    // Sample the heightmap in the V texture coordinate direction.
    float height_01 = tex.Sample( s, uv, int2( 0, 1 ) ).r * bumpScale;

    float3 p_00 = { 0, 0, height_00 };
    float3 p_10 = { 1, 0, height_10 };
    float3 p_01 = { 0, 1, height_01 };

    // normal = tangent x bitangent
    float3 normal = cross( normalize( p_10 - p_00 ), normalize( p_01 - p_00 ) );

    // Transform normal from tangent space to view space.
    normal = mul( normal, TBN );

    return float4( normal, 0.0f );
}

float DoDiffuse( float4 N, float4 L )
{
    float NdotL = max( dot( N, L ), 0.0f );
    return NdotL;
}

float DoSpecular( Material material, float4 V, float4 L, float4 N )
{
    float4 R = normalize( reflect( -L, N ) );
    float RdotV = max( dot( R, V ), 0.0f );

    return pow( RdotV, material.SpecularPower );
}

// Compute the attenuation based on the range of the light.
float DoAttenuation( float range, float d )
{
    return 1.0f - smoothstep( range * 0.75f, range, d );
}

float DoSpotCone( SpotLight light, float4 L )
{
    // If the cosine angle of the light's direction 
    // vector and the vector from the light source to the point being 
    // shaded is less than minCos, then the spotlight contribution will be 0.
    float minCos = cos( radians( light.SpotlightAngle ) );
    // If the cosine angle of the light's direction vector
    // and the vector from the light source to the point being shaded
    // is greater than maxCos, then the spotlight contribution will be 1.
    float maxCos = lerp( minCos, 1, 0.5f );
    float cosAngle = dot( light.DirectionVS, -L );
    // Blend between the maxixmum and minimum cosine angles.
    return smoothstep( minCos, maxCos, cosAngle );
}

LightingResult DoPointLight( PointLight light, Material material, float4 V, float4 P, float4 N )
{
    LightingResult result;

    float4 L = light.PositionVS - P;
    float distance = length( L );
    L = L / distance;

    float attenuation = DoAttenuation( light.Range, distance );

    result.Diffuse = light.Color * DoDiffuse( N, L ) * attenuation * light.Intensity;
    result.Specular = light.Color * DoSpecular( material, V, L, N ) * attenuation * light.Intensity;

    return result;
}

LightingResult DoDirectionalLight( DirectionalLight light, Material material, float4 V, float4 P, float4 N )
{
    LightingResult result;

    float4 L = normalize( -light.DirectionVS );

    result.Diffuse = light.Color * DoDiffuse( N, L ) * light.Intensity;
    result.Specular = light.Color * DoSpecular( material, V, L, N ) * light.Intensity;

    return result;
}

LightingResult DoSpotLight( SpotLight light, Material material, float4 V, float4 P, float4 N )
{
    LightingResult result;

    float4 L = light.PositionVS - P;
    float distance = length( L );
    L = L / distance;

    float attenuation = DoAttenuation( light.Range, distance );
    float spotIntensity = DoSpotCone( light, L );

    result.Diffuse = light.Color * DoDiffuse( N, L ) * attenuation * spotIntensity * light.Intensity;
    result.Specular = light.Color * DoSpecular( material, V, L, N ) * attenuation * spotIntensity * light.Intensity;

    return result;
}

// Convert clip space coordinates to view space
float4 ClipToView( float4 clip )
{
    // View space position.
    float4 view = mul( Camera.InverseProjection, clip );
    // Perspecitive projection.
    view = view / view.w;

    return view;
}

// Convert screen space coordinates to view space.
float4 ScreenToView( float4 screen )
{
    // Convert to normalized texture coordinates in the range [0 .. 1].
    float2 texCoord = screen.xy / Camera.ScreenDimensions;

    // Convert to clip space
    float4 clip = float4( float2( texCoord.x, 1.0f - texCoord.y ) * 2.0f - 1.0f, screen.z, screen.w );

    return ClipToView( clip );
}

// Compute a plane from 3 noncollinear points that form a triangle.
// This equation assumes a right-handed (counter-clockwise winding order) 
// coordinate system to determine the direction of the plane normal.
Plane ComputePlane( float3 p0, float3 p1, float3 p2 )
{
    Plane plane;

    float3 v0 = p1 - p0;
    float3 v2 = p2 - p0;

    plane.N = normalize( cross( v0, v2 ) );

    // Compute the distance to the origin using p0.
    plane.d = dot( plane.N, p0 );

    return plane;
}

// Check to see if a sphere is fully behind (inside the negative halfspace of) a plane.
// Source: Real-time collision detection, Christer Ericson (2005)
bool SphereInsidePlane( Sphere sphere, Plane plane )
{
    return dot( plane.N, sphere.c ) - plane.d < -sphere.r;
}

// Check to see if a point is fully behind (inside the negative halfspace of) a plane.
bool PointInsidePlane( float3 p, Plane plane )
{
    return dot( plane.N, p ) - plane.d < 0;
}

// Check to see if a cone if fully behind (inside the negative halfspace of) a plane.
// Source: Real-time collision detection, Christer Ericson (2005)
bool ConeInsidePlane( Cone cone, Plane plane )
{
    // Compute the farthest point on the end of the cone to the positive space of the plane.
    float3 m = cross( cross( plane.N, cone.d ), cone.d );
    float3 Q = cone.T + cone.d * cone.h - m * cone.r;

    // The cone is in the negative halfspace of the plane if both
    // the tip of the cone and the farthest point on the end of the cone to the 
    // positive halfspace of the plane are both inside the negative halfspace 
    // of the plane.
    return PointInsidePlane( cone.T, plane ) && PointInsidePlane( Q, plane );
}

// Check to see of a light is partially contained within the frustum.
// Source: Real-time collision detection, Christer Ericson (2005)
bool SphereInsideFrustum( Sphere sphere, Frustum frustum, float zNear, float zFar )
{
    bool result = true;

    // First check depth
    // Note: Here, the view vector points in the -Z axis so the 
    // far depth value will be approaching -infinity.
    if ( sphere.c.z - sphere.r > zNear || sphere.c.z + sphere.r < zFar )
    {
        result = false;
    }

    // Then check frustum planes
    for ( int i = 0; i < 4 && result; i++ )
    {
        if ( SphereInsidePlane( sphere, frustum.planes[i] ) )
        {
            result = false;
        }
    }

    return result;
}

// Compute the square distance between a point p and an AABB b.
// Source: Real-time collision detection, Christer Ericson (2005)
float SqDistancePointAABB( float3 p, AABB b )
{
    float sqDistance = 0.0f;

    for ( int i = 0; i < 3; ++i )
    {
        float v = p[i];

        if ( v < b.Min[i] ) sqDistance += pow( b.Min[i] - v, 2 );
        if ( v > b.Max[i] ) sqDistance += pow( v - b.Max[i], 2 );
    }

    return sqDistance;
}

// Check to see if a sphere is interesecting an AABB
// Source: Real-time collision detection, Christer Ericson (2005)
bool SphereInsideAABB( Sphere sphere, AABB aabb )
{
    float sqDistance = SqDistancePointAABB( sphere.c, aabb );

    return sqDistance <= sphere.r * sphere.r;
}

// Check to see if on AABB intersects another AABB.
// Source: Real-time collision detection, Christer Ericson (2005)
bool AABBIntersectAABB( AABB a, AABB b )
{
    bool result = true;

    [unroll]
    for ( int i = 0; i < 3; ++i )
    {
        result = result && ( a.Max[i] >= b.Min[i] && a.Min[i] <= b.Max[i] );
    }

    return result;
}

bool ConeInsideFrustum( Cone cone, Frustum frustum, float zNear, float zFar )
{
    bool result = true;

    Plane nearPlane = { float3( 0, 0, -1 ), -zNear };
    Plane farPlane = { float3( 0, 0, 1 ), zFar };

    // First check the near and far clipping planes.
    if ( ConeInsidePlane( cone, nearPlane ) || ConeInsidePlane( cone, farPlane ) )
    {
        result = false;
    }

    // Then check frustum planes
    for ( int i = 0; i < 4 && result; i++ )
    {
        if ( ConeInsidePlane( cone, frustum.planes[i] ) )
        {
            result = false;
        }
    }

    return result;
}

/**
 * Convert a 1D cluster index into a 3D cluster index.
 */
uint3 ComputeClusterIndex3D( uint clusterIndex1D )
{
    uint i = clusterIndex1D % ClusterCB.GridDim.x;
    uint j = clusterIndex1D % ( ClusterCB.GridDim.x * ClusterCB.GridDim.y ) / ClusterCB.GridDim.x;
    uint k = clusterIndex1D / ( ClusterCB.GridDim.x * ClusterCB.GridDim.y );

    return uint3( i, j, k );
}

/**
 * Convert the 3D cluster index into a 1D cluster index.
 */
uint ComputeClusterIndex1D( uint3 clusterIndex3D )
{
    return clusterIndex3D.x + ( ClusterCB.GridDim.x * ( clusterIndex3D.y + ClusterCB.GridDim.y * clusterIndex3D.z ) );
}

/**
* Compute the 3D cluster index from a 2D screen position and Z depth in view space.
* source: Clustered deferred and forward shading (Olsson, Billeter, Assarsson, 2012)
*/
uint3 ComputeClusterIndex3D( float2 screenPos, float viewZ )
{
    uint i = screenPos.x / ClusterCB.Size.x;
    uint j = screenPos.y / ClusterCB.Size.y;
    // It is assumed that view space z is negative (right-handed coordinate system)
    // so the view-space z coordinate needs to be negated to make it positive.
    uint k = log( -viewZ / ClusterCB.ViewNear ) * ClusterCB.LogGridDimY;

    return uint3( i, j, k );
}

/**
 * Find the intersection of a line segment with a plane.
 * This function will return true if an intersection point
 * was found or false if no intersection could be found.
 * Source: Real-time collision detection, Christer Ericson (2005)
 */
bool IntersectLinePlane( float3 a, float3 b, Plane p, out float3 q )
{
    float3 ab = b - a;

    float t = ( p.d - dot( p.N, a ) ) / dot( p.N, ab );

    bool intersect = ( t >= 0.0f && t <= 1.0f );

    q = float3( 0, 0, 0 );
    if ( intersect )
    {
        q = a + t * ab;
    }

    return intersect;
}