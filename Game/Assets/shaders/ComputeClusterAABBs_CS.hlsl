#include "Include/CommonInclude.hlsli"

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 1024
#endif

// A compute shader kernel to compute the AABB for the 
// 3D cluster grid.
[RootSignature( ComputeClusterAABBs_RS )]
[numthreads( BLOCK_SIZE, 1, 1 )]
void main( ComputeShaderInput IN )
{
    uint clusterIndex1D = IN.DispatchThreadID.x;
    // Convert the 1D cluster index into a 3D index in the cluster grid.
    uint3 clusterIndex3D = ComputeClusterIndex3D( clusterIndex1D );

    // Compute the near and far planes for cluster K.
    Plane nearPlane = { 0.0f, 0.0f, 1.0f, -ClusterCB.ViewNear * pow( abs( ClusterCB.NearK ), clusterIndex3D.z ) };
    Plane farPlane =  { 0.0f, 0.0f, 1.0f, -ClusterCB.ViewNear * pow( abs( ClusterCB.NearK ), clusterIndex3D.z + 1 ) };

    // The top-left point of cluster K in screen space.
    float4 pMin = float4( clusterIndex3D.xy * ClusterCB.Size.xy, 1.0f, 1.0f );
    // The bottom-right point of cluster K in screen space.
    float4 pMax = float4( ( clusterIndex3D.xy + 1 ) * ClusterCB.Size.xy, 1.0f, 1.0f );

    // Transform the screen space points to view space.
    pMin = ScreenToView( pMin );
    pMax = ScreenToView( pMax );

    // Find the min and max points on the near and far planes.
    float3 nearMin, nearMax, farMin, farMax;
    // Origin (camera eye position)
    float3 eye = float3( 0, 0, 0 );
    IntersectLinePlane( eye, (float3)pMin, nearPlane, nearMin );
    IntersectLinePlane( eye, (float3)pMax, nearPlane, nearMax );
    IntersectLinePlane( eye, (float3)pMin, farPlane, farMin );
    IntersectLinePlane( eye, (float3)pMax, farPlane, farMax );

    float3 aabbMin = min( nearMin, min( nearMax, min( farMin, farMax ) ) );
    float3 aabbMax = max( nearMin, max( nearMax, max( farMin, farMax ) ) );

    AABB aabb = { float4( aabbMin, 1.0f ), float4( aabbMax, 1.0f ) };
    RWClusterAABBs[clusterIndex1D] = aabb;
}