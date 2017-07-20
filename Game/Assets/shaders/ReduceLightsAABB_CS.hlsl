#include "Include/CommonInclude.hlsli"

/**
 * A compute shader to determine the view space AABB of all of the lights in the scene.
 * A parallel reduction algorithm is used to reduce the light AABBs to a single AABB that
 * encompasses all lights.
 * Source: The CUDA Handbook (2013), Nicholas Wilt.
 */

#ifndef NUM_THREADS
#define NUM_THREADS 512
#endif

#define FLT_MAX 3.402823466e+38F

groupshared float4 gs_AABBMin[NUM_THREADS]; // 8,192 Bytes
groupshared float4 gs_AABBMax[NUM_THREADS]; // 8,192 Bytes

// Perform log-step reduction in group shared memory.
void LogStepReduction( ComputeShaderInput IN )
{
    // If we can assume that NUM_THREADS is a power of 2, we can compute
    // the reduction index by performing a bit shift. This is equivalent to 
    // halving the number of values (and threads) that must perform the reduction
    // operation.
    uint reduceIndex = NUM_THREADS >> 1;

    [unroll]
    while ( reduceIndex > 32 )
    {
        if ( IN.GroupIndex < reduceIndex )
        {
            gs_AABBMin[IN.GroupIndex] = min( gs_AABBMin[IN.GroupIndex], gs_AABBMin[IN.GroupIndex + reduceIndex] );
            gs_AABBMax[IN.GroupIndex] = max( gs_AABBMax[IN.GroupIndex], gs_AABBMax[IN.GroupIndex + reduceIndex] );
        }

        // Sync group shared memory writes.
        GroupMemoryBarrierWithGroupSync();

        // Halve the number of threads that participate in the reduction.
        reduceIndex >>= 1;
    }

    // Within a warp (of 32 threads), instructions are warp-synchronous
    // and the GroupMemoryBarrierWithGroupSync() is no longer need to ensure
    // the previous writes to groups shared memory have completed.
    // Source: DirectCompute Optimizations and Best Practices (2010), Eric Young.
    // Source: The CUDA Handbook (2013), Nicholas Wilt
    if ( IN.GroupIndex < 32 )
    {
        [unroll]
        while ( reduceIndex > 0 )
        {
            // To avoid out-of-bounds memory access, the number of threads in the 
            // group must be at least 2x the reduce index. For example, the 
            // thread at index 31 will access elements 31 and 63 so the size of the thread group
            // must be at least 64.
            if ( NUM_THREADS >= reduceIndex << 1 )
            {
                gs_AABBMin[IN.GroupIndex] = min( gs_AABBMin[IN.GroupIndex], gs_AABBMin[IN.GroupIndex + reduceIndex] );
                gs_AABBMax[IN.GroupIndex] = max( gs_AABBMax[IN.GroupIndex], gs_AABBMax[IN.GroupIndex + reduceIndex] );
            }

            reduceIndex >>= 1;
        }

        // Now write the reduced values to global memory.
        if ( IN.GroupIndex == 0 )
        {
            RWLightAABB[IN.GroupID.x].Min = gs_AABBMin[IN.GroupIndex];
            RWLightAABB[IN.GroupID.x].Max = gs_AABBMax[IN.GroupIndex];
        }
    }
}


// The 1st pass of the reduction operates on the light buffers.
[RootSignature( ReduceLightsAABB_RS )]
[numthreads( NUM_THREADS, 1, 1)]
void reduce1( ComputeShaderInput IN )
{
    uint i;

    float4 aabbMin = float4( FLT_MAX, FLT_MAX, FLT_MAX, 1 );
    float4 aabbMax = float4( -FLT_MAX, -FLT_MAX, -FLT_MAX, 1 );

    // First compute point lights AABB.
    for ( i = IN.DispatchThreadID.x; i < LightCountsCB.NumPointLights; i += NUM_THREADS * DispatchParamsCB.NumThreadGroups.x )
    {
        PointLight pointLight = PointLights[i];

        aabbMin = min( aabbMin, pointLight.PositionVS - pointLight.Range );
        aabbMax = max( aabbMax, pointLight.PositionVS + pointLight.Range );
    }

    // Next, expand AABB for spot lights.
    for ( i = IN.DispatchThreadID.x; i < LightCountsCB.NumSpotLights; i += NUM_THREADS * DispatchParamsCB.NumThreadGroups.x )
    {
        SpotLight spotLight = SpotLights[i];

        aabbMin = min( aabbMin, spotLight.PositionVS - spotLight.Range );
        aabbMax = max( aabbMax, spotLight.PositionVS + spotLight.Range );
    }

    gs_AABBMin[IN.GroupIndex] = aabbMin;
    gs_AABBMax[IN.GroupIndex] = aabbMax;

    // Sync group shared memory writes.
    GroupMemoryBarrierWithGroupSync();

    // Perform log-step reduction to allow each thread group in the dispatch
    // to reduce to a single element.
    LogStepReduction( IN );
}

// The subsequent passes of the reduction operate on the global AABB computed 
// in previous pass.
// This step is repeated until we are reduced to a single thread group.
[RootSignature( ReduceLightsAABB_RS )]
[numthreads( NUM_THREADS, 1, 1 )]
void reduce2( ComputeShaderInput IN )
{
    uint i; // Loop counter.

    float4 aabbMin = float4( FLT_MAX, FLT_MAX, FLT_MAX, 1 );
    float4 aabbMax = float4( -FLT_MAX, -FLT_MAX, -FLT_MAX, 1 );

    for ( i = IN.GroupIndex; i < ReductionParamsCB.NumElements; i += NUM_THREADS * DispatchParamsCB.NumThreadGroups.x )
    {
        aabbMin = min( aabbMin, RWLightAABB[i].Min );
        aabbMax = max( aabbMax, RWLightAABB[i].Max );
    }

    gs_AABBMin[IN.GroupIndex] = aabbMin;
    gs_AABBMax[IN.GroupIndex] = aabbMax;

    // Sync group shared memory writes.
    GroupMemoryBarrierWithGroupSync();

    // Perform log-step reduction to allow each thread group in the dispatch
    // to reduce to a single element. If there was only a single thread group
    // in this dispatch, then this will reduce to a single element.
    LogStepReduction( IN );
}