#include "Include/CommonInclude.hlsli"

#define NUM_THREADS 1024

[RootSignature( UpdateLights_RS )]
[numthreads(NUM_THREADS, 1, 1)]
void main( ComputeShaderInput IN )
{
    uint threadID = IN.DispatchThreadID.x;

    // Update point lights.
    if ( threadID < LightCountsCB.NumPointLights )
    {
        PointLight pointLight = RWPointLights[threadID];

        pointLight.PositionWS = mul( UpdateLightsCB.WorldMatrix, pointLight.PositionWS );
        pointLight.PositionVS = mul( UpdateLightsCB.ViewMatrix, float4( pointLight.PositionWS.xyz, 1 ) );

        RWPointLights[threadID] = pointLight;
    }

    // Update spot lights
    if ( threadID < LightCountsCB.NumSpotLights )
    {
        SpotLight spotLight = RWSpotLights[threadID];

        spotLight.PositionWS = mul( UpdateLightsCB.WorldMatrix, spotLight.PositionWS );
        spotLight.DirectionWS = mul( UpdateLightsCB.WorldMatrix, spotLight.DirectionWS );

        spotLight.PositionVS = mul( UpdateLightsCB.ViewMatrix, float4( spotLight.PositionWS.xyz, 1 ) );
        spotLight.DirectionVS = normalize( mul( UpdateLightsCB.ViewMatrix, float4( spotLight.DirectionWS.xyz, 0 ) ) );

        RWSpotLights[threadID] = spotLight;
    }

    // Update directional lights
    if ( threadID < LightCountsCB.NumDirectionalLights )
    {
        DirectionalLight directionalLight = RWDirectionalLights[threadID];

        directionalLight.DirectionWS = mul( UpdateLightsCB.WorldMatrix, directionalLight.DirectionWS );
        directionalLight.DirectionVS = normalize( mul( UpdateLightsCB.ViewMatrix, float4( directionalLight.DirectionWS.xyz, 0 ) ) );

        RWDirectionalLights[threadID] = directionalLight;
    }
}