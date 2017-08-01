#include "Include/CommonInclude.hlsli"

#define NUM_THREADS 1024

groupshared uint gs_ClusterIndex1D;
groupshared AABB gs_ClusterAABB;

groupshared uint gs_PointLightCount;
groupshared uint gs_SpotLightCount;
groupshared uint gs_PointLightStartOffset;
groupshared uint gs_SpotLightStartOffset;
groupshared uint gs_PointLightList[1024];
groupshared uint gs_SpotLightList[1024];

#define AppendLight( lightIndex, counter, lightList ) \
    InterlockedAdd( counter, 1, index ); \
    if ( index < 1024 ) \
    { \
        lightList[index] = lightIndex; \
    }

[RootSignature( AssignLightsToClusters_RS )]
[numthreads( NUM_THREADS, 1, 1 )]
void main( ComputeShaderInput IN )
{
    uint i, index;

    if ( IN.GroupIndex == 0 )
    {
        gs_PointLightCount = 0;
        gs_SpotLightCount = 0;

        gs_ClusterIndex1D = UniqueClusters[IN.GroupID.x];
        gs_ClusterAABB = ClusterAABBs[gs_ClusterIndex1D];
    }

    GroupMemoryBarrierWithGroupSync();

    // Intersect point lights against AABB.
    for ( i = IN.GroupIndex; i < LightCountsCB.NumPointLights; i += NUM_THREADS )
    {
        if ( PointLights[i].Enabled )
        {
            PointLight pointLight = PointLights[i];
            Sphere sphere = { pointLight.PositionVS.xyz, pointLight.Range };

            if ( SphereInsideAABB( sphere, gs_ClusterAABB ) )
            {
                AppendLight( i, gs_PointLightCount, gs_PointLightList );
            }
        }
    }

    // Intersect spot lights against AABB.
    for ( i = IN.GroupIndex; i < LightCountsCB.NumSpotLights; i += NUM_THREADS )
    {
        if ( SpotLights[i].Enabled )
        {
            SpotLight spotLight = SpotLights[i];
            Sphere sphere = { spotLight.PositionVS.xyz, spotLight.Range };

            // I don't know of any good algorithms to perform cone / AABB intersection
            // tests. For now, just treat spotlights cones as spheres.
            if ( SphereInsideAABB( sphere, gs_ClusterAABB ) )
            {
                AppendLight( i, gs_SpotLightCount, gs_SpotLightList );
            }
        }
    }

    GroupMemoryBarrierWithGroupSync();

    // Now update the global light grids with the light lists and light counts.
    if ( IN.GroupIndex == 0 )
    {
        // Update light grid for point lights.
        InterlockedAdd( RWPointLightIndexCounter_Cluster[0], gs_PointLightCount, gs_PointLightStartOffset );
        RWPointLightGrid_Cluster[gs_ClusterIndex1D] = uint2( gs_PointLightStartOffset, gs_PointLightCount );

        // Update light grid for spot lights.
        InterlockedAdd( RWSpotLightIndexCounter_Cluster[0], gs_SpotLightCount, gs_SpotLightStartOffset );
        RWSpotLightGrid_Cluster[gs_ClusterIndex1D] = uint2( gs_SpotLightStartOffset, gs_SpotLightCount );
    }

    GroupMemoryBarrierWithGroupSync();

    // Now update the global light index lists with the group shared light lists.
    for ( i = IN.GroupIndex; i < gs_PointLightCount; i += NUM_THREADS )
    {
        RWPointLightIndexList_Cluster[gs_PointLightStartOffset + i] = gs_PointLightList[i];
    }

    for ( i = IN.GroupIndex; i < gs_SpotLightCount; i += NUM_THREADS )
    {
        RWSpotLightIndexList_Cluster[gs_SpotLightStartOffset + i] = gs_SpotLightList[i];
    }

    // TODO: Update debug texture?
}