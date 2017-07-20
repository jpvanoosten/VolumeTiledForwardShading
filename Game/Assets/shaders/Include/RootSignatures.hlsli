#ifndef __COMMON_INCLUDE_HLSL__
#error Do not include this header directly. Only include this file via CommonInclude.hlsli.
#endif

/**
 * This file contains all of the root signatures used throught all of the various
 * Shader implementations.
 */

// Simple Shading
// 0. cbuffer _PerObjectCB : register( b0 )
// 1. cbuffer _MaterialCB : register( b1 )
// 2. cbuffer _LightCountsCB : register( b2 )
// 3. Texture2D AmbientTexture        : register( t0 );
//    Texture2D EmissiveTexture       : register( t1 );
//    Texture2D DiffuseTexture        : register( t2 );
//    Texture2D SpecularTexture       : register( t3 );
//    Texture2D SpecularPowerTexture  : register( t4 );
//    Texture2D NormalTexture         : register( t5 );
//    Texture2D BumpTexture           : register( t6 );
//    Texture2D OpacityTexture        : register( t7 );
//------------------------------------------------------------------------------
//    StructuredBuffer<PointLight> PointLights : register( t8 );
//    StructuredBuffer<SpotLight> SpotLights : register( t9 );
//    StructuredBuffer<DirectionalLight> DirectionalLights : register( t10 );
// Samplers:
//    SamplerState LinearRepeatSampler     : register( s0 );
//    SamplerState LinearClampSampler      : register( s1 );
//    SamplerState AnisotropicSampler      : register( s2 );
#define SimpleVS_RS \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
    "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX)," \
    "CBV(b1, visibility = SHADER_VISIBILITY_PIXEL)," \
    "RootConstants(num32BitConstants=3, b2, visibility = SHADER_VISIBILITY_PIXEL)," \
    "DescriptorTable(SRV(t0, numDescriptors=11), visibility=SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s0, filter=FILTER_MIN_MAG_MIP_LINEAR, visibility=SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s1, filter=FILTER_MIN_MAG_MIP_LINEAR," \
                      "addressU=TEXTURE_ADDRESS_CLAMP," \
                      "addressV=TEXTURE_ADDRESS_CLAMP," \
                      "visibility = SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s2, filter = FILTER_ANISOTROPIC," \
                      "maxAnisotropy = 16," \
                      "visibility = SHADER_VISIBILITY_PIXEL)"

// Display a debug texture.
// 0. cbuffer _PerObjectCB : register( b0 )
// 1. Texture2D DebugTexture : register( t0 );
// Samplers:
//    SamplerState LinearRepeatSampler     : register( s0 );
//    SamplerState LinearClampSampler      : register( s1 );
//    SamplerState AnisotropicSampler      : register( s2 );
#define DebugTexture_RS \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
    "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX)," \
    "DescriptorTable(SRV(t0, numDescriptors=1), visibility=SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s0, filter=FILTER_MIN_MAG_MIP_LINEAR, visibility=SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s1, filter=FILTER_MIN_MAG_MIP_LINEAR," \
                      "addressU=TEXTURE_ADDRESS_CLAMP," \
                      "addressV=TEXTURE_ADDRESS_CLAMP," \
                      "visibility = SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s2, filter = FILTER_ANISOTROPIC," \
                      "maxAnisotropy = 16," \
                      "visibility = SHADER_VISIBILITY_PIXEL)"

