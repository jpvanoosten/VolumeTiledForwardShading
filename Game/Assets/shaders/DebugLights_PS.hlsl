#include "Include/CommonInclude.hlsli"

float4 PointLight_PS( VertexShaderOutput IN ) : SV_TARGET
{
    PointLight light = PointLights[IN.InstanceID];

    float4 N = normalize( float4( IN.NormalVS, 0 ) );

    return float4( ( light.Color * saturate( N.z ) ), MaterialCB.Opacity );
}

float4 SpotLight_PS( VertexShaderOutput IN ) : SV_TARGET
{
    SpotLight light = SpotLights[IN.InstanceID];

    float4 N = normalize( float4( IN.NormalVS, 0 ) );

    return float4( ( light.Color * saturate( N.z ) ), MaterialCB.Opacity );
}