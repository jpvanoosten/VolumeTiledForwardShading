#include "Include/CommonInclude.hlsli"

[RootSignature( DebugLightsVS_RS )]
VertexShaderOutput PointLight_VS( AppData IN )
{
    PointLight light = PointLights[IN.InstanceID];

    float4x4 LightMatrix = {
        light.Range, 0.0f, 0.0f, light.PositionWS.x,
        0.0f, light.Range, 0.0f, light.PositionWS.y,
        0.0f, 0.0f, light.Range, light.PositionWS.z,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    float4x4 M = mul( LightMatrix, PerObjectDataCB.Model );
    float4x4 MV = mul( PerObjectDataCB.View, M );
    float4x4 MVP = mul( PerObjectDataCB.Projection, MV );

    VertexShaderOutput OUT;

    OUT.Position = mul( MVP, float4( IN.Position, 1.0f ) );
    OUT.PositionVS = mul( MV, float4( IN.Position, 1.0f ) );
    OUT.NormalVS = mul( (float3x3)MV, IN.Normal );
    OUT.TangentVS = mul( (float3x3)MV, IN.Tangent );
    OUT.BitangentVS = mul( (float3x3)MV, IN.Bitangent );
    OUT.TexCoord = IN.TexCoord;
    OUT.InstanceID = IN.InstanceID;

	return OUT;
}

[RootSignature( DebugLightsVS_RS )]
VertexShaderOutput SpotLight_VS( AppData IN )
{
    SpotLight light = SpotLights[IN.InstanceID];

    float4x4 TranslationMatrix = {
        1.0f, 0.0f, 0.0f, light.PositionWS.x,
        0.0f, 1.0f, 0.0f, light.PositionWS.y,
        0.0f, 0.0f, 1.0f, light.PositionWS.z,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    float tangent = tan( radians( light.SpotlightAngle ) );
    float4x4 ScaleMatrix = {
        light.Range * tangent, 0.0f                 , 0.0f       , 0.0f,
        0.0f                 , light.Range * tangent, 0.0f       , 0.0f,
        0.0f                 , 0.0f                 , light.Range, 0.0f,
        0.0f                 , 0.0f                 , 0.0f       , 1.0f
    };

    float4x4 RotationMatrix = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    // Compute a rotation matirx that rotates from the global z axis to the direction of the spot light.
    // Source: http://www.3dgep.com/3d-math-primer-for-game-programmers-matrices/#Rotation_about_an_arbitrary_axis
    float3 zDir = float3( 0, 0, 1 );
    float3 lDir = normalize( light.DirectionWS.xyz );
    float3 v = cross( zDir, lDir ); // axis of rotation.
    float s = length( v ); // sine of the angle between the two vectors.
    if ( s > 0.0f )
    {
        v = v / s; // Normalize v.
        float c = dot( zDir, lDir ); // cosine of the angle between the two vectors.

        float x = v.x;    float y = v.y;    float z = v.z;
        float xx = x * x; float yy = y * y; float zz = z * z;
        float xy = x * y; float xz = x * z; float yz = y * z;
        float c2 = 1 - c;

        RotationMatrix = float4x4(
            xx * c2 + c    , xy * c2 - z * s, xz * c2 + y * s, 0.0f,
            xy * c2 + z * s, yy * c2 + c    , yz * c2 - x * s, 0.0f,
            xz * c2 - y * s, yz * c2 + x * s, zz * c2 + c    , 0.0f,
                  0.0f     ,      0.0f      ,     0.0f       , 1.0f
        );
    }

    float4x4 M = mul( TranslationMatrix, mul( RotationMatrix,  mul( ScaleMatrix, PerObjectDataCB.Model ) ) );
    float4x4 MV = mul( PerObjectDataCB.View, M );
    float4x4 MVP = mul( PerObjectDataCB.Projection, MV );

    VertexShaderOutput OUT;

    OUT.Position = mul( MVP, float4( IN.Position, 1.0f ) );
    OUT.PositionVS = mul( MV, float4( IN.Position, 1.0f ) );
    OUT.NormalVS = mul( (float3x3)MV, IN.Normal );
    OUT.TangentVS = mul( (float3x3)MV, IN.Tangent );
    OUT.BitangentVS = mul( (float3x3)MV, IN.Bitangent );
    OUT.TexCoord = IN.TexCoord;
    OUT.InstanceID = IN.InstanceID;

    return OUT;
}