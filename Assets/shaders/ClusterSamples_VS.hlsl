#include "Include/CommonInclude.hlsli"

[RootSignature( ClusterSamples_RS )]
VertexShaderOutput main( AppData IN )
{
    VertexShaderOutput OUT;

    OUT.Position = mul( PerObjectDataCB.ModelViewProjection, float4( IN.Position, 1.0f ) );
    OUT.PositionVS = mul( PerObjectDataCB.ModelView, float4( IN.Position, 1.0f ) );
    OUT.NormalVS = mul( ( float3x3 )PerObjectDataCB.InverseTransposeModelView, IN.Normal );
    OUT.TangentVS = mul( ( float3x3 )PerObjectDataCB.InverseTransposeModelView, IN.Tangent );
    OUT.BitangentVS = mul( ( float3x3 )PerObjectDataCB.InverseTransposeModelView, IN.Bitangent );
    OUT.TexCoord = IN.TexCoord;
    OUT.InstanceID = IN.InstanceID;

    return OUT;
}
