// Axis-Aligned bounding box
struct AABB
{
    float4 Min;
    float4 Max;
};

struct VertexShaderOutput
{
    float4 Min          : AABB_MIN;  // Min vertex position in view space.
    float4 Max          : AABB_MAX;  // Max vertex position in view space.
    float4 Color        : COLOR;     // Cluster color.
};

struct GeometryShaderOutput
{
    float4 Color        : COLOR;
    float4 Position     : SV_POSITION;          // Clip space position.
};

cbuffer DebugClustersCB : register( b0 )
{
    float4x4 View;          // Transform from previous view point to current view point.
    float4x4 Projection;    // Projection to clip space.
}

// Unique clusters stores a list of cluster IDs that 
// contain a sample.
StructuredBuffer<uint> UniqueClusters : register( t0 );
StructuredBuffer<AABB> ClusterAABBs : register( t1 );
StructuredBuffer<float4> ClusterColors : register( t2 );

// Root Signature
// 0. cbuffer DebugClustersCB : register( b0 )
// 1. StructuredBuffer<uint> UniqueClusters : register( t0 );
//    StructuredBuffer<AABB> ClusterAABBs : register( t1 );
//    StructuredBuffer<float4> ClusterColors : register( t2 );
#define DebugClusters_RS \
    "RootFlags(0)," \
    "CBV( b0 )," \
    "DescriptorTable( SRV(t0, numDescriptors=3 ) )"

[RootSignature( DebugClusters_RS )]
VertexShaderOutput main_VS( uint VertexID : SV_VertexID )
{
    uint clusterID = UniqueClusters[VertexID];

    VertexShaderOutput OUT = (VertexShaderOutput)0;

    AABB aabb = ClusterAABBs[clusterID];

    OUT.Min = aabb.Min;
    OUT.Max = aabb.Max;
    OUT.Color = ClusterColors[clusterID];

    return OUT;
}

// Geometry shader to convert AABB to cube.
[maxvertexcount(16)]
void main_GS( point VertexShaderOutput IN[1], inout TriangleStream<GeometryShaderOutput> OutputStream )
{
    float4 min = IN[0].Min;
    float4 max = IN[0].Max;

    float4x4 ViewProj = mul( Projection, View );

    // Clip space position
    GeometryShaderOutput OUT = (GeometryShaderOutput)0;

    // AABB vertices
    const float4 Pos[8] = {
        float4( min.x, min.y, min.z, 1.0f ),    // 0
        float4( min.x, min.y, max.z, 1.0f ),    // 1
        float4( min.x, max.y, min.z, 1.0f ),    // 2
        float4( min.x, max.y, max.z, 1.0f ),    // 3
        float4( max.x, min.y, min.z, 1.0f ),    // 4
        float4( max.x, min.y, max.z, 1.0f ),    // 5
        float4( max.x, max.y, min.z, 1.0f ),    // 6
        float4( max.x, max.y, max.z, 1.0f )     // 7
    };

    // Colors (to test correctness of AABB vertices)
    const float4 Col[8] = {
        float4( 0.0f, 0.0f, 0.0f, 1.0f ),       // Black
        float4( 0.0f, 0.0f, 1.0f, 1.0f ),       // Blue
        float4( 0.0f, 1.0f, 0.0f, 1.0f ),       // Green
        float4( 0.0f, 1.0f, 1.0f, 1.0f ),       // Cyan
        float4( 1.0f, 0.0f, 0.0f, 1.0f ),       // Red
        float4( 1.0f, 0.0f, 1.0f, 1.0f ),       // Magenta
        float4( 1.0f, 1.0f, 0.0f, 1.0f ),       // Yellow
        float4( 1.01, 1.0f, 1.0f, 1.0f )        // White
    };

    const uint Index[18] = {
        0, 1, 2,
        3, 6, 7,
        4, 5, -1,
        2, 6, 0,
        4, 1, 5,
        3, 7, -1
    };

    [unroll]
    for ( uint i = 0; i < 18; ++i )
    {
        if ( Index[i] == (uint)-1 )
        {
            OutputStream.RestartStrip();
        }
        else
        {
            OUT.Position = mul( ViewProj, Pos[Index[i]] );
            OUT.Color = IN[0].Color;
            //OUT.Color = Col[Index[i]];
            OutputStream.Append( OUT );
        }
    }
}

float4 main_PS( GeometryShaderOutput IN ) : SV_Target
{
    return float4( IN.Color.rgb, 0.2f );
}