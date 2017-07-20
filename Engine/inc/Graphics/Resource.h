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
 *  @file Resource.h
 *  @date Jan 2016/01/11, 2016
 *  @author Jeremiah van Oosten
 *
 *  @brief A resource is the base class for any graphics related object (buffer, or texture).
 */

#include "../EngineDefines.h"
#include "GraphicsEnums.h"

namespace Graphics
{
    class CopyCommandBuffer;

    class ENGINE_DLL Resource
    {
    public:
        virtual ~Resource() {}

        /**
         * Set the name of the internal resource.
         * Primarily used for debugging.
         */
        virtual void SetName( const std::wstring& name ) = 0;

        /**
         * Get the current state of the resource.
         */
        virtual ResourceState GetResourceState() const = 0;
    };
}
