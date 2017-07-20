#pragma once

/*
 *  Copyright(c) 2015 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

/**
 *  @file ConstantBuffers.h
 *  @date March 2, 2016
 *  @author Jeremiah
 *
 *  @brief Definition of all of the constant buffers used by the application.
 */

struct alignas( 16 ) PerObjectCB
{
    glm::mat4 Model;
    glm::mat4 View;
    glm::mat4 InverseView;
    glm::mat4 Projection;
    glm::mat4 ModelView;
    glm::mat4 ModelViewProjection;
    glm::mat4 InverseTransposeModel;
    glm::mat4 InverseTransposeModelView;
};

struct alignas(4) LightCountsCB
{
    uint32_t NumPointLights;
    uint32_t NumSpotLights;
    uint32_t NumDirectionalLights;
};

struct alignas( 16 ) UpdateLightsCB
{
    glm::mat4 ModelMatrix;
    glm::mat4 ViewMatrix;
};

struct alignas( 16 ) CameraParamsCB 
{
    glm::mat4 Projection;
    glm::mat4 InverseProjection;
    glm::vec2 ScreenDimensions;
};

// Constant buffer to store the number of groups executed in a dispatch.
struct alignas(4) DispatchParamsCB
{
    glm::uvec3 NumThreadGroups;
    uint32_t   Padding0; // Pad to 16 bytes.
    glm::uvec3 NumThreads;
    uint32_t   Padding1; // Pad to 16 bytes.
};

struct alignas( 16 ) Frustum
{
    glm::vec4 Planes[4];    // 64 Bytes
};

struct alignas(4) ClusterDataCB
{
    glm::uvec3 GridDim;  // The 3D dimensions of the cluster grid.
    float ViewNear;      // The distance to the near clipping plane. (Used for computing the index in the cluster grid)
    glm::uvec2 Size;     // The size of cluster in screen space.
    float NearK;         // ( 1 + ( 2 * tan( fov * 0.5 ) / ClusterGridDim.y ) ) // Used to compute the near plane for clusters at depth k.
    float LogGridDimY;   // 1.0f / log( NearK )  // Used to compute the k index of the cluster from the view depth of a pixel sample.
};

struct alignas(16) DebugClustersCB
{
    glm::mat4 View;          // Transform from previous view point to current view point.
    glm::mat4 Projection;    // Projection to clip space.
};

/**
 * Axis-aligned bounding box.
 * Primarily used for Cluster grids.
 */
struct alignas( 16 ) AABB
{
    glm::vec4 m_Min;
    glm::vec4 m_Max;
};

/**
 * Arguments passed to the sorting compute shaders.
 */
struct alignas(4) SortParams
{
    uint32_t NumElements;   // The total number of elements to be sorted.
    uint32_t ChunkSize;     // The number of elements in a sorted chunk.
};

struct alignas(4) BVHParams
{
    uint32_t PointLightLevels;  // Number of levels in the BVH for point lights.
    uint32_t SpotLightLevels;   // Number of levels in the BVH for spot lights.
    uint32_t ChildLevel;        // The level of the child nodes in the BVH that is being computed.
};
