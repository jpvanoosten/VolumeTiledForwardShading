#include "Include/CommonInclude.hlsli"

#define NUM_THREADS 32  // Each thread group is a warp!
#define MAX_LIGHTS 2048

// The number of child nodes (excluding leaves)
// given the number of levels of the BVH.
// for( i = l-1 ... 0 ) { N += 32 ^ i }
static const uint NumChildNodes[] = {
    1,          // 1 level   =32^0
    33,         // 2 levels  +32^1
    1057,       // 3 levels  +32^2
    33825,      // 4 levels  +32^3
    1082401,    // 5 levels  +32^4
    34636833,   // 6 levels  +32^5
};

// Using a stack of node IDs to traverse the BVH was inspired by:
// Source: https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-ii-tree-traversal-gpu/
// Author: Tero Karras (NVIDIA)
// Retrieved: September 13, 2016
groupshared uint gs_NodeStack[1024];    // This should be enough to push 32 layers of nodes (32 nodes per layer).
groupshared uint gs_StackPtr;           // The current index in the node stack.
groupshared uint gs_ParentIndex;        // The index of the parent node in the BVH that is currently being processed.

groupshared uint gs_ClusterIndex1D;
groupshared AABB gs_ClusterAABB;

groupshared uint gs_PointLightCount;
groupshared uint gs_SpotLightCount;
groupshared uint gs_PointLightStartOffset;
groupshared uint gs_SpotLightStartOffset;
groupshared uint gs_PointLightList[MAX_LIGHTS];
groupshared uint gs_SpotLightList[MAX_LIGHTS];

#define AppendLight( lightIndex, counter, lightList ) \
    InterlockedAdd( counter, 1, index ); \
    if ( index < MAX_LIGHTS ) \
    { \
        lightList[index] = lightIndex; \
    }

void PushNode( uint nodeIndex )
{
    int stackPtr;    
    InterlockedAdd( gs_StackPtr, 1, stackPtr );
    
    if ( stackPtr < 1024 )
    {
        gs_NodeStack[stackPtr] = nodeIndex;
    }
}

uint PopNode()
{
    uint nodeIndex = 0;
    int stackPtr;
    InterlockedAdd( gs_StackPtr, -1, stackPtr );
    
    if ( stackPtr > 0 && stackPtr < 1024 )
    {
        nodeIndex = gs_NodeStack[stackPtr-1];
    }

    return nodeIndex;
}

// Get the index of the the first child node in the BVH.
uint GetFirstChild( uint parentIndex, uint numLevels )
{
    return ( numLevels > 0 ) ? parentIndex * 32 + 1 : 0;
}

// Check to see if an index of the BVH is a leaf.
bool IsLeafNode( uint childIndex, uint numLevels )
{
    return ( numLevels > 0 ) ? childIndex > ( NumChildNodes[numLevels - 1] - 1 ): true;
}

// Get the index of a leaf node given the node ID in the BVH.
uint GetLeafIndex( uint nodeIndex, uint numLevels )
{
    return ( numLevels > 0 ) ? nodeIndex - NumChildNodes[numLevels - 1] : nodeIndex;
}

[RootSignature( AssignLightsToClustersBVH_RS )]
[numthreads( NUM_THREADS, 1, 1 )]
void main( ComputeShaderInput IN )
{
    uint i, index;
    uint childOffset = IN.GroupIndex;

    if ( IN.GroupIndex == 0 )
    {
        gs_PointLightCount = 0;
        gs_SpotLightCount = 0;
        gs_StackPtr = 0;
        gs_ParentIndex = 0;

        gs_ClusterIndex1D = UniqueClusters[IN.GroupID.x];
        gs_ClusterAABB = ClusterAABBs[gs_ClusterIndex1D];

        // Push the root node (at index 0) on the node stack.
        PushNode( 0 );
    }

    GroupMemoryBarrierWithGroupSync();

    // First check point light BVH
    do
    {
        uint childIndex = GetFirstChild( gs_ParentIndex, BVHParamsCB.PointLightLevels ) + childOffset;

        if ( IsLeafNode( childIndex, BVHParamsCB.PointLightLevels ) )
        {
            uint leafIndex = GetLeafIndex( childIndex, BVHParamsCB.PointLightLevels );
            if ( leafIndex < LightCountsCB.NumPointLights )
            {
                uint lightIndex = PointLightIndices[leafIndex];
                PointLight pointLight = PointLights[lightIndex];
                Sphere sphere = { pointLight.PositionVS.xyz, pointLight.Range };

                if ( pointLight.Enabled && SphereInsideAABB( sphere, gs_ClusterAABB ) )
                {
                    AppendLight( lightIndex, gs_PointLightCount, gs_PointLightList );
                }
            }
        }
        else if ( AABBIntersectAABB( gs_ClusterAABB, PointLightBVH[childIndex] ) )
        {
            PushNode( childIndex );
        }

        GroupMemoryBarrierWithGroupSync();

        if ( IN.GroupIndex == 0 )
        {
            gs_ParentIndex = PopNode();
        }

        GroupMemoryBarrierWithGroupSync();

    } while ( gs_ParentIndex > 0 );

    GroupMemoryBarrierWithGroupSync();

    // Reset the stack.
    if ( IN.GroupIndex == 0 )
    {
        gs_StackPtr = 0;
        gs_ParentIndex = 0;

        // Push the root node (at index 0) on the node stack.
        PushNode( 0 );
    }

    GroupMemoryBarrierWithGroupSync();

    // Now check spot light BVH.
    do
    {
        uint childIndex = GetFirstChild( gs_ParentIndex, BVHParamsCB.SpotLightLevels ) + childOffset;

        if ( IsLeafNode( childIndex, BVHParamsCB.SpotLightLevels ) )
        {
            uint leafIndex = GetLeafIndex( childIndex, BVHParamsCB.SpotLightLevels );
            if ( leafIndex < LightCountsCB.NumSpotLights )
            {
                uint lightIndex = SpotLightIndices[leafIndex];
                SpotLight spotLight = SpotLights[lightIndex];
                Sphere sphere = { spotLight.PositionVS.xyz, spotLight.Range };

                if ( spotLight.Enabled && SphereInsideAABB( sphere, gs_ClusterAABB ) )
                {
                    AppendLight( lightIndex, gs_SpotLightCount, gs_SpotLightList );
                }
            }
        }
        else if ( AABBIntersectAABB( gs_ClusterAABB, SpotLightBVH[childIndex] ) )
        {
            PushNode( childIndex );
        }

        GroupMemoryBarrierWithGroupSync();

        if ( IN.GroupIndex == 0 )
        {
            gs_ParentIndex = PopNode();
        }

        GroupMemoryBarrierWithGroupSync();

    } while ( gs_ParentIndex > 0 );

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
}
