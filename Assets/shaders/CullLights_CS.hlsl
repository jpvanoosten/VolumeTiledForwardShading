#include "Include/CommonInclude.hlsli"

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 16
#endif

typedef uint RenderPass;
static const RenderPass OPAQUE = 0;
static const RenderPass TRANSPARENT = 1;

// Group shared variables.
groupshared uint uMinDepth;
groupshared uint uMaxDepth;
groupshared Frustum GroupFrustum;

// Group shared light lists.
groupshared uint PointLightCount_OPAQUE;
groupshared uint PointLightCount_TRANSPARENT;
groupshared uint SpotLightCount_OPAQUE;
groupshared uint SpotLightCount_TRANSPARENT;
groupshared uint PointLightIndexStartOffset_OPAQUE;
groupshared uint PointLightIndexStartOffset_TRANSPARENT;
groupshared uint SpotLightIndexStartOffset_OPAQUE;
groupshared uint SpotLightIndexStartOffset_TRANSPARENT;
groupshared uint PointLightList_OPAQUE[1024];
groupshared uint PointLightList_TRANSPARENT[1024];
groupshared uint SpotLightList_OPAQUE[1024];
groupshared uint SpotLightList_TRANSPARENT[1024];

#define AppendLight( lightIndex, counter, lightList ) \
    InterlockedAdd( counter, 1, index ); \
    if ( index < 1024 ) \
    { \
        lightList[index] = lightIndex; \
    }

