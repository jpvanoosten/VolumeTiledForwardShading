#include "Include/CommonInclude.hlsli"

/**
 * Perform a parallel merge sort over two sorted lists A, and B.
 * This results in a third sorted list C that is the result of merging A and B.
 * This shader uses MergePath to determine which elelments should be sorted per thread.
 * Based on "GPU Merge Path - A GPU Merging Algorithm" (2012), Oded Gree, Robert McColl, David A. Bader.
 * Based on https://moderngpu.github.io/bulkinsert.html#mergepath
 * Retrieved on Aug 8, 2016.
 */

#ifndef NUM_THREADS
#define NUM_THREADS 256
#endif

// The number of values that each thread will merge.
#ifndef NUM_VALUES_PER_THREAD
#define NUM_VALUES_PER_THREAD 8
#endif

#define NUM_VALUES_PER_THREAD_GROUP ( NUM_THREADS * NUM_VALUES_PER_THREAD )

#define INT_MAX 0xffffffff

groupshared uint gs_Keys[NUM_VALUES_PER_THREAD_GROUP];      // Intermediate keys.               (8,192 Bytes)
groupshared uint gs_Values[NUM_VALUES_PER_THREAD_GROUP];    // Intermediate values.             (8,192 Bytes)

/**
 * MergePath is a binary search over two sorted arrays that finds the 
 * point in list A and list B to begin a merge operation.
 * Based on: https://moderngpu.github.io/bulkinsert.html#mergepath
 * Retrieved on: Aug 9, 2016.
 * 
 * @param a0 The first element in list A.
 * @param aCount The number of elements in A.
 * @param b0 The first element in list B.
 * @param bCount The number of elements in B.
 * @param diag The cross diagonal of the merge matrix where the merge path is computed.
 * @param bUseSharedMem Whether to read from shared memory or global memory.
 * @return 
 */
int MergePath( int a0, int aCount, int b0, int bCount, int diag, bool bUseSharedMem )
{
    int begin = max( 0, diag - bCount );
    int end = min( diag, aCount );

    while ( begin < end )
    {
        // Find the mid-point to start searching from.
        int mid = ( begin + end ) >> 1;
        uint a = bUseSharedMem ? gs_Keys[a0 + mid] : InputKeys[a0 + mid];
        uint b = bUseSharedMem ? gs_Keys[b0 + diag - 1 - mid] : InputKeys[b0 + diag - 1 - mid];
        if ( a < b )
        {
            begin = mid + 1;
        }
        else
        {
            end = mid;
        }
    }

    return begin;
}

/**
 * Compute the merge path partitions that determine
 * how list A and list B will be processed by each thread block.
 * If we know the merge path partions before executing MergeSort then 
 * we can load the subarrays of list A and list B into shared memory.
 * This allows the mergesort algorithm to operate in shared memory instead
 * of reading from global memory.
 */
[RootSignature( MergeSort_RS )]
[numthreads( NUM_THREADS, 1, 1 )]
void MergePathPartitions_CS( ComputeShaderInput IN )
{
    uint chunkSize = SortParamsCB.ChunkSize;
    // Num values to sort per sort group.
    uint numValuesPerSortGroup = min( chunkSize * 2, SortParamsCB.NumElements );

    // Number of chunks to sort.
    uint numChunks = ceil( SortParamsCB.NumElements / (float)chunkSize );
    // Number of sort groups needed to sort all chunks.
    uint numSortGroups = numChunks / 2;

    // Total number of partitions per sort group.
    uint numPartitionsPerSortGroup = ceil( numValuesPerSortGroup / (float)NUM_VALUES_PER_THREAD_GROUP ) + 1;
    // The sort group this thread is operating on.
    uint sortGroup = IN.DispatchThreadID.x / numPartitionsPerSortGroup;
    // The partition this thread is computing within the sort group.
    uint partitionInSortGroup = IN.DispatchThreadID.x % numPartitionsPerSortGroup;

    // The partition across all sort groups.
    uint globalPartition = ( sortGroup * numPartitionsPerSortGroup ) + partitionInSortGroup;
    // Compute the maximum number of partitions to compute.
    uint maxPartitions = numSortGroups * numPartitionsPerSortGroup;

    if ( globalPartition < maxPartitions )
    {
        int a0 = sortGroup * numValuesPerSortGroup;
        int a1 = min( a0 + chunkSize, SortParamsCB.NumElements );
        int aCount = a1 - a0;
        int b0 = a1;
        int b1 = min( b0 + chunkSize, SortParamsCB.NumElements );
        int bCount = b1 - b0;
        // Number of values to sort in this sort group.
        int numValues = aCount + bCount;
        // The diagonal in the merge matrix of this sort group.
        int diag = min( partitionInSortGroup * NUM_VALUES_PER_THREAD_GROUP, numValues);

        // Find the merge path for this partition using global memory.
        int mergePath = MergePath( a0, aCount, b0, bCount, diag, false );

        // Write the merge path to global memory.
        RWMergePathPartitions[globalPartition] = mergePath;
    }
}

