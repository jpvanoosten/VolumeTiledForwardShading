/**
 * Colors a pixel based on the number of lights covering that pixel.
 * This shader uses the result of the "Assign Lights to Clusters" stage.
 */

#include "Include/CommonInclude.hlsli"
[RootSignature( CountLights_RS )]
[numthreads(32, 32, 1)]
void main( ComputeShaderInput IN )
{
    uint i, j, k;
    uint lightCount = 0;
    // Screenspace texture coordinate.
    int2 texCoord = IN.DispatchThreadID.xy;

    float fDepth = DepthTexture.Load( texCoord, 0 );

    // Convert the screen space coordinates into view space.
    float4 positionVS = ScreenToView( float4( IN.DispatchThreadID.xy, fDepth, 1.0f ) );

    // Compute the 3D cluster index.
    uint3 clusterIndex3D = ComputeClusterIndex3D( texCoord, positionVS.z );
    // Convert to 1D cluster index.
    uint clusterIndex1D = ComputeClusterIndex1D( clusterIndex3D );

    lightCount += PointLightGrid_Cluster[clusterIndex1D].y;
    lightCount += SpotLightGrid_Cluster[clusterIndex1D].y;

    // Update the debug texture output.
/*    if ( IN.DispatchThreadID.x % ClusterCB.Size.x == 0 || IN.DispatchThreadID.y % ClusterCB.Size.y == 0 )
    {
        RWDebugTexture[texCoord] = float4( 1, 1, 1, 0.2f );
    }
    else */if ( lightCount > 0 )
    {
        float normalizedLightCount = lightCount / 100.0f;
        float4 color = LightCountHeatMap.SampleLevel( LinearClampSampler, float2( normalizedLightCount, 0 ), 0 );
        RWDebugTexture[texCoord] = float4( color.rgb, 0.9f );
    }
    else
    {
        RWDebugTexture[texCoord] = float4( 0, 0, 0, 0.9f );
    }
}