// Forward Shading
// 0. cbuffer _PerObjectCB : register( b0 )
// 1. cbuffer _MaterialCB : register( b1 )
// 2. cbuffer _LightCountsCB : register( b2 )
// 3. Texture2D AmbientTexture        : register( t0 );
//    Texture2D EmissiveTexture       : register( t1 );
//    Texture2D DiffuseTexture        : register( t2 );
//    Texture2D SpecularTexture       : register( t3 );
//    Texture2D SpecularPowerTexture  : register( t4 );
//    Texture2D NormalTexture         : register( t5 );
//    Texture2D BumpTexture           : register( t6 );
//    Texture2D OpacityTexture        : register( t7 );
//------------------------------------------------------------------------------
//    StructuredBuffer<PointLight> PointLights : register( t8 );
//    StructuredBuffer<SpotLight> SpotLights : register( t9 );
//    StructuredBuffer<DirectionalLight> DirectionalLights : register( t10 );
// Samplers: 
//    SamplerState LinearRepeatSampler     : register( s0 );
//    SamplerState LinearClampSampler      : register( s1 );
//    SamplerState AnisotropicSampler      : register( s2 );
#define ForwardVS_RS \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
    "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX)," \
    "CBV(b1, visibility = SHADER_VISIBILITY_PIXEL)," \
    "RootConstants(num32BitConstants=3, b2, visibility = SHADER_VISIBILITY_PIXEL)," \
    "DescriptorTable(SRV(t0, numDescriptors=11), visibility=SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s0, filter=FILTER_MIN_MAG_MIP_LINEAR, visibility=SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s1, filter=FILTER_MIN_MAG_MIP_LINEAR," \
                      "addressU=TEXTURE_ADDRESS_CLAMP," \
                      "addressV=TEXTURE_ADDRESS_CLAMP," \
                      "visibility = SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s2, filter = FILTER_ANISOTROPIC," \
                      "maxAnisotropy = 16," \
                      "visibility = SHADER_VISIBILITY_PIXEL)"

// Forward+ Shading
// 0. cbuffer _PerObjectCB : register( b0 )
// 1. cbuffer _MaterialCB : register( b1 )
// 2. cbuffer _LightCountsCB : register( b2 )
// 3. Texture2D AmbientTexture        : register( t0 );
//    Texture2D EmissiveTexture       : register( t1 );
//    Texture2D DiffuseTexture        : register( t2 );
//    Texture2D SpecularTexture       : register( t3 );
//    Texture2D SpecularPowerTexture  : register( t4 );
//    Texture2D NormalTexture         : register( t5 );
//    Texture2D BumpTexture           : register( t6 );
//    Texture2D OpacityTexture        : register( t7 );
//------------------------------------------------------------------------------
//    StructuredBuffer<PointLight> PointLights : register( t8 );
//    StructuredBuffer<SpotLight> SpotLights : register( t9 );
//    StructuredBuffer<DirectionalLight> DirectionalLights : register( t10 );
//    StructuredBuffer<uint> PointLightIndexList : register( t11 );
//    StructuredBuffer<uint> SpotLightIndexList : register( t12 );
//    Texture2D<uint2> PointLightGrid : register( t13 );
//    Texture2D<uint2> SpotLightGrid : register( t14 );
// Samplers
//    SamplerState LinearRepeatSampler     : register( s0 );
//    SamplerState LinearClampSampler      : register( s1 );
//    SamplerState AnisotropicSampler      : register( s2 );
#define ForwardPlusVS_RS \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
    "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX)," \
    "CBV(b1, visibility = SHADER_VISIBILITY_PIXEL)," \
    "RootConstants(num32BitConstants=3, b2, visibility = SHADER_VISIBILITY_PIXEL)," \
    "DescriptorTable(SRV(t0, numDescriptors=15), visibility=SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s0, filter=FILTER_MIN_MAG_MIP_LINEAR, visibility=SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s1, filter=FILTER_MIN_MAG_MIP_LINEAR," \
                      "addressU=TEXTURE_ADDRESS_CLAMP," \
                      "addressV=TEXTURE_ADDRESS_CLAMP," \
                      "visibility = SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s2, filter = FILTER_ANISOTROPIC," \
                      "maxAnisotropy = 16," \
                      "visibility = SHADER_VISIBILITY_PIXEL)"

