#ifndef __COMMON_INCLUDE_HLSL__
#error Do not include this header directly. Only include this file via CommonInclude.hlsli.
#endif

// The vertex layout expected to be passed from the application to the 
// vertex shader.
struct AppData
{
    float3 Position     : POSITION;
    float3 Normal       : NORMAL;
    float3 Tangent      : TANGENT;
    float3 Bitangent    : BITANGENT;
    float3 TexCoord     : TEXCOORD0;
    uint   InstanceID   : SV_InstanceID;
};

// Ouput from the vertex shader.
struct VertexShaderOutput
{
    float4 PositionVS   : VIEWSPACEPOS;         // View space position.
    float3 NormalVS     : VIEWSPACENORMAL;      // View space normal.
    float3 TangentVS    : VIEWSPACETANGENT;     // View space tangent.
    float3 BitangentVS  : VIEWSPACEBITANGENT;   // View space bitangent.
    float3 TexCoord     : TEXCOORD;             // Texture Coordinate.
    uint   InstanceID   : SV_InstanceID;
    float4 Position     : SV_POSITION;          // Clip space position.
};

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

/**
 * Material properties.
 */
struct Material
{
    float4      GlobalAmbient;
    //-------------------------- ( 16 bytes )
    float4      AmbientColor;
    //-------------------------- ( 16 bytes )
    float4      EmissiveColor;
    //-------------------------- ( 16 bytes )
    float4      DiffuseColor;
    //-------------------------- ( 16 bytes )
    float4      SpecularColor;
    //-------------------------- ( 16 bytes )
    float4      Reflectance;
    //-------------------------- ( 16 bytes )
    // If Opacity < 1, then the material is transparent.
    float       Opacity;
    float       SpecularPower;
    // For transparent materials, IOR > 0.
    float       IndexOfRefraction;
    bool        HasAmbientTexture;
    //-------------------------- ( 16 bytes )
    bool        HasEmissiveTexture;
    bool        HasDiffuseTexture;
    bool        HasSpecularTexture;
    bool        HasSpecularPowerTexture;
    //-------------------------- ( 16 bytes )
    bool        HasNormalTexture;
    bool        HasBumpTexture;
    bool        HasOpacityTexture;
    float       BumpIntensity;      // When using bump textures (heightmaps) we need 
                                    // to scale the height values so the normals are visible.
                                    //-------------------------- ( 16 bytes )
    float       SpecularScale;      // When reading specular power from a texture, 
                                    // we need to scale it into the correct range.
    float       AlphaThreshold;     // Pixels with alpha < m_AlphaThreshold will be discarded.
    float2      Padding;            // Pad to 16 byte boundary.
                                    //-------------------------- ( 16 bytes )
                                    //--------------------------- ( 16 * 10 = 160 bytes )
};

struct PointLight
{
    float4 PositionWS;                          // World space position.
                                                //--------------------( 16 bytes )
    float4 PositionVS;                          // View space position.
                                                //--------------------( 16 bytes )
    float3 Color;                               // Light color.
    float  Range;                               // Range of the light in world units.
                                                //--------------------( 16 bytes )
    float  Intensity;                           // Intensity of the light.
    bool   Enabled;                             // Is the light on?
    float2 Padding;                             // Pad to 16 bytes.
                                                //--------------------( 16 bytes )
                                                //--------------------( 16 * 4 = 64 bytes )
};

struct SpotLight
{
    float4 PositionWS;                          // World space position.
                                                //---------------------( 16 bytes )
    float4 PositionVS;                          // View space position.
                                                //---------------------( 16 bytes )
    float4 DirectionWS;                         // Spotlight direction in world space.
                                                //---------------------( 16 bytes )
    float4 DirectionVS;                         // Spotlight direction in view space.
                                                //---------------------( 16 bytes )
    float3 Color;                               // Light color.
    float  SpotlightAngle;                      // Spotlight angle (in degrees).
                                                //---------------------( 16 bytes )
    float Range;                                // Range of the light in world units.
    float Intensity;                            // Intensity of the light.
    bool  Enabled;                              // Is the light on?
    float Padding;                              // Pad to 16 bytes.
                                                //---------------------( 16 bytes )
                                                //---------------------( 16 * 6 = 96 bytes )
};

