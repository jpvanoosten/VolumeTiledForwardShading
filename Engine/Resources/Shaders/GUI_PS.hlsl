struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

SamplerState Sampler0 : register( s0 );
Texture2D Texture0 : register( t0 );

float4 PS_gui(PS_INPUT input) : SV_Target
{
    float4 out_col = input.col * Texture0.Sample(Sampler0, input.uv);
    return out_col;
}