// Clustered Shading
// 0. cbuffer _PerObjectCB : register( b0 )
// 1. cbuffer _MaterialCB : register( b1 )
// 2. cbuffer _LightCountsCB : register( b2 )
// 3. Texture2D AmbientTexture        : register( t0 );
//    Texture2D EmissiveTexture       : register( t1 );
//    Texture2D DiffuseTexture        : register( t2 );
//    Texture2D SpecularTexture       : register( t3 );
//    Texture2D SpecularPowerTexture  : register( t4 );
//    Texture2D NormalTexture         : register( t5 );
//    Texture2D BumpTexture           : register( t6 );
//    Texture2D OpacityTexture        : register( t7 );
//------------------------------------------------------------------------------
//    StructuredBuffer<PointLight> PointLights : register( t8 );
//    StructuredBuffer<SpotLight> SpotLights : register( t9 );
//    StructuredBuffer<DirectionalLight> DirectionalLights : register( t10 );
//------------------------------------------------------------------------------
//    StructuredBuffer<uint> PointLightIndexList_Cluster : register( t21 );
//    StructuredBuffer<uint> SpotLightIndexList_Cluster : register( t22 );
//    StructuredBuffer<uint2> PointLightGrid_Cluster : register( t23 );
//    StructuredBuffer<uint2> SpotLightGrid_Cluster : register( t24 );
// 4. cbuffer _ClusterDataCB : register( b5 )
// Samplers
//    SamplerState LinearRepeatSampler     : register( s0 );
//    SamplerState LinearClampSampler      : register( s1 );
//    SamplerState AnisotropicSampler      : register( s2 );
#define ClusteredVS_RS \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
    "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX)," \
    "CBV(b1, visibility = SHADER_VISIBILITY_PIXEL)," \
    "RootConstants(num32BitConstants=3, b2, visibility = SHADER_VISIBILITY_PIXEL)," \
    "DescriptorTable(SRV(t0, numDescriptors=11), SRV(t21, numDescriptors=4), visibility=SHADER_VISIBILITY_PIXEL)," \
    "RootConstants(num32BitConstants=8, b5, visibility = SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s0, filter=FILTER_MIN_MAG_MIP_LINEAR, visibility=SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s1, filter=FILTER_MIN_MAG_MIP_LINEAR," \
                      "addressU=TEXTURE_ADDRESS_CLAMP," \
                      "addressV=TEXTURE_ADDRESS_CLAMP," \
                      "visibility = SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s2, filter = FILTER_ANISOTROPIC," \
                      "maxAnisotropy = 16," \
                      "visibility = SHADER_VISIBILITY_PIXEL)"


// Cluster Samples
// 0. cbuffer _PerObjectCB : register( b0 )
// 1. cbuffer _ClusterDataCB : register( b5 )
// 2. Material textures t0-t7, RWStructuredBuffer<bool> RWClusterFlags : register( u4 );
// 3. StructuredBuffer<float4> ClusterColors
#define ClusterSamples_RS \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
    "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX)," \
    "RootConstants(num32BitConstants=8, b5, visibility = SHADER_VISIBILITY_PIXEL)," \
    "DescriptorTable(SRV(t0, numDescriptors=8),UAV(u4), visibility=SHADER_VISIBILITY_PIXEL)," \
    "DescriptorTable(SRV(t15), visibility=SHADER_VISIBILITY_PIXEL )"

// Compute Cluster AABBs
// 0. cbuffer CameraParamsCB : register( b3 )
// 1. cbuffer _ClusterDataCB : register( b5 )
// 2. RWStructuredBuffer<AABB> RWClusterAABBs : register( u3 );
#define ComputeClusterAABBs_RS \
    "RootFlags(0), " \
    "CBV(b3)," \
    "RootConstants(num32BitConstants=8, b5)," \
    "UAV(u3)"

// Update Lights
// 0. cbuffer _UpdateLightsCB : register( b6 )
// 1. cbuffer _LightCountsCB : register( b2 )
// 2. RWStructuredBuffer<PointLight> RWPointLights : register( u0 ), 
//    RWStructuredBuffer<SpotLight> RWSpotLights : register( u1 ), 
//    RWStructuredBuffer<DirectionalLight> RWDirectionalLights : register( u2 );
#define UpdateLights_RS \
    "RootFlags(0), " \
    "CBV(b6)," \
    "RootConstants(b2, num32BitConstants = 3), " \
    "DescriptorTable( UAV(u0, numDescriptors = 3) )"

// Compute Grid Frustums
// 0. cbuffer CameraParamsCB : register( b3 )
// 1. cbuffer _DispatchParamsCB : register( b4 )
// 2. RWStructuredBuffer<Frustum> RWFrustums : register( u5 );
#define ComputeGridFrustums_RS \
    "RootFlags(0), " \
    "CBV(b3)," \
    "RootConstants(num32BitConstants=8, b4)," \
    "UAV(u5)"

