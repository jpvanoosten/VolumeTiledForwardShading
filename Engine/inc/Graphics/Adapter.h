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
 *  @file GraphicsAdapter.h
 *  @date December 22, 2015
 *  @author Jeremiah
 *
 *  @brief The GraphicsAdapter represents a physical GPU in the system.
 *  Applications can make use of all adapters available while rendering to 
 *  maximize performance.
 */

#include "../EngineDefines.h"

namespace Graphics
{
    class Display;

    using GraphicsDisplayList = std::vector<std::shared_ptr<Display>>;

    enum class AdapterType
    {
        Hardware,
        Software,
    };

    /**
     * The application can query the current video memory usage of the adapter.
     * The local memory segment is high-speed memory that the adapter has access to.
     * The non-local memory segment is low-speed memory usually located in shared system memory.
     */
    enum class VideoMemorySegment
    {
        Local,
        NonLocal,
    };

    /**
     * The video memory usage that is returned from Adapter::QueryVideoMemoryUsage.
     */
    struct VideoMemoryUsage
    {
        // The amount of video memory (in bytes) that is currently budgeted to the adapter from the OS.
        uint64_t Budget;
        // The adapters current video memory usage (in bytes). If CurrentUsage is greater than Budget
        // the application may incur stuttering while the OS tries to provide sufficient memory to other running applications.
        uint64_t CurrentUsage;
        // The amount of video memory (in bytes) that is reserved for the application.
        // To reserve video memory, use Adapter::SetVideoMemoryReservation.
        uint64_t AvailableForReservation;
        uint64_t CurrentReservation;
    };

    class ENGINE_DLL Adapter
    {
    public:
        /**
         * Get the name of the physical adapter.
         * In DirectX, this is the description of the adapter.
         */
        virtual const std::wstring& GetDescription() const = 0;

        /**
         * Get the adapter type.
         * @see AdapterType
         */
        virtual AdapterType GetType() const = 0;

        /**
         * Get the vendor ID of the GPU.
         */
        virtual uint32_t GetVendorID() const = 0;

        /**
         * Get the device ID of the GPU.
         */
        virtual uint32_t GetDeviceID() const = 0;

        /**
         * Get the amount of dedicated video memory in bytes.
         */
        virtual uint64_t GetDedicatedVideoMemory() const = 0;

        /**
         * Get the amount of dedicated system memory in bytes.
         */
        virtual uint64_t GetDedicatedSystemMemory() const = 0;

        /**
         * Get the amount of shared system memory in bytes.
         */
        virtual uint64_t GetSharedSystemMemory() const = 0;

        /**
         * Query the video memory usage for this adapter.
         * @param videoMemorySegment Which segment of video memory to query.
         */
        virtual VideoMemoryUsage QueryVideoMemoryUsage( VideoMemorySegment videoMemorySegment ) const = 0;

        /**
         * Specify to the OS the minimum amount of video memory (in bytes) that this application must
         * have reserved. Use Adapter::QueryVideoMemoryUsage to determine amount of video 
         * memory that is available for reservation.
         */
        virtual void SetVideoMemoryReservation( VideoMemorySegment videoMemorySegment, uint64_t bytes ) = 0;

        /**
         * Get a list of the displays connected to the physical adapter.
         * This could be empty if no display is connected to the adapter or it
         * is a headless GPU.
         */
        virtual const GraphicsDisplayList& GetDisplays() const = 0;
    };
}
