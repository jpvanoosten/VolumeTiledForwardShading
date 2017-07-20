#include "Include/CommonInclude.hlsli"

/**
 * This compute shader is used to build the BVH for the lights in the scene.
 * Each BVH node (including the root) has 32 childeren. No attempt is made
 * to create a perfectly balanced tree with no empty children.
 * The BVH is created by assigning 32 consecutive leaves to the left-most
 * node of the tree, the next 32 leaves to the 2nd node, and so forth.
 * A BVH constructed from 1,000,000 lights will have 4 levels with 
 * 1,048,576 leaf nodes and 33,825 child nodes (including the root).
 * The compute shader proceeds by computing the last level of the tree and
 * building upwards to the root. Since each thread group will compute 8 nodes,
 * each level will require a global syncronization before the next higest level
 * can be computed (since syncronization between thread groups is not possible).
 * So to build a BVH of 4 levels will require 2 dispatches (the root and leaf nodes 
 * do not need to be explicitly computed because they are implicitly computed. 
 */

#ifndef NUM_THREADS
#define NUM_THREADS 32 * 16
#endif

#define FLT_MAX 3.402823466e+38F

// The number of nodes in each level of the BVH
static const uint NumLevelNodes[] = {
    1,          // Level 0 ( 32^0 )
    32,         // Level 1 ( 32^1 )
    1024,       // Level 2 ( 32^2 )
    32768,      // Level 3 ( 32^3 )
    1048576,    // Level 4 ( 32^4 )
    33554432,   // Level 5 ( 32^5 )
    1073741824, // Level 6 ( 32^6 )
};

// The first index of a node in the BVH 
// given the level of the BVH.
static const uint FirstNodeIndex[] = {
    0,          // Level 0
    1,          // Level 1
    33,         // Level 2
    1057,       // Level 3
    33825,      // Level 4
    1082401,    // Level 5
    34636833,   // Level 6
};

groupshared float4 gs_AABBMin[NUM_THREADS]; // 8,192 Bytes
groupshared float4 gs_AABBMax[NUM_THREADS]; // 8,192 Bytes

// Perform warp-synchronous log step reduction.
// This function is similar to the function in the ReduceLightsAABB compute 
// shader but only reduces chunks of 32 elements into a single element.
void LogStepReduction( ComputeShaderInput IN )
{
    uint reduceIndex = 32 >> 1;
    uint mod32GroupIndex = IN.GroupIndex % 32;

    while ( mod32GroupIndex < reduceIndex )
    {
        gs_AABBMin[IN.GroupIndex] = min( gs_AABBMin[IN.GroupIndex], gs_AABBMin[IN.GroupIndex + reduceIndex] );
        gs_AABBMax[IN.GroupIndex] = max( gs_AABBMax[IN.GroupIndex], gs_AABBMax[IN.GroupIndex + reduceIndex] );

        reduceIndex >>= 1;
    }
}

// Compute the number of levels of the BVH (including the leaf nodes)
//uint GetNumLevels( uint numLeaves )
//{
//    return ceil( log( numLeaves ) / log( 32 ) );
//}

/**
 * Compute the bottom level of the BVH tree.
 */
[RootSignature( BuildBVH_RS )]
[numthreads( NUM_THREADS, 1, 1)]
void BuildBottom( ComputeShaderInput IN )
{
    uint lightIndex;
    uint numLevels, nodeIndex, nodeOffset;
    float4 aabbMin, aabbMax;

    // First compute BVH AABB for point lights.
    uint leafIndex = IN.DispatchThreadID.x;

    if ( leafIndex < LightCountsCB.NumPointLights )
    {
        lightIndex = PointLightIndices[leafIndex];
        PointLight pointLight = PointLights[lightIndex];

        aabbMin = pointLight.PositionVS - pointLight.Range;
        aabbMax = pointLight.PositionVS + pointLight.Range;
    }
    else
    {
        aabbMin = float4( FLT_MAX, FLT_MAX, FLT_MAX, 1 );
        aabbMax = float4( -FLT_MAX, -FLT_MAX, -FLT_MAX, 1 );
    }

    gs_AABBMin[IN.GroupIndex] = aabbMin;
    gs_AABBMax[IN.GroupIndex] = aabbMax;

    // Log-step reduction is performed warp-syncronous and thus does not require
    // a group sync barrier.
    LogStepReduction( IN );

    // The first thread of each warp will write the AABB to global memory.
    if ( IN.DispatchThreadID.x % 32 == 0 )
    {
        // Number of levels in the BVH
        numLevels = BVHParamsCB.PointLightLevels; // GetNumLevels( LightCountsCB.NumPointLights );
        // Offset of the node in the BVH at the last level of child nodes.
        nodeOffset = IN.DispatchThreadID.x / 32;

        if ( numLevels > 0 && nodeOffset < NumLevelNodes[numLevels - 1] )
        {
            nodeIndex = FirstNodeIndex[numLevels - 1] + nodeOffset;
            RWPointLightBVH[nodeIndex].Min = gs_AABBMin[IN.GroupIndex];
            RWPointLightBVH[nodeIndex].Max = gs_AABBMax[IN.GroupIndex];
        }
    }

    // Now compute BVH nodes for spot lights.

    if ( leafIndex < LightCountsCB.NumSpotLights )
    {
        lightIndex = SpotLightIndices[leafIndex];
        SpotLight spotLight = SpotLights[lightIndex];

        aabbMin = spotLight.PositionVS - spotLight.Range;
        aabbMax = spotLight.PositionVS + spotLight.Range;
    }
    else
    {
        aabbMin = float4( FLT_MAX, FLT_MAX, FLT_MAX, 1 );
        aabbMax = float4( -FLT_MAX, -FLT_MAX, -FLT_MAX, 1 );
    }

    gs_AABBMin[IN.GroupIndex] = aabbMin;
    gs_AABBMax[IN.GroupIndex] = aabbMax;

    LogStepReduction( IN );

    // The first thread of each warp will write the AABB to global memory.
    if ( IN.DispatchThreadID.x % 32 == 0 )
    {
        // Number of levels in the BVH
        numLevels = BVHParamsCB.SpotLightLevels; // GetNumLevels( LightCountsCB.NumSpotLights );
        // Offset of the node in the BVH at the last level of child nodes.
        nodeOffset = IN.DispatchThreadID.x / 32;

        if ( numLevels > 0 && nodeOffset < NumLevelNodes[numLevels - 1] )
        {
            nodeIndex = FirstNodeIndex[numLevels - 1] + nodeOffset;

            RWSpotLightBVH[nodeIndex].Min = gs_AABBMin[IN.GroupIndex];
            RWSpotLightBVH[nodeIndex].Max = gs_AABBMax[IN.GroupIndex];
        }
    }
}

