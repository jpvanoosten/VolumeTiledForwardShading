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
 *  @file IndirectArgument.h
 *  @date July 20, 2016
 *  @author jeremiah
 *
 *  @brief Append IndirectArguments to an IndirectCommandSignature to specify the
 *  indirect commands to execute.
 *  
 *  @description One or more indirect arguments can be specified in an indirect
 *  command signature. An indirect argument can be one of the following:
 *      * Draw
 *      * DrawInstanced
 *      * Dispatch
 *  If an IndirectCommandSignature contains a Draw or DrawInstanced indirect argument
 *  then it is a graphics command signature and should not contain any Dispatch arguments.
 *  If an IndirectCommandSignature contains a Dispatch indirect argument then it is a 
 *  compute command signature and it should not contain any Draw or DrawInstanced arguments.
 *  
 *  Graphics command signatures should only be invoked on a GraphicsCommandBuffer.
 *  Compute command signatures can be invoked on a ComputeCommandBuffer or a GraphicsCommandBuffer.
 *  
 *  The format of the argument buffer that is passed to the ComputeCommandBuffer::ExecuteIndirect 
 *  function is determined by the order of the commands in the command signature.
 *  
 *  To specify the arguments in the argument buffer use the following structures
 *  depending on the argument type:
 *      * Draw:         DrawIndirectArgument structure.
 *      * DrawIndexed:  DrawIndexedIndirectArgument structure.
 *      * Dispatch:     DispatchIndirectArgument structure.
 */

#include "../EngineDefines.h"
#include "GraphicsEnums.h"

namespace Graphics
{
    // Use this structure in the argument buffer to specify the arguments 
    // for a Draw indirect command.
    struct DrawIndirectArgument
    {
        uint32_t VertexCount;
        uint32_t InstanceCount;
        uint32_t FirstVertex;
        uint32_t FirstInstance;
    };

    // Use this structure in the argument buffer to specify the arguments
    // for a DrawInstanced indirect command.
    struct DrawIndexedIndirectArgument
    {
        uint32_t IndexCount;
        uint32_t InstanceCount;
        uint32_t FirstIndex;
        int32_t  VertexOffset;
        uint32_t FirstInstance;
    };

    // Use this structure in the argument buffer to specify the arguments
    // for a Dispatch indirect command.
    struct DispatchIndirectArgument
    {
        uint32_t NumThreadGroupsX;
        uint32_t NumThreadGroupsY;
        uint32_t NumThreadGroupsZ;
    };

    class ENGINE_DLL IndirectArgument
    {
    public:
        IndirectArgument( IndirectArgumentType indirectArgumentType = IndirectArgumentType::Invalid );

        /**
         * Query the indirect argument type.
         */
        IndirectArgumentType GetArgumentType() const;

    private:
        IndirectArgumentType m_IndirectArgumentType;
    };
}