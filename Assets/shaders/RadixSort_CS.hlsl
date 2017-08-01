#include "Include/CommonInclude.hlsli"

/**
 * Perform a Radix sort over a set of integers.
 * Based on "Parallel Prefix Sum (Scan) with CUDA", GPU Gems 3 Chapter 39. Mark Harris, et. al. 
 * Retrieved from: http://http.developer.nvidia.com/GPUGems3/gpugems3_ch39.html
 * Retrieved on: Aug 2nd, 2016.
 */

#ifndef NUM_THREADS
#define NUM_THREADS 256
#endif

#define INT_MAX 0xffffffff

groupshared uint gs_Keys[NUM_THREADS];      // A temporary buffer to store the input keys.                                          (1,024 Bytes)
groupshared uint gs_Values[NUM_THREADS];    // A temporary buffer to store the input values.                                        (1,024 Bytes)
groupshared uint gs_E[NUM_THREADS];         // Set a 1 for all false sort keys (b == 0) and a 0 for all true sort keys (b == 1)     (1,024 Bytes)
groupshared uint gs_F[NUM_THREADS];         // Scan the splits. This results in the output index of all false sort keys (b == 0)    (1,024 Bytes)
groupshared uint gs_D[NUM_THREADS];         // The desination index for the ouput key and value.                                    (1,024 Bytes)
groupshared uint gs_TotalFalses;            // The result of e[NUM_THREADS - 1] + f[NUM_THREADS - 1];                               (4 Bytes)

[RootSignature(RadixSort_RS)]
[numthreads(NUM_THREADS, 1, 1)]
void main( ComputeShaderInput IN )
{
    // The number of bits to consider sorting.
    // In this case, the input keys are 30-bit morton codes.
    const uint NumBits = 30;

    // Loop variables.
    uint b, i;

    // Store the input key and values into shared memory.
    gs_Keys[IN.GroupIndex] = ( IN.DispatchThreadID.x < SortParamsCB.NumElements ) ? InputKeys[IN.DispatchThreadID.x] : INT_MAX;
    gs_Values[IN.GroupIndex] = ( IN.DispatchThreadID.x < SortParamsCB.NumElements ) ? InputValues[IN.DispatchThreadID.x] : INT_MAX;

    // Loop over the bits starting at the least-significant bit.
    for ( b = 0; b < NumBits; ++b )
    {
        // 1. In a temporary buffer in shared memory, we set a 1 for all false 
        //    sort keys (b = 0) and a 0 for all true sort keys.
        gs_E[IN.GroupIndex] = ( ( gs_Keys[IN.GroupIndex] >> b ) & 1 ) == 0 ? 1 : 0;

        // Sync group shared memory writes.
        GroupMemoryBarrierWithGroupSync();

        if ( IN.GroupIndex == 0 )
        {
            gs_F[IN.GroupIndex] = 0;
        }
        else
        {
            gs_F[IN.GroupIndex] = gs_E[IN.GroupIndex - 1];
        }

        // Sync group shared memory writes.
        GroupMemoryBarrierWithGroupSync();

        // 2. We then scan (prefix sum) this buffer. This is the enumerate operation; 
        //    each false sort key now contains its destination address in the scan 
        //    output, which we will call f. These first two steps are equivalent to 
        //    a stream compaction operation on all false sort keys.
        [unroll]
        for ( i = 1; i < NUM_THREADS; i <<= 1 )
        {
            uint temp = gs_F[IN.GroupIndex];

            if ( IN.GroupIndex > i )
            {
                temp += gs_F[IN.GroupIndex - i];
            }

            // Sync group shared memory reads before writes.
            GroupMemoryBarrierWithGroupSync();

            gs_F[IN.GroupIndex] = temp;

            // Sync group shared memory writes.
            GroupMemoryBarrierWithGroupSync();
        }

        // 3. The last element in the scan's output now contains the total 
        //    number of false sort keys. We write this value to a shared 
        //    variable, gs_TotalFalses.
        if ( IN.GroupIndex == 0 )
        {
            gs_TotalFalses = gs_E[NUM_THREADS - 1] + gs_F[NUM_THREADS - 1];
        }

        // Sync group shared memory writes.
        GroupMemoryBarrierWithGroupSync();

        // 4. Now we compute the destination address for the true sort keys. For 
        // a sort key at index i, this address is t = i - f + totalFalses. We 
        // then select between t and f depending on the value of b to get the 
        // destination address d of each fragment.
        gs_D[IN.GroupIndex] = ( gs_E[IN.GroupIndex] == 1 ) ? gs_F[IN.GroupIndex] : IN.GroupIndex - gs_F[IN.GroupIndex] + gs_TotalFalses;

        // 5. Finally, we scatter the original sort keys to destination address 
        //    d. The scatter pattern is a perfect permutation of the input, so 
        //    we see no write conflicts with this scatter.
        uint key = gs_Keys[IN.GroupIndex];
        uint value = gs_Values[IN.GroupIndex];

        // Sync group shared memory reads before writes.
        GroupMemoryBarrierWithGroupSync();

        gs_Keys[gs_D[IN.GroupIndex]] = key;
        gs_Values[gs_D[IN.GroupIndex]] = value;

        // Sync group shared memory writes.
        GroupMemoryBarrierWithGroupSync();
    }

    // Now commit the results to global memory.
    OutputKeys[IN.DispatchThreadID.x] = gs_Keys[IN.GroupIndex];
    OutputValues[IN.DispatchThreadID.x] = gs_Values[IN.GroupIndex];
}