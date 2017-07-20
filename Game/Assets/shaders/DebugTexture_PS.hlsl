#include "Include/CommonInclude.hlsli"

Texture2D DebugTexture : register( t0 );

float4 DebugTexture_PS( VertexShaderOutput IN ) : SV_Target
{
    return DebugTexture.Sample( LinearRepeatSampler, IN.TexCoord.xy );
}

float4 DebugDepthTexture_PS( VertexShaderOutput IN ) : SV_Target
{
    return float4( DebugTexture.Sample( LinearRepeatSampler, IN.TexCoord.xy ).rrr, 1.0f );
}