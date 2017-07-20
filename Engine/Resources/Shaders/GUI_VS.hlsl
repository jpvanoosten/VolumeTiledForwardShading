#define GUI_RootSignature \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
    "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX)," \
    "DescriptorTable( SRV(t0), visibility=SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s0, filter=FILTER_MIN_MAG_POINT_MIP_LINEAR, visibility=SHADER_VISIBILITY_PIXEL)" \

cbuffer vertexBuffer : register(b0)
{
    float4x4 ProjectionMatrix;
};

struct VS_INPUT
{
    float2 pos : POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

[RootSignature( GUI_RootSignature )]
PS_INPUT VS_gui( VS_INPUT input )
{
    PS_INPUT output;
    
    output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));
    output.col = input.col;
    output.uv  = input.uv;

    return output;
}