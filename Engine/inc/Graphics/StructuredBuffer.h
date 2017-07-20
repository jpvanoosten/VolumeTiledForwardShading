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
 *  @file StructuredBuffer.h
 *  @date April 29, 2016
 *  @author jeremiah
 *
 *  @brief Structured buffer.
 */

#include "../EngineDefines.h"
#include "Buffer.h"

namespace Graphics
{
    class ByteAddressBuffer;

    class ENGINE_DLL StructuredBuffer : public Buffer
    {
    public:
        /**
         * Get the number of elements contained in this buffer.
         */
        virtual size_t GetNumElements() const = 0;

        /**
         * Get the size in bytes of each element in this buffer.
         */
        virtual size_t GetElementSize() const = 0;

        /**
         * Get the buffer that stores the internal counter of the structured buffer.
         */
        virtual std::shared_ptr<ByteAddressBuffer> GetCounterBuffer() const = 0;
    };
}