#include "Include/CommonInclude.hlsli"

#ifndef NUM_THREADS
#define NUM_THREADS 1024
#endif

groupshared AABB gs_AABB;
groupshared float4 gs_AABBRange;

// Produce a 3k-bit morton code from a quantized coordinate.
uint MortonCode( uint3 quantizedCoord, uint k )
{
    uint mortonCode = 0;
    uint bitMask = 1;
    uint bitShift = 0;
    uint kBits = ( 1 << k );

    while ( bitMask < kBits )
    {
        // Interleave the bits of the X, Y, and Z coordinates to produce the final Morton code.
        mortonCode |= ( quantizedCoord.x & bitMask ) << ( bitShift + 0 );
        mortonCode |= ( quantizedCoord.y & bitMask ) << ( bitShift + 1 );
        mortonCode |= ( quantizedCoord.z & bitMask ) << ( bitShift + 2 );

        bitMask <<= 1;
        bitShift += 2;
    }

    return mortonCode;
}

[RootSignature( ComputeLightMortonCodes_RS )]
[numthreads(NUM_THREADS, 1, 1)]
void main( ComputeShaderInput IN )
{
    uint4 quantized; // Quantized coordinate.

    // Generate 3k-bit morton codes.
    // For k = 10, morton codes will be 30-bits.
    const uint kBitMortonCode = 10;
    // To quantize the light's position, the light's view space position will 
    // be normalized based on the AABB that encompases all lights.
    // The normalized value will then be scaled based on the k-bits of the morton 
    // code and the resulting bits of the x, y, and z components will be interleved
    // to produce the final morton code.
    const uint coordinateScale = ( 1 << kBitMortonCode ) - 1; // This is equivalent to 2^k-1 which results in a value that when scaled by 1 will produce a number that is exactly k bits.
    
    if ( IN.GroupIndex == 0 )
    {
        gs_AABB = LightAABB[0];
        // Compute the recipocol of the range of the AABB.
        // This is used to normalize the light coordinates within the bounds of the AABB.
        gs_AABBRange = 1.0f / ( gs_AABB.Max - gs_AABB.Min );
    }

    GroupMemoryBarrierWithGroupSync();

    uint threadIndex = IN.DispatchThreadID.x;

    if ( threadIndex < LightCountsCB.NumPointLights )
    {
        PointLight pointLight = PointLights[threadIndex];
        // Normalize and scale the position of the light to produce the quantized coordinate.
        quantized = ( pointLight.PositionVS - gs_AABB.Min ) * gs_AABBRange * coordinateScale;

        RWPointLightMortonCodes[threadIndex] = MortonCode( quantized.xyz, kBitMortonCode );
        RWPointLightIndices[threadIndex] = threadIndex;
    }

    if ( threadIndex < LightCountsCB.NumSpotLights )
    {
        SpotLight spotLight = SpotLights[threadIndex];
        // Normalize and scale the position of the light to produce the quantized coordinate.
        quantized = ( spotLight.PositionVS - gs_AABB.Min ) * gs_AABBRange * coordinateScale;

        RWSpotLightMortonCodes[threadIndex] = MortonCode( quantized.xyz, kBitMortonCode );
        RWSpotLightIndices[threadIndex] = threadIndex;
    }

}