// Cull lights for Forward+ rendering algorithm.
// 0. cbuffer _LightCountsCB : register( b2 )
// 1. cbuffer CameraParamsCB : register( b3 )
// 2. cbuffer _DispatchParamsCB : register( b4 )
// 3. StructuredBuffer<PointLight> PointLights : register( t8 );
//    StructuredBuffer<SpotLight> SpotLights : register( t9 );
//    StructuredBuffer<DirectionalLight> DirectionalLights : register( t10 );
//------------------------------------------------------------------------------
//    Texture2D DepthTexture : register( t18 );
//    StructuredBuffer<Frustum> Frustums : register( t19 );
//    Texture2D LightCountHeatMap : register( t20 );
//------------------------------------------------------------------------------
//    RWTexture2D<float4> RWDebugTexture : register( u6 );
//    RWStructuredBuffer<uint> PointLightIndexCounter_OPAQUE : register( u7 );
//    RWStructuredBuffer<uint> PointLightIndexCounter_TRANSPARENT : register( u8 );
//    RWStructuredBuffer<uint> SpotLightIndexCounter_OPAQUE : register( u9 );
//    RWStructuredBuffer<uint> SpotLightIndexCounter_TRANSPARENT : register( u10 );
//    RWStructuredBuffer<uint> PointLightIndexList_OPAQUE : register( u11 );
//    RWStructuredBuffer<uint> PointLightIndexList_TRANSPARENT : register( u12 );
//    RWStructuredBuffer<uint> SpotLightIndexList_OPAQUE : register( u13 );
//    RWStructuredBuffer<uint> SpotLightIndexList_TRANSPARENT : register( u14 );
//    RWTexture2D<uint2> PointLightGrid_OPAQUE : register( u15 );
//    RWTexture2D<uint2> PointLightGrid_TRANSPARENT : register( u16 );
//    RWTexture2D<uint2> SpotLightGrid_OPAQUE : register( u17 );
//    RWTexture2D<uint2> SpotLightGrid_TRANSPARENT : register( u18 );
//------------------------------------------------------------------------------
#define CullLights_RS \
    "RootFlags(0)," \
    "RootConstants(num32BitConstants=3,b2)," \
    "CBV(b3)," \
    "RootConstants(num32BitConstants=8, b4)," \
    "DescriptorTable(SRV(t8, numDescriptors=3),SRV(t18, numDescriptors=3),UAV(u6, numDescriptors=13))," \
    "StaticSampler(s1, filter=FILTER_MIN_MAG_MIP_LINEAR," \
                      "addressU=TEXTURE_ADDRESS_CLAMP," \
                      "addressV=TEXTURE_ADDRESS_CLAMP)"

// Assign Lights to Clusters
// 0. cbuffer _LightCountsCB : register( b2 )
// 1. StructuredBuffer<PointLight> PointLights : register( t8 );
//    StructuredBuffer<SpotLight> SpotLights : register( t9 );
//------------------------------------------------------------------------------
//    StructuredBuffer<uint> UniqueClusters : register( t16 );
//    StructuredBuffer<AABB> ClusterAABBs : register( t17 );
//------------------------------------------------------------------------------
//    RWStructuredBuffer<uint> RWPointLightIndexCounter_Cluster : register( u19 );
//    RWStructuredBuffer<uint> RWSpotLightIndexCounter_Cluster : register( u20 );
//    RWStructuredBuffer<uint2> RWPointLightGrid_Cluster : register( u21 );
//    RWStructuredBuffer<uint2> RWSpotLightGrid_Cluster : register( u22 );
//    RWStructuredBuffer<uint> RWPointLightIndexList_Cluster : register( u23 );
//    RWStructuredBuffer<uint> RWSpotLightIndexList_Cluster : register( u24 );
#define AssignLightsToClusters_RS \
    "RootFlags(0)," \
    "RootConstants(num32BitConstants=3,b2)," \
    "DescriptorTable( SRV( t8, numDescriptors=2), SRV( t16, numDescriptors=2 ), UAV( u19, numDescriptors=6 ) )"

