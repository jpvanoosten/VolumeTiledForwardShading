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
 *  @file ReadbackBuffer.h
 *  @date May 13, 2016
 *  @author Jeremiah
 *
 *  @brief A ReadbackBuffer is used to read back the contents of a GPU buffer.
 *  Use CopyCommandBuffer::CopyResource to copy the contents of a default GPU buffer into a readback buffer.
 */

#include "../EngineDefines.h"
#include "Buffer.h"

namespace Graphics
{
    class ENGINE_DLL ReadbackBuffer : public Buffer
    {
    public:
        /**
         * To read the content of a GPU buffer, you must first copy the default GPU buffer
         * into a readback buffer using CopyCommandBuffer::CopyResource. After copying the source 
         * buffer into a readback buffer, the contents of the readback buffer can be 
         * read using GetData.
         * @param dstBuffer A CPU buffer that is large enough to store the contents of the 
         * readback buffer. It is up to the application to ensure there is enough space
         * to copy the contents of the readback buffer.
         * @param offset The offset in bytes to start reading from.
         * @param size The number of bytes to read. By default, the entire buffer is read.
         */
        virtual void GetData( void* dstBuffer, size_t offset = 0, size_t size = std::numeric_limits<size_t>::max() ) const = 0;

    };
}