/**
 * Build the upper levels of the BVH.
 */
[RootSignature( BuildBVH_RS )]
[numthreads( NUM_THREADS, 1, 1 )]
void BuildTop( ComputeShaderInput IN )
{
    uint numLevels, childLevel;
    uint childOffset, childIndex;
    uint nodeOffset, nodeIndex;
    float4 aabbMin, aabbMax;

    // First build upper BVH for point light BVH.

    childLevel = BVHParamsCB.ChildLevel;
    numLevels = BVHParamsCB.PointLightLevels;
    childOffset = IN.DispatchThreadID.x;

    if ( childLevel < numLevels && childOffset < NumLevelNodes[BVHParamsCB.ChildLevel] )
    {
        childIndex = FirstNodeIndex[childLevel] + childOffset;

        aabbMin = RWPointLightBVH[childIndex].Min;
        aabbMax = RWPointLightBVH[childIndex].Max;
    }
    else
    {
        aabbMin = float4( FLT_MAX, FLT_MAX, FLT_MAX, 1 );
        aabbMax = float4( -FLT_MAX, -FLT_MAX, -FLT_MAX, 1 );
    }

    gs_AABBMin[IN.GroupIndex] = aabbMin;
    gs_AABBMax[IN.GroupIndex] = aabbMax;

    // Log-step reduction is performed warp-syncronous and thus does not require
    // a group sync barrier.
    LogStepReduction( IN );

    // The first thread of each warp will write the AABB to global memory.
    if ( IN.DispatchThreadID.x % 32 == 0 )
    {
        nodeOffset = IN.DispatchThreadID.x / 32;

        if ( childLevel < numLevels && nodeOffset < NumLevelNodes[BVHParamsCB.ChildLevel - 1] )
        {
            nodeIndex = FirstNodeIndex[childLevel - 1] + nodeOffset;
            RWPointLightBVH[nodeIndex].Min = gs_AABBMin[IN.GroupIndex];
            RWPointLightBVH[nodeIndex].Max = gs_AABBMax[IN.GroupIndex];
        }
    }

    // Now build upper BVH for spot light BVH.

    numLevels = BVHParamsCB.SpotLightLevels;

    if ( childLevel < numLevels && childOffset < NumLevelNodes[BVHParamsCB.ChildLevel] )
    {
        childIndex = FirstNodeIndex[childLevel] + childOffset;

        aabbMin = RWSpotLightBVH[childIndex].Min;
        aabbMax = RWSpotLightBVH[childIndex].Max;
    }
    else
    {
        aabbMin = float4( FLT_MAX, FLT_MAX, FLT_MAX, 1 );
        aabbMax = float4( -FLT_MAX, -FLT_MAX, -FLT_MAX, 1 );
    }

    gs_AABBMin[IN.GroupIndex] = aabbMin;
    gs_AABBMax[IN.GroupIndex] = aabbMax;

    // Log-step reduction is performed warp-syncronous and thus does not require
    // a group sync barrier.
    LogStepReduction( IN );

    // The first thread of each warp will write the AABB to global memory.
    if ( IN.DispatchThreadID.x % 32 == 0 )
    {
        nodeOffset = IN.DispatchThreadID.x / 32;

        if ( childLevel < numLevels && nodeOffset < NumLevelNodes[BVHParamsCB.ChildLevel - 1] )
        {
            nodeIndex = FirstNodeIndex[childLevel - 1] + nodeOffset;
            RWSpotLightBVH[nodeIndex].Min = gs_AABBMin[IN.GroupIndex];
            RWSpotLightBVH[nodeIndex].Max = gs_AABBMax[IN.GroupIndex];
        }
    }
}
