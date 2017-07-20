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
 *  @file ComputeCommandSignature.h
 *  @date July 20, 2016
 *  @author jeremiah
 *
 *  @brief A command signature is used to specify the parameters to 
 *  an ExectueIndirect command on a ComputeCommandBuffer.
 */

#include "../EngineDefines.h"
#include "IndirectArgument.h"

namespace Graphics
{
    class ShaderSignature;

    using IndirectArguments = std::vector<IndirectArgument>;

    class ENGINE_DLL IndirectCommandSignature
    {
    public:

        /**
         * Specify the stride in bytes of the commands that are specified in the
         * argument buffer that is passed to the ComputeCommandBuffer::ExecuteIndirect function.
         */
        virtual void SetByteStride( size_t byteStride ) = 0;
        virtual size_t GetByteStride() const = 0;

        /**
         * Append a command argument to the command signature.
         */
        virtual void AppendCommandArgument( const IndirectArgument& indirectArgument ) = 0;

        /**
         * Get the command arguments that are associated with this command signature.
         */
        virtual const IndirectArguments& GetCommandArguments() const = 0;
    };
}