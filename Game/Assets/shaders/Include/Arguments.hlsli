#ifndef __COMMON_INCLUDE_HLSL__
#error Do not include this header directly. Only include this file via CommonInclude.hlsli.
#endif

/*******************************************************************************
 *
 * Constant buffers
 *
 *******************************************************************************/

// Per object constant buffer data.
cbuffer _PerObjectCB : register( b0 )
{
    PerObjectData PerObjectDataCB;
}

cbuffer _MaterialCB : register( b1 )
{
    Material MaterialCB;
}

cbuffer _LightCountsCB : register( b2 )
{
    LightCounts LightCountsCB;
}

cbuffer CameraParamsCB : register( b3 )
{
    CameraParameters Camera;
}

cbuffer _DispatchParamsCB : register( b4 )
{
    DispatchParams DispatchParamsCB;
}

cbuffer _ClusterDataCB : register( b5 )
{
    ClusterData ClusterCB;
}

cbuffer _UpdateLightsCB : register( b6 )
{
    UpdateLights UpdateLightsCB;
}

cbuffer _ReductionParamsCB : register( b7 )
{
    ReductionParams ReductionParamsCB;
};

cbuffer _SortParamsCB : register( b8 )
{
    SortParams SortParamsCB;
}

cbuffer _BVHParamsCB : register( b9 )
{
    BVHParams BVHParamsCB;
}

/*******************************************************************************
 *
 * Samplers
 *
 *******************************************************************************/

// Samplers
SamplerState LinearRepeatSampler     : register( s0 );
SamplerState LinearClampSampler      : register( s1 );
SamplerState AnisotropicSampler      : register( s2 );

/*******************************************************************************
 *
 * SRV's
 *
 *******************************************************************************/

// Textures
Texture2D AmbientTexture        : register( t0 );
Texture2D EmissiveTexture       : register( t1 );
Texture2D DiffuseTexture        : register( t2 );
Texture2D SpecularTexture       : register( t3 );
Texture2D SpecularPowerTexture  : register( t4 );
Texture2D NormalTexture         : register( t5 );
Texture2D BumpTexture           : register( t6 );
Texture2D OpacityTexture        : register( t7 );

// Lights
StructuredBuffer<PointLight> PointLights : register( t8 );
StructuredBuffer<SpotLight> SpotLights : register( t9 );
StructuredBuffer<DirectionalLight> DirectionalLights : register( t10 );

// Light index list for Forward+ shading.
StructuredBuffer<uint> PointLightIndexList : register( t11 );
StructuredBuffer<uint> SpotLightIndexList : register( t12 );

// Light grid for Forward+ shading.
Texture2D<uint2> PointLightGrid : register( t13 );
Texture2D<uint2> SpotLightGrid : register( t14 );

/**
 * Cluster colors. Used for debugging for forward clustered rendering.
 */
StructuredBuffer<float4> ClusterColors : register( t15 );

/**
 * A list of the unique clusters.
 * @see FindUniqueClusters_CS.hlsl
 */
StructuredBuffer<uint> UniqueClusters : register( t16 );
StructuredBuffer<AABB> ClusterAABBs : register( t17 );

/**
 * Depth texture is used to perform light culling for Forward tiled rendering.
 */
Texture2DMS<float> DepthTexture : register( t18 );
// Precomputed frustums for the grid.
StructuredBuffer<Frustum> Frustums : register( t19 );

// Output texture for debugging purposes.
Texture2D LightCountHeatMap : register( t20 );

// Global light index lists for clustered shading.
StructuredBuffer<uint> PointLightIndexList_Cluster : register( t21 );
StructuredBuffer<uint> SpotLightIndexList_Cluster : register( t22 );

// Global light grids for clustered shading.
StructuredBuffer<uint2> PointLightGrid_Cluster : register( t23 );
StructuredBuffer<uint2> SpotLightGrid_Cluster : register( t24 );

/**
* Global AABB for scene lights.
* This buffer is used for reducing light AABB's to a single
* AABB that covers all of the lights in the scene.
* The global AABB for all lights is used to compute the
* normalized light positions within the AABB. This is later used to
* compute the morton code for the light source which will later
* be used to compute a BVH for all the lights in the scene.
*/
StructuredBuffer<AABB> LightAABB : register( t25 );

// Input keys for the radix and merge sort compute shaders.
StructuredBuffer<uint> InputKeys : register( t26 );
// Input values for the radix and merge sort compute shaders.
StructuredBuffer<uint> InputValues : register( t27 );

// The merge path partions for each thread block.
StructuredBuffer<int> MergePathPartitions : register( t28 );

// Sorted point light indices for building the BVH.
StructuredBuffer<uint> PointLightIndices : register( t29 );
// Sorted spot light indices for building the BVH.
StructuredBuffer<uint> SpotLightIndices : register( t30 );

