#include "Include/CommonInclude.hlsli"

[earlydepthstencil]
float4 main( VertexShaderOutput IN ) : SV_TARGET
{
    uint3 clusterIndex3D = ComputeClusterIndex3D( IN.Position.xy, IN.PositionVS.z );
    uint clusterIndex1D = ComputeClusterIndex1D( clusterIndex3D );

    RWClusterFlags[clusterIndex1D] = true;

    // When clustering samples, the output color is not important but may be useful for 
    // debug purposes.
    float4 clusterColor = ClusterColors[clusterIndex1D];
    return float4( clusterColor.rgb, 0.5f );
}