#include "Include/CommonInclude.hlsli"

void main( VertexShaderOutput IN )
{
    float alpha = 1;
    if ( MaterialCB.HasOpacityTexture )
    {
        alpha = OpacityTexture.Sample( LinearRepeatSampler, IN.TexCoord.xy ).r;
    }
    else if ( MaterialCB.HasDiffuseTexture )
    {
        alpha = DiffuseTexture.Sample( LinearRepeatSampler, IN.TexCoord.xy ).a;
    }
    
    if ( alpha < MaterialCB.AlphaThreshold )
        discard;
}