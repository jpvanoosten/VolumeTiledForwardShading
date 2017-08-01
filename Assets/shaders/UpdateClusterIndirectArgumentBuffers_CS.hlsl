/**
 * This compute shader will copy the unique clusters counter to two indirect
 * argument buffers.
 * One indirect argument buffer is used for the Assign Lights to Clusters compute
 * shader and the other is used to render the cluster AABB for debugging.
 * Both indirect argument buffers need to be updated in the compute pipeline 
 * because the result of the number of unique clusters will not be known until 
 * the Find Unique Clusters compute shader is finished.
 */

cbuffer UpdateClustersCB : register( b0 )
{
    // When freezing the camera view to visualize debug clusters,
    // we also want to freeze the updating of the unique clusters counter 
    // so that we always visualize the clusters from the previous viewing angle.
    bool UpdateUniqueClusters;
}

ByteAddressBuffer ClusterCounter : register( t0 );
RWByteAddressBuffer AssignLightsToClustersIndirectArgumentBuffer : register( u0 );
RWByteAddressBuffer DebugClustersIndirectArgumentBuffer : register( u1 );

#define UpdateClusterIndirectArgumentBuffers_RS \
    "RootFlags(0), " \
    "RootConstants(num32BitConstants=1, b0)," \
    "DescriptorTable( SRV(t0), UAV(u0, numDescriptors=2) )"

[RootSignature( UpdateClusterIndirectArgumentBuffers_RS )]
[numthreads(1, 1, 1)]
void main()
{
    // Read the cluster counter.
    uint clusterCount = ClusterCounter.Load( 0 );

    // Update the indirect argument buffers.
    AssignLightsToClustersIndirectArgumentBuffer.Store3( 0, uint3( clusterCount, 1, 1 ) ); // NumThreadGroupsX, NumThreadGroupsY, NumThreadGroupsZ

    if ( UpdateUniqueClusters )
    {
        DebugClustersIndirectArgumentBuffer.Store4( 0, uint4( clusterCount, 1, 0, 0 ) ); // VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation
    }
}