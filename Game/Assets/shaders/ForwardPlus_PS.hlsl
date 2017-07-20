#include "Include/CommonInclude.hlsli"

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 16
#endif

LightingResult DoLighting( uint2 tileIndex, Material material, float4 eyePos, float4 P, float4 N )
{
    // Counters
    uint i;
    uint startOffset, lightCount;
    uint lightIndex;

    float4 V = normalize( eyePos - P );

    LightingResult totalResult = (LightingResult)0;

    // Get the start position and offset of the light in the light index list.
    startOffset = PointLightGrid[tileIndex].x;
    lightCount = PointLightGrid[tileIndex].y;
    
    // Iterate point lights.
    for ( i = 0; i < lightCount; ++i )
    {
        lightIndex = PointLightIndexList[startOffset+i];

        if ( !PointLights[lightIndex].Enabled ) continue;

        LightingResult result = DoPointLight( PointLights[lightIndex], material, V, P, N );

        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
    }

    // Get the start position and offset of the light in the light index list.
    startOffset = SpotLightGrid[tileIndex].x;
    lightCount = SpotLightGrid[tileIndex].y;

    // Iterate spot lights.
    for ( i = 0; i < lightCount; ++i )
    {
        lightIndex = SpotLightIndexList[startOffset+i];

        if ( !SpotLights[lightIndex].Enabled ) continue;

        LightingResult result = DoSpotLight( SpotLights[lightIndex], material, V, P, N );

        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
    }

    // Iterate directional lights.
    for ( i = 0; i < LightCountsCB.NumDirectionalLights; ++i )
    {
        if ( !DirectionalLights[i].Enabled ) continue;

        LightingResult result = DoDirectionalLight( DirectionalLights[i], material, V, P, N );

        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
    }

    return totalResult;
}

[earlydepthstencil]
float4 main( VertexShaderOutput IN ) : SV_Target
{
    // Everything is in view space.
    float4 eyePos = { 0, 0, 0, 1 };
    Material material = MaterialCB;

    float4 diffuse = material.DiffuseColor;
    if ( material.HasDiffuseTexture == true )
    {
        float4 diffuseTex = DiffuseTexture.Sample( LinearRepeatSampler, IN.TexCoord.xy );
        if ( any( diffuse.rgb ) )
        {
            diffuse *= diffuseTex;
        }
        else
        {
            diffuse = diffuseTex;
        }
    }

    // By default, use the alpha from the diffuse component.
    float alpha = diffuse.a;
    if ( material.HasOpacityTexture == true )
    {
        // If the material has an opacity texture, use that to override the diffuse alpha.
        alpha = OpacityTexture.Sample( LinearRepeatSampler, IN.TexCoord.xy ).r;
    }

    float4 ambient = material.AmbientColor;
    if ( material.HasAmbientTexture == true )
    {
        float4 ambientTex = AmbientTexture.Sample( LinearRepeatSampler, IN.TexCoord.xy );
        if ( any( ambient.rgb ) )
        {
            ambient *= ambientTex;
        }
        else
        {
            ambient = ambientTex;
        }
    }

    // Combine the global ambient term.
    ambient *= material.GlobalAmbient;

    float4 emissive = material.EmissiveColor;
    if ( material.HasEmissiveTexture == true )
    {
        float4 emissiveTex = EmissiveTexture.Sample( LinearRepeatSampler, IN.TexCoord.xy );
        if ( any( emissive.rgb ) )
        {
            emissive *= emissiveTex;
        }
        else
        {
            emissive = emissiveTex;
        }
    }

    if ( material.HasSpecularPowerTexture == true )
    {
        material.SpecularPower = SpecularPowerTexture.Sample( LinearRepeatSampler, IN.TexCoord.xy ).r * material.SpecularScale;
    }

    float4 N;

    // Normal mapping
    if ( material.HasNormalTexture == true )
    {
        // For scense with normal mapping, I don't have to invert the binormal.
        float3x3 TBN = float3x3( normalize( IN.TangentVS ),
                                 normalize( IN.BitangentVS ),
                                 normalize( IN.NormalVS ) );

        N = DoNormalMapping( TBN, NormalTexture, LinearRepeatSampler, IN.TexCoord.xy );
    }
    // Bump mapping
    else if ( material.HasBumpTexture == true )
    {
        // For most scenes using bump mapping, I have to invert the binormal.
        float3x3 TBN = float3x3( normalize( IN.TangentVS ),
                                 normalize( -IN.BitangentVS ),
                                 normalize( IN.NormalVS ) );

        N = DoBumpMapping( TBN, BumpTexture, LinearRepeatSampler, IN.TexCoord.xy, material.BumpIntensity );
    }
    // Just use the normal from the model.
    else
    {
        N = normalize( float4( IN.NormalVS, 0 ) );
    }

    // Get the index of the current pixel in the light grid.
    uint2 tileIndex = uint2( floor( IN.Position.xy / BLOCK_SIZE ) );

    LightingResult lit = DoLighting( tileIndex, material, eyePos, IN.PositionVS, N );

    diffuse *= float4( lit.Diffuse, 1.0f );

    float4 specular = 0;
    if ( material.SpecularPower > 1.0f ) // If specular power is too low, don't use it.
    {
        specular = material.SpecularColor;
        if ( material.HasSpecularTexture == true )
        {
            float4 specularTex = SpecularTexture.Sample( LinearRepeatSampler, IN.TexCoord.xy );
            if ( any( specular.rgb ) )
            {
                specular *= specularTex;
            }
            else
            {
                specular = specularTex;
            }
        }
        specular *= float4( lit.Specular, 1.0f );
    }
    
   return float4( ( ambient + emissive + diffuse + specular ).rgb, alpha * material.Opacity );
}