struct DirectionalLight
{
    float4 DirectionWS;                         // World space direction.
                                                //---------------------( 16 bytes )
    float4 DirectionVS;                         // View space direction.
                                                //---------------------( 16 bytes )
    float3 Color;                               // Light color.
    float  Intensity;                           // Intensity of the light.
                                                //---------------------( 16 bytes )
    bool   Enabled;                             // Is the light on?
    float3 Padding;                             // Pad to 16 bytes.
                                                //---------------------( 16 bytes )
                                                //---------------------( 16 * 4 = 64 bytes )
};

struct CameraParameters
{
    float4x4 Projection;
    float4x4 InverseProjection;
    float2   ScreenDimensions;
};

struct UpdateLights
{
    float4x4 WorldMatrix;
    float4x4 ViewMatrix;
};

struct PerObjectData
{
    float4x4 Model;
    float4x4 View;
    float4x4 InverseView;
    float4x4 Projection;
    float4x4 ModelView;
    float4x4 ModelViewProjection;
    float4x4 InverseTransposeModel;
    float4x4 InverseTransposeModelView;
};

struct ClusterData
{
    uint3 GridDim;      // The 3D dimensions of the cluster grid.
    float ViewNear;     // The distance to the near clipping plane. (Used for computing the index in the cluster grid)
    uint2 Size;         // The size of a cluster in screen space (pixels).
    float NearK;        // ( 1 + ( 2 * tan( fov * 0.5 ) / ClusterGridDim.y ) ) // Used to compute the near plane for clusters at depth k.
    float LogGridDimY;  // 1.0f / log( 1 + ( tan( fov * 0.5 ) / ClusterGridDim.y )
};

struct DispatchParams
{
    // Number of groups dispatched. (This parameter is not available as an HLSL system value!)
    uint3   NumThreadGroups;
    // Total number of threads dispatched. (Also not available as an HLSL system value!)
    // Note: This value may be less than the actual number of threads executed 
    // if the screen size is not evenly divisible by the block size.
    uint3   NumThreads;
};

struct ReductionParams
{
    uint NumElements;   // The number of elements to be reduced.
};

struct SortParams
{
    uint NumElements;   // The total number of elements to be sorted.
    uint ChunkSize;     // The number of elements in a sorted chunk.
};

struct BVHParams
{
    uint PointLightLevels;  // Number of levels in the BVH for point lights.
    uint SpotLightLevels;   // Number of levels in the BVH for spot lights.
    uint ChildLevel;    // The level of the child nodes in the BVH that is being computed.
};

struct LightCounts
{
    uint NumPointLights;
    uint NumSpotLights;
    uint NumDirectionalLights;
};

struct Plane
{
    float3 N;   // Plane normal.
    float  d;   // Distance to origin.
};

struct Sphere
{
    float3 c;   // Center point.
    float  r;   // Radius.
};

struct Cone
{
    float3 T;   // Cone tip.
    float  h;   // Height of the cone.
    float3 d;   // Direction of the cone.
    float  r;   // bottom radius of the cone.
};

// Axis-Aligned bounding box
struct AABB
{
    float4 Min;
    float4 Max;
};

// Four planes of a view frustum (in view space).
// The planes are:
//  * Left,
//  * Right,
//  * Top,
//  * Bottom.
// The back and/or front planes can be computed from depth values in the 
// light culling compute shader.
struct Frustum
{
    Plane planes[4];   // left, right, top, bottom frustum planes.
};

// This lighting result is returned by the 
// lighting functions for each light type.
struct LightingResult
{
    float3 Diffuse;
    float3 Specular;
};


// Use this structure in the argument buffer to specify the arguments 
// for a Draw indirect command.
struct DrawIndirectArgument
{
    uint VertexCount;
    uint InstanceCount;
    uint FirstVertex;
    uint FirstInstance;
};

// Use this structure in the argument buffer to specify the arguments
// for a DrawInstanced indirect command.
struct DrawIndexedIndirectArgument
{
    uint IndexCount;
    uint InstanceCount;
    uint FirstIndex;
    int  VertexOffset;
    uint FirstInstance;
};

// Use this structure in the argument buffer to specify the arguments
// for a Dispatch indirect command.
struct DispatchIndirectArgument
{
    uint NumThreadGroupsX;
    uint NumThreadGroupsY;
    uint NumThreadGroupsZ;
};
