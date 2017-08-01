#define NUM_THREADS 1024

/**
 * Inputs that are passed to a compute shader.
 */
struct ComputeShaderInput
{
    uint3 GroupID           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 GroupThreadID     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 DispatchThreadID  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  GroupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

// A buffer that contains a set flag for clusters that contain samples.
StructuredBuffer<bool> ClusterFlags : register( t0 );
// For each unique cluster, append the 1D cluster index.
RWStructuredBuffer<uint> UniqueClusters : register( u0 );

// 0. StructuredBuffer<bool> ClusterFlags : register( t0 );
//    RWStructuredBuffer<uint> UniqueClusters : register( u0 );
#define FindUniqueClusters_RootSignature \
    "RootFlags(0), " \
    "DescriptorTable( SRV(t0),UAV(u0) )"

[RootSignature( FindUniqueClusters_RootSignature )]
[numthreads( NUM_THREADS, 1, 1)]
void main( ComputeShaderInput IN )
{
    uint clusterID = IN.DispatchThreadID.x;
    if ( ClusterFlags[clusterID] )
    {
        uint i = UniqueClusters.IncrementCounter();
        UniqueClusters[i] = clusterID;
    }
}