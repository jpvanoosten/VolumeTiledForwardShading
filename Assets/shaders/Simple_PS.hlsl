#include "Include/CommonInclude.hlsli"

[earlydepthstencil]
float4 main( VertexShaderOutput IN ) : SV_Target
{
    //float4 diffuseTex = DiffuseTexture.SampleLevel( LinearRepeatSampler, IN.TexCoord.xy, 0 );
    float4 diffuseTex = DiffuseTexture.Sample( AnisotropicSampler, IN.TexCoord.xy );

    return diffuseTex; // * IN.NormalVS.z;
}