/**
 * Perform a serial merge using shared memory. Write results to global memory.
 */
void SerialMerge( int a0, int a1, int b0, int b1, int diag, uint numValues, uint out0 )
{
    uint i, aKey, bKey, aValue, bValue;

    aKey = gs_Keys[a0];
    bKey = gs_Keys[b0];

    aValue = gs_Values[a0];
    bValue = gs_Values[b0];

    [unroll]
    for ( i = 0; i < NUM_VALUES_PER_THREAD && diag + i < numValues; ++i )
    {
        if ( b0 >= b1 || ( a0 < a1 && aKey < bKey ) )
        {
            OutputKeys[out0 + diag + i] = aKey;
            OutputValues[out0 + diag + i] = aValue;

            ++a0;

            aKey = gs_Keys[a0];
            aValue = gs_Values[a0];
        }
        else
        {
            OutputKeys[out0 + diag + i] = bKey;
            OutputValues[out0 + diag + i] = bValue;

            ++b0;

            bKey = gs_Keys[b0];
            bValue = gs_Values[b0];
        }
    }
}

[RootSignature( MergeSort_RS )]
[numthreads(NUM_THREADS, 1, 1)]
void MergeSort( ComputeShaderInput IN )
{
    uint i, key, value;

    uint chunkSize = SortParamsCB.ChunkSize;
    // Number of chunks to sort.
    uint numChunks = ceil( SortParamsCB.NumElements / (float)chunkSize );
    // Number of sort groups needed to sort all chunks.
    uint numSortGroups = max( numChunks / 2, 1 );

    // Num values to sort per sort group.
    uint numValuesPerSortGroup = min( chunkSize * 2, SortParamsCB.NumElements );
    // Compute the number of thread groups required to sort a single sort group.
    uint numThreadGroupsPerSortGroup = ceil( numValuesPerSortGroup / (float)NUM_VALUES_PER_THREAD_GROUP );
    // The number of partitions per sort group.
    // We add 1 to account for the merge path partition at the end of the sort group.
    uint numPartitionsPerSortGroup = numThreadGroupsPerSortGroup + 1;

    // Compute the sort group that this thread is operating on.
    uint sortGroup = IN.GroupID.x / numThreadGroupsPerSortGroup;
    // The merge path partition within the sort group.
    uint partition = IN.GroupID.x % numThreadGroupsPerSortGroup;

    uint globalPartition = ( sortGroup * numPartitionsPerSortGroup ) + partition;

    // Load the keys into shared memory based on the mergepath for this thread group.
    int mergePath0 = MergePathPartitions[globalPartition];
    int mergePath1 = MergePathPartitions[globalPartition + 1];
    int diag0 = min( partition * NUM_VALUES_PER_THREAD_GROUP, numValuesPerSortGroup );
    int diag1 = min( ( partition + 1 ) * NUM_VALUES_PER_THREAD_GROUP, numValuesPerSortGroup );

    // Compute the chunk ranges in the input set.
    int chunkOffsetA0 = min( sortGroup * numValuesPerSortGroup, SortParamsCB.NumElements );
    int chunkOffsetA1 = min( chunkOffsetA0 + chunkSize, SortParamsCB.NumElements );
    int chunkSizeA = chunkOffsetA1 - chunkOffsetA0;

    int chunkOffsetB0 = chunkOffsetA1;
    int chunkOffsetB1 = min( chunkOffsetB0 + chunkSize, SortParamsCB.NumElements );
    int chunkSizeB = chunkOffsetB1 - chunkOffsetB0;

    // The total number of values to be sorted.
    uint numValues = chunkSizeA + chunkSizeB;

    int a0 = mergePath0;
    int a1 = mergePath1;
    int numA = min( a1 - a0, chunkSizeA );

    int b0 = diag0 - mergePath0;
    int b1 = diag1 - mergePath1;
    int numB = min( b1 - b0, chunkSizeB );

    // Compute the diagonal for this thread within the threadgroup.
    int diag = IN.GroupIndex * NUM_VALUES_PER_THREAD;

    // Load the keys and values into shared memory.
    [unroll]
    for ( i = 0; i < NUM_VALUES_PER_THREAD; ++i )
    {
        int a = a0 + diag + i;
        int b = b0 + ( a - a1 );
        if ( a < a1 )
        {
            key = InputKeys[chunkOffsetA0 + a];
            value = InputValues[chunkOffsetA0 + a];
        }
        else
        {
            key = InputKeys[chunkOffsetB0 + b];
            value = InputValues[chunkOffsetB0 + b];
        }

        gs_Keys[diag + i] = key;
        gs_Values[diag + i] = value;
    }

    // Sync loading of keys/values in shared memory.
    GroupMemoryBarrierWithGroupSync();

    // Compute the mergepath for this thread using shared memory.
    int mergePath = MergePath( 0, numA, numA, numB, diag, true );

    // Perform the serial merge using shared memory.
    SerialMerge( mergePath, numA, numA + diag - mergePath, numA + numB, diag0 + diag, numValues, chunkOffsetA0 );
}