// Assign Lights to Clusters (BVH)
// 0. cbuffer _BVHParamsCB : register( b9 )
// 1. cbuffer _LightCountsCB : register( b2 )
// 2. StructuredBuffer<PointLight> PointLights : register( t8 );
//    StructuredBuffer<SpotLight> SpotLights : register( t9 );
//------------------------------------------------------------------------------
//    StructuredBuffer<uint> UniqueClusters : register( t16 );
//    StructuredBuffer<AABB> ClusterAABBs : register( t17 );
//------------------------------------------------------------------------------
//    StructuredBuffer<uint> PointLightIndices : register( t29 );
//    StructuredBuffer<uint> SpotLightIndices : register( t30 );
//    StructuredBuffer<AABB> PointLightBVH : register( t31 );
//    StructuredBuffer<AABB> SpotLightBVH : register( t32 );
//------------------------------------------------------------------------------
//    RWStructuredBuffer<uint> RWPointLightIndexCounter_Cluster : register( u19 );
//    RWStructuredBuffer<uint> RWSpotLightIndexCounter_Cluster : register( u20 );
//    RWStructuredBuffer<uint2> RWPointLightGrid_Cluster : register( u21 );
//    RWStructuredBuffer<uint2> RWSpotLightGrid_Cluster : register( u22 );
//    RWStructuredBuffer<uint> RWPointLightIndexList_Cluster : register( u23 );
//    RWStructuredBuffer<uint> RWSpotLightIndexList_Cluster : register( u24 );
#define AssignLightsToClustersBVH_RS \
    "RootFlags(0)," \
    "RootConstants(num32BitConstants=3,b9)," \
    "RootConstants(num32BitConstants=3,b2)," \
    "DescriptorTable( SRV( t8, numDescriptors=2), SRV( t16, numDescriptors=2 ), SRV( t29, numDescriptors=4 ), UAV( u19, numDescriptors=6 ) )"

// Debug Lights
// 0. cbuffer _PerObjectCB : register( b0 )
// 1. cbuffer _MaterialCB : register( b1 )
// 2. cbuffer _LightCountsCB : register( b2 )
// 3. StructuredBuffer<PointLight> PointLights : register( t8 );
//    StructuredBuffer<SpotLight> SpotLights : register( t9 );
//    StructuredBuffer<DirectionalLight> DirectionalLights : register( t10 );
#define DebugLightsVS_RS \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
    "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX)," \
    "CBV(b1, visibility = SHADER_VISIBILITY_PIXEL)," \
    "RootConstants(num32BitConstants=3, b2, visibility = SHADER_VISIBILITY_PIXEL)," \
    "DescriptorTable(SRV(t8, numDescriptors=3), visibility=SHADER_VISIBILITY_ALL)"

// Reduce Lights AABB
// 0. cbuffer _LightCountsCB : register( b2 )
// 1. cbuffer _DispatchParamsCB : register( b4 )
// 2. cbuffer _ReductionParamsCB : register( b7 )
// 3. StructuredBuffer<PointLight> PointLights : register( t8 );
//    StructuredBuffer<SpotLight> SpotLights : register( t9 );
//------------------------------------------------------------------------------
//    RWStructuredBuffer<AABB> RWLightAABB : register( u25 );
#define ReduceLightsAABB_RS \
    "RootFlags(0)," \
    "RootConstants(num32BitConstants=3,b2)," \
    "RootConstants(num32BitConstants=8, b4)," \
    "RootConstants(num32BitConstants=1, b7)," \
    "DescriptorTable( SRV(t8, numDescriptors=2), UAV(u25) )"


// Compute light Morton codes
// 0. cbuffer _LightCountsCB : register( b2 )
// 1. StructuredBuffer<PointLight> PointLights : register( t8 );
//    StructuredBuffer<SpotLight> SpotLights : register( t9 );
//------------------------------------------------------------------------------
//    StructuredBuffer<AABB> LightAABB : register( t25 );
//------------------------------------------------------------------------------
//    RWStructuredBuffer<uint> RWPointLightMortonCodes : register( u26 );
//    RWStructuredBuffer<uint> RWSpotLightMortonCodes : register( u27 );
//    RWStructuredBuffer<uint> RWPointLightIndices : register( 28 );
//    RWStructuredBuffer<uint> RWSpotLightIndices : register( 29 )