// Implementation of light culling compute shader is based on the presentation
// "DirectX 11 Rendering in Battlefield 3" (2011) by Johan Andersson, DICE.
// Retrieved from: http://www.slideshare.net/DICEStudio/directx-11-rendering-in-battlefield-3
// Retrieved: July 13, 2015
// And "Forward+: A Step Toward Film-Style Shading in Real Time", Takahiro Harada (2012)
// published in "GPU Pro 4", Chapter 5 (2013) Taylor & Francis Group, LLC.
[RootSignature( CullLights_RS )]
[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main( ComputeShaderInput IN )
{
    // Counters
    uint i, index;

    // Calculate min & max depth in threadgroup / tile.
    int2 texCoord = IN.DispatchThreadID.xy;
    float fDepth = DepthTexture.Load( texCoord, 0 );

    uint uDepth = asuint( fDepth );

    if ( IN.GroupIndex == 0 ) // Avoid contention by other threads in the group.
    {
        uMinDepth = 0xffffffff;
        uMaxDepth = 0;
        PointLightCount_OPAQUE = 0;
        SpotLightCount_OPAQUE = 0;
        PointLightCount_TRANSPARENT = 0;
        SpotLightCount_TRANSPARENT = 0;
        GroupFrustum = Frustums[IN.GroupID.x + ( IN.GroupID.y * DispatchParamsCB.NumThreadGroups.x )];
    }

    GroupMemoryBarrierWithGroupSync();

    InterlockedMin( uMinDepth, uDepth );
    InterlockedMax( uMaxDepth, uDepth );

    GroupMemoryBarrierWithGroupSync();

    float fMinDepth = asfloat( uMinDepth );
    float fMaxDepth = asfloat( uMaxDepth );

    // Convert depth values to view space.
    float minDepthVS = ClipToView( float4( 0, 0, fMinDepth, 1 ) ).z;
    float maxDepthVS = ClipToView( float4( 0, 0, fMaxDepth, 1 ) ).z;
    float nearClipVS = ClipToView( float4( 0, 0, 0, 1 ) ).z;

    // Clipping plane for minimum depth value 
    // (used for testing lights within the bounds of opaque geometry).
    Plane minPlane = { float3( 0, 0, -1 ), -minDepthVS };

    // Cull point lights.
    for ( i = IN.GroupIndex; i < LightCountsCB.NumPointLights; i += BLOCK_SIZE * BLOCK_SIZE )
    {
        if ( PointLights[i].Enabled )
        {
            PointLight pointLight = PointLights[i];

            Sphere sphere = { pointLight.PositionVS.xyz, pointLight.Range };
            if ( SphereInsideFrustum( sphere, GroupFrustum, nearClipVS, maxDepthVS ) )
            {
                AppendLight( i, PointLightCount_TRANSPARENT, PointLightList_TRANSPARENT );

                if ( !SphereInsidePlane( sphere, minPlane ) )
                {
                    AppendLight( i, PointLightCount_OPAQUE, PointLightList_OPAQUE );
                }
            }
        }
    }

    // Cull spot lights
    for ( i = IN.GroupIndex; i < LightCountsCB.NumSpotLights; i += BLOCK_SIZE * BLOCK_SIZE )
    {
        if ( SpotLights[i].Enabled )
        {
            SpotLight spotLight = SpotLights[i];

            float coneRadius = tan( radians( spotLight.SpotlightAngle ) ) * spotLight.Range;
            Cone cone = { spotLight.PositionVS.xyz, spotLight.Range, spotLight.DirectionVS.xyz, coneRadius };

            if ( ConeInsideFrustum( cone, GroupFrustum, nearClipVS, maxDepthVS ) )
            {
                // Add spot light to light list for transparent geometry.
                AppendLight( i, SpotLightCount_TRANSPARENT, SpotLightList_TRANSPARENT );

                if ( !ConeInsidePlane( cone, minPlane ) )
                {
                    // Add light to light list for opaque geometry.
                    AppendLight( i, SpotLightCount_OPAQUE, SpotLightList_OPAQUE );
                }
            }
        }
    }

    // Wait till all threads in group have caught up.
    GroupMemoryBarrierWithGroupSync();

    // Update global memory with visible light buffer.
    // First update the light grid (only thread 0 in group needs to do this)
    if ( IN.GroupIndex == 0 )
    {
        // Update the light grids for point lights
        InterlockedAdd( PointLightIndexCounter_OPAQUE[0], PointLightCount_OPAQUE, PointLightIndexStartOffset_OPAQUE );
        PointLightGrid_OPAQUE[IN.GroupID.xy] = uint2( PointLightIndexStartOffset_OPAQUE, PointLightCount_OPAQUE );

        InterlockedAdd( PointLightIndexCounter_TRANSPARENT[0], PointLightCount_TRANSPARENT, PointLightIndexStartOffset_TRANSPARENT );
        PointLightGrid_TRANSPARENT[IN.GroupID.xy] = uint2( PointLightIndexStartOffset_TRANSPARENT, PointLightCount_TRANSPARENT );

        // Update the light grids for spot lights
        InterlockedAdd( SpotLightIndexCounter_OPAQUE[0], SpotLightCount_OPAQUE, SpotLightIndexStartOffset_OPAQUE );
        SpotLightGrid_OPAQUE[IN.GroupID.xy] = uint2( SpotLightIndexStartOffset_OPAQUE, SpotLightCount_OPAQUE );

        InterlockedAdd( SpotLightIndexCounter_TRANSPARENT[0], SpotLightCount_TRANSPARENT, SpotLightIndexStartOffset_TRANSPARENT );
        SpotLightGrid_TRANSPARENT[IN.GroupID.xy] = uint2( SpotLightIndexStartOffset_TRANSPARENT, SpotLightCount_TRANSPARENT );
    }

    GroupMemoryBarrierWithGroupSync();

    // Now update the point light index list (all threads).
    for ( i = IN.GroupIndex; i < PointLightCount_OPAQUE; i += BLOCK_SIZE * BLOCK_SIZE )
    {
        PointLightIndexList_OPAQUE[PointLightIndexStartOffset_OPAQUE + i] = PointLightList_OPAQUE[i];
    }

    for ( i = IN.GroupIndex; i < PointLightCount_TRANSPARENT; i += BLOCK_SIZE * BLOCK_SIZE )
    {
        PointLightIndexList_TRANSPARENT[PointLightIndexStartOffset_TRANSPARENT + i] = PointLightList_TRANSPARENT[i];
    }

    // Now update the spot light index list (all threads)
    // Update the light index list for spot lights.
    for ( i = IN.GroupIndex; i < SpotLightCount_OPAQUE; i += BLOCK_SIZE * BLOCK_SIZE )
    {
        SpotLightIndexList_OPAQUE[SpotLightIndexStartOffset_OPAQUE + i] = SpotLightList_OPAQUE[i];
    }

    for ( i = IN.GroupIndex; i < SpotLightCount_TRANSPARENT; i += BLOCK_SIZE * BLOCK_SIZE )
    {
        SpotLightIndexList_TRANSPARENT[SpotLightIndexStartOffset_TRANSPARENT + i] = SpotLightList_TRANSPARENT[i];
    }

    // Update the debug texture output.
    if ( IN.GroupThreadID.x == 0 || IN.GroupThreadID.y == 0 )
    {
        RWDebugTexture[texCoord] = float4( 1, 1, 1, 0 );
    }
    else if ( IN.GroupThreadID.x < 1 || IN.GroupThreadID.y < 1 )
    {
        RWDebugTexture[texCoord] = float4( 1, 1, 1, 0.2f );
    }
    else if ( PointLightCount_OPAQUE + SpotLightCount_OPAQUE > 0 )
    {
        float normalizedLightCount = ( PointLightCount_OPAQUE + SpotLightCount_OPAQUE ) / 100.0f;
        float4 color = LightCountHeatMap.SampleLevel( LinearClampSampler, float2( normalizedLightCount, 0 ), 0 );
        RWDebugTexture[texCoord] = float4( color.rgb, 0.9f );
    }
    else
    {
        RWDebugTexture[texCoord] = float4( 0, 0, 0, 1 );
    }

    //DebugTexture[texCoord] = float4( maxDepthVS, maxDepthVS, maxDepthVS, 1 );

}