// Point light BVH.
StructuredBuffer<AABB> PointLightBVH : register( t31 );
// Spot light BVH.
StructuredBuffer<AABB> SpotLightBVH : register( t32 );


/*******************************************************************************
 *
 * UAV's
 *
 *******************************************************************************/

RWStructuredBuffer<PointLight> RWPointLights : register( u0 );
RWStructuredBuffer<SpotLight> RWSpotLights : register( u1 );
RWStructuredBuffer<DirectionalLight> RWDirectionalLights : register( u2 );

/**
 * Cluster Axis-Aligned bounding boxes.
 */
RWStructuredBuffer<AABB> RWClusterAABBs : register( u3 );

/**
 * The ClusterFlags buffer stores a true flag wherever a sample has been
 * writen by the fragment shader. Flagged clusters will be used for
 * light assignments.
 */
RWStructuredBuffer<bool> RWClusterFlags : register( u4 );

/**
 * View space frustums for the grid cells used in Forward+ rendering.
 */
RWStructuredBuffer<Frustum> RWFrustums : register( u5 );

/**
 * Debug texture for displaying tiled light counts.
 */
RWTexture2D<float4> RWDebugTexture : register( u6 );

/**
 * Point and Spot light index counters for opaque and transparent passes.
 */
RWStructuredBuffer<uint> PointLightIndexCounter_OPAQUE : register( u7 );
RWStructuredBuffer<uint> PointLightIndexCounter_TRANSPARENT : register( u8 );
RWStructuredBuffer<uint> SpotLightIndexCounter_OPAQUE : register( u9 );
RWStructuredBuffer<uint> SpotLightIndexCounter_TRANSPARENT : register( u10 );

/**
 * Point and Spot light index lists for opaque and transparent passes.
 */
RWStructuredBuffer<uint> PointLightIndexList_OPAQUE : register( u11 );
RWStructuredBuffer<uint> PointLightIndexList_TRANSPARENT : register( u12 );
RWStructuredBuffer<uint> SpotLightIndexList_OPAQUE : register( u13 );
RWStructuredBuffer<uint> SpotLightIndexList_TRANSPARENT : register( u14 );

/**
 * Point and Spot light grids for opaque and transparent passes.
 */
RWTexture2D<uint2> PointLightGrid_OPAQUE : register( u15 );
RWTexture2D<uint2> PointLightGrid_TRANSPARENT : register( u16 );
RWTexture2D<uint2> SpotLightGrid_OPAQUE : register( u17 );
RWTexture2D<uint2> SpotLightGrid_TRANSPARENT : register( u18 );

/**
 * Point and Spot light indicies for clustered rendering.
 */
RWStructuredBuffer<uint> RWPointLightIndexCounter_Cluster : register( u19 );
RWStructuredBuffer<uint> RWSpotLightIndexCounter_Cluster : register( u20 );

/**
 * Light grids for clustered rending
 */
RWStructuredBuffer<uint2> RWPointLightGrid_Cluster : register( u21 );
RWStructuredBuffer<uint2> RWSpotLightGrid_Cluster : register( u22 );

/**
 * Global light index lists for clustered rendering.
 */
RWStructuredBuffer<uint> RWPointLightIndexList_Cluster : register( u23 );
RWStructuredBuffer<uint> RWSpotLightIndexList_Cluster : register( u24 );

/**
 * Global AABB for scene lights.
 * This buffer is used for reducing light AABB's to a single
 * AABB that covers all of the lights in the scene.
 * The global AABB for all lights is used to compute the 
 * normalized light positions within the AABB. This is later used to 
 * compute the morton code for the light source which will later
 * be used to compute a BVH for all the lights in the scene.
 */
RWStructuredBuffer<AABB> RWLightAABB : register( u25 );

/**
 * To construct the BVH for the light hierarchy, the lights will be sorted
 * based on their morton codes.
 * This buffer is used to store the morton codes for the lights.
 */
RWStructuredBuffer<uint> RWPointLightMortonCodes : register( u26 );
RWStructuredBuffer<uint> RWSpotLightMortonCodes : register( u27 );
RWStructuredBuffer<uint> RWPointLightIndices : register( u28 );
RWStructuredBuffer<uint> RWSpotLightIndices : register( u29 );

// Output keys for the radix/merge sort compute shaders.
RWStructuredBuffer<uint> OutputKeys : register( u30 );
// Output values for the radix/merge sort compute shaders.
RWStructuredBuffer<uint> OutputValues : register( u31 );

// The merge path partitions per thread block.
// This will be filled by the MergePathPartitions compute shader.
RWStructuredBuffer<int> RWMergePathPartitions : register( u32 );

// BVH structure.
RWStructuredBuffer<AABB> RWPointLightBVH : register( u33 );
RWStructuredBuffer<AABB> RWSpotLightBVH : register( u34 );

// Used for displaying the light counts for clustered shading.
RWTexture2D<uint2> RWLightCounts : register( u35 );
