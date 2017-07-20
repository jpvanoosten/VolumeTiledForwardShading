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
 *  @file ComputeCommandBuffer.h
 *  @date January 2, 2016
 *  @author Jeremiah
 *
 *  @brief A Compute Command buffer is used to record either copy or compute operations.
 */

#include "CopyCommandBuffer.h"
#include "ClearColor.h"

namespace Graphics
{
    class Resource;
    class Buffer;
    class ShaderSignature;
    class ShaderParameter;
    class ComputePipelineState;
    class Query;
    class IndirectCommandSignature;

    using ShaderArguments = std::vector< std::shared_ptr<Resource> >;

    class ENGINE_DLL ComputeCommandBuffer : public CopyCommandBuffer
    {
    public:

        /**
         * Begin a GPU query.
         */
        virtual void BeginQuery( std::shared_ptr<Query> query, uint32_t index ) = 0;

        /**
         * End a GPU query.
         */
        virtual void EndQuery( std::shared_ptr<Query> query, uint32_t index ) = 0;

        /**
         * Begin a profiling event.
         */
        virtual void BeginProfilingEvent( const std::wstring& name ) = 0;

        /**
         * End a profiling event.
         */
        virtual void EndProfilingEvent( const std::wstring& name ) = 0;

        /**
         * Bind 32-bit constants to the compute pipeline.
         */
        virtual void BindCompute32BitConstants( uint32_t slotID, uint32_t numConstants, const void* constants ) = 0;

        template<typename T>
        void BindCompute32BitConstants( uint32_t slotID, const T& constants )
        {
            static_assert( sizeof( T ) % sizeof(uint32_t) == 0, "Size of type must be a multiple of 4 bytes" );
            BindCompute32BitConstants( slotID, sizeof( T ) / sizeof( uint32_t ), &constants );
        }

        /**
         * Bind a single shader argument to the compute pipeline.
         * @param slotID The shader signature slot to bind the argument to.
         * @param argument The shader resource to bind.
         */
        virtual void BindComputeShaderArgument( uint32_t slotID, std::shared_ptr<Resource> argument ) = 0;

        /**
         * Bind a shader parameter argument to the compute pipeline.
         * @param slotID The shader signature slot to bind the parameter to.
         * @param offset The staring offset in the table range.
         * @param shaderParameter The shader parameter to bind (as an argument) to the compute pipeline.
         */
        virtual void BindComputeShaderArguments( uint32_t slotID, uint32_t offset, const ShaderArguments& shaderArguments ) = 0;

        /**
        * Bind a dynamic constant buffer data to the compute pipeline.
        * @param slotID The slot or register to bind the constant buffer to.
        * @param bufferSizeInBytes The size in bytes of the constant buffer.
        * @param bufferData A pointer to the constant buffer data.
        */
        virtual void BindComputeDynamicConstantBuffer( uint32_t slotID, size_t bufferSizeInBytes, const void* bufferData ) = 0;

        template<typename T>
        void BindComputeDynamicConstantBuffer( uint32_t slotID, const T& data )
        {
            BindComputeDynamicConstantBuffer( slotID, sizeof( T ), &data );
        }

        /**
        * Bind a dynamic structured buffer data to the compute pipeline.
        * @param slotID The slot or register to bind the structured buffer to.
        * @param numElements The number of structured elements in the structured buffer.
        * @param elementSize The size of each element in the structured buffer.
        * @param bufferData A pointer to the constant buffer data.
        */
        virtual void BindComputeDynamicStructuredBuffer( uint32_t slotID, size_t numElements, size_t elementSize, const void* bufferData ) = 0;

        template<typename T>
        void BindComputeDynamicStructuredBuffer( uint32_t slotID, const std::vector<T>& data )
        {
            BindComputeDynamicConstantBuffer( slotID, data.size(), sizeof( T ), data.data() );
        }
        
        /**
         * Bind a shader signature to the compute pipeline.
         */
        virtual void BindComputeShaderSignature( std::shared_ptr<ShaderSignature> shaderSignature ) = 0;

        /**
         * Bind the compute pipeline state object.
         */
        virtual void BindComputePipelineState( std::shared_ptr<ComputePipelineState> pipelineState ) = 0;

        /**
         * Clear the contents of a GPU resource.
         */
        virtual void ClearResourceFloat( std::shared_ptr<Resource> resource, const glm::vec4& clearValues = { 0.0f, 0.0f, 0.0f, 0.0f } ) = 0;
        virtual void ClearResourceUInt( std::shared_ptr<Resource> resource, const glm::uvec4& clearValues = { 0, 0, 0, 0 } ) = 0;

        /**
         * Generate mipmaps for the given texture.
         */
        virtual void GenerateMips( std::shared_ptr<Texture> texture ) = 0;

        /**
         * Dispatch a compute shader.
         */
        virtual void Dispatch( uint32_t numGroupsX, uint32_t numGroupsY = 1, uint32_t numGroupsZ = 1 ) = 0;
        void Dispatch( const glm::uvec3 numGroups )
        {
            Dispatch( numGroups.x, numGroups.y, numGroups.z );
        }

        /**
         * Dispatch a set of draw/dispatch commands using an indirect argument buffer.
         * @param commandSignature The command signature defines the indirect commands that should be executed.
         * @param indirectArguments A buffer that contains the arguments for the commands defined in the command signature.
         * @param byteOffset The offset within the argument buffer to the first command argument.
         * @param commandCountBuffer [optional] By default, the number of commands that will be executed is defined in the command signature.
         * If the commandCountBuffer is specified, it should contain a 32-bit integer that specifies the maximum number of
         * commands to execute. This is useful for performing object occlusion culling on the GPU.
         * @param countBufferOffset A byte offset to apply to the commandCountBuffer before reading the command count.
         * @see https://msdn.microsoft.com/en-us/library/windows/desktop/dn903884(v=vs.85).aspx
         * @see https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkCmdDispatchIndirect.html
         * @see https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkCmdDrawIndirect.html
         * @see https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkCmdDrawIndexedIndirect.html
         */
        virtual void ExecuteIndirect( std::shared_ptr<IndirectCommandSignature> commandSignature, 
                                      std::shared_ptr<Buffer> indirectArguments, size_t byteOffset = 0,
                                      std::shared_ptr<Buffer> commandCountBuffer = nullptr, size_t countBufferOffset = 0 ) = 0;

    };
}