#define ComputeLightMortonCodes_RS \
    "RootFlags(0)," \
    "RootConstants(num32BitConstants=3,b2)," \
    "DescriptorTable( SRV(t8, numDescriptors=2), SRV(t25), UAV(u26, numDescriptors=4) )"

// Radix sort
// 0. cbuffer _SortParamsCB : register( b8 )
// 1. StructuredBuffer<uint> InputKeys : register( t26 );
//    StructuredBuffer<uint> InputValues : register( t27 );
//------------------------------------------------------------------------------
//    RWStructuredBuffer<uint> OutputKeys : register( u30 );
//    RWStructuredBuffer<uint> OutputValues : register( u31 );
#define RadixSort_RS \
    "RootFlags(0)," \
    "RootConstants(num32BitConstants=2, b8)," \
    "DescriptorTable( SRV(t26, numDescriptors=2), UAV(u30, numDescriptors=2) )"

// Merge sort.
// 0. cbuffer _MergeRangesCB : register( b9 )
// 1. StructuredBuffer<uint> InputKeys : register( t26 );
//    StructuredBuffer<uint> InputValues : register( t27 );
//    StructuredBuffer<int> MergePathPartitions : register( 28 );
//------------------------------------------------------------------------------
//    RWStructuredBuffer<uint> OutputKeys : register( u30 );
//    RWStructuredBuffer<uint> OutputValues : register( u31 );
//    RWStructuredBuffer<int> RWMergePathPartitions : register( u32 );
#define MergeSort_RS \
    "RootFlags(0)," \
    "RootConstants(num32BitConstants=2, b8)," \
    "DescriptorTable( SRV(t26, numDescriptors=3), UAV(u30, numDescriptors=3) )"


// Build BVH.
// 0. cbuffer _BVHParamsCB : register( b9 )
// 1. cbuffer _LightCountsCB : register( b2 )
// 2. StructuredBuffer<PointLight> PointLights : register( t8 );
//    StructuredBuffer<SpotLight> SpotLights : register( t9 );
//------------------------------------------------------------------------------
//    StructuredBuffer<uint> PointLightIndices : register( t29 );
//    StructuredBuffer<uint> SpotLightIndices : register( t30 );
//------------------------------------------------------------------------------
//    RWStructuredBuffer<AABB> RWPointLightBVH : register( u33 );
//    RWStructuredBuffer<AABB> RWSpotLightBVH : register( u34 );
#define BuildBVH_RS \
    "RootFlags(0)," \
    "RootConstants(num32BitConstants=3, b9)," \
    "RootConstants(num32BitConstants=3, b2)," \
    "DescriptorTable( SRV(t8, numDescriptors=2), SRV(t29, numDescriptors=2), UAV(u33, numDescriptors=2) )"


// Count lights for clustered rendering.
// 0. cbuffer CameraParamsCB : register( b3 )
// 1. cbuffer _ClusterDataCB : register( b5 )
// 2. Texture2DMS<float> DepthTexture : register( t18 );
//    Texture2D LightCountHeatMap : register( t20 );
//    StructuredBuffer<uint2> PointLightGrid_Cluster : register( t23 );
//    StructuredBuffer<uint2> SpotLightGrid_Cluster : register( t24 );
//------------------------------------------------------------------------------
//    RWTexture2D<float4> RWDebugTexture : register( u6 );
// Samplers:
//    SamplerState LinearClampSampler      : register( s1 );
#define CountLights_RS \
    "RootFlags(0)," \
    "CBV(b3)," \
    "RootConstants(num32BitConstants=8, b5)," \
    "DescriptorTable( SRV(t18), SRV(t20), SRV(t23, numDescriptors=2), UAV(u6) )," \
    "StaticSampler(s1, filter=FILTER_MIN_MAG_MIP_LINEAR," \
        "addressU=TEXTURE_ADDRESS_CLAMP," \
        "addressV=TEXTURE_ADDRESS_CLAMP )"
