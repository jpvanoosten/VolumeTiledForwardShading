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
 *  @file Fence.h
 *  @date December 29, 2015
 *  @author Jeremiah
 *
 *  @brief GPU fence object. Can be used for CPU and GPU synchronization.
 *  A fence can be inserted into a command queue during command buffer submission
 *  and the fence status can be checked on the CPU.
 *  Fences can be waited on the CPU side using Graphics::Device::Wait function.
 *  
 */

#include "../EngineDefines.h"

namespace Graphics
{
    enum class FenceStatus
    {
        Unknown,    // Fences start in this state when they are created.
        Ready,      // The fence has been reached.
        NotReady    // The fence has not been reached.
    };

    class ENGINE_DLL Fence
    {
    public:
        /**
         * Check the fence status.
         */
        virtual FenceStatus GetStatus() const = 0;

        /**
        * Wait a specific period of time for the fence to complete.
        * @param duration Time to wait in milliseconds. Default value is "infinite" wait.
        */
        virtual void WaitFor( std::chrono::milliseconds duration = std::chrono::milliseconds::max() ) = 0;

    };
}
