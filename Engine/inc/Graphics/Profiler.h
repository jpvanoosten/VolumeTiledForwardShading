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
 *  @file GpuProfiler.h
 *  @date April 12, 2016
 *  @author jeremiah
 *
 *  @brief GPU and CPU performance profiling.
 */

#include "../EngineDefines.h"
#include "../Statistic.h"
#include "Query.h"

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>

#include <Graphics/GraphicsCommandBuffer.h> // Needed for static_cast

namespace Core
{
    class ProfilerVisitor;
}

namespace Graphics
{
    class Device;
    class ComputeCommandQueue;
    class Query;

    struct ENGINE_DLL ProfileNode : public std::enable_shared_from_this<ProfileNode>
    {
        ProfileNode( const std::wstring& name, uint32_t queryIndex, std::shared_ptr<Query> gpuQuery, std::shared_ptr<ProfileNode> parent );

        std::string Name;
        std::wstring NameWStr;
        size_t ID;
        bool IsSelected;
        std::weak_ptr<ProfileNode> Parent;
        std::vector<std::shared_ptr<ProfileNode> > Children;                    // Warning: Not thread-safe.
        std::unordered_map< std::wstring, std::shared_ptr<ProfileNode> > ChildrenMap;   // Warning: Not thread-safe.

        std::chrono::high_resolution_clock::time_point StartTime;
        std::chrono::high_resolution_clock::time_point EndTime;

        // Index into the GPU query heap.
        uint32_t QueryIndex;
        std::shared_ptr<Query> GpuQuery;
        bool HasGpuQueryResults;

        // The last frame the CPU stats were modified.
        uint64_t CpuFrame;
        // The last frame the GPU stats were modified.
        uint64_t GpuFrame;

        Core::Statistic<double> CpuStats;
        Core::Statistic<double> GpuStats;

        std::shared_ptr<ProfileNode> GetChild( const std::wstring& name );
        void Start( std::shared_ptr<ComputeCommandBuffer> commandBuffer );
        void Stop( std::shared_ptr<ComputeCommandBuffer> commandBuffer, uint64_t frame );
        
        void UpdateQueryResult( const std::vector<QueryResult>& results );

        void DeleteChildren();

        void Accept( Core::ProfilerVisitor& visitor );
    };

    class ENGINE_DLL Profiler
    {
    public:
        Profiler( std::shared_ptr<Device> device, uint32_t numProfilingMarkers );
        ~Profiler();

        // Get the instance of the profiler.
        // Must first call Profiler::Init.
        static Profiler& Get();

        // Initialize the static instance of the profiler.
        // Must be called before Profiler::Get.
        static void Init( std::shared_ptr<Device> device, uint32_t numProfilingMarkers = 1024 );
        static void Shutdown();

        void SetPaused( bool paused );
        bool IsPaused() const;
        void SetCurrentFrame( uint64_t frame );
        uint64_t GetCurrentFrame() const;

        void PushProfilingMarker( const std::wstring& name, std::shared_ptr<ComputeCommandBuffer> commandBuffer = nullptr );
        void PopProfilingMarker( std::shared_ptr<ComputeCommandBuffer> commandBuffer = nullptr );

        // This should be called after the command buffer has been submitted to the command queue.
        void UpdateQueryResults( std::shared_ptr<ComputeCommandQueue> commandQueue );

        // Clear all profiling data
        void ClearAllProfilingData();

        std::shared_ptr<ProfileNode> GetRootProfileMarker();

        void Accept( Core::ProfilerVisitor& visitor );

    protected:

    private:
        friend struct ProfileNode;

        std::tuple< std::shared_ptr<Query>, uint32_t> AddQuery();

        std::weak_ptr<Device> m_Device;
        std::shared_ptr<ProfileNode> m_RootNode;
        std::shared_ptr<ProfileNode> m_CurrentNode;
        std::shared_ptr<Query> m_GpuTimestampQuery;
        std::atomic_uint32_t m_NumQueryTimers;
        std::vector<QueryResult> m_QueryResults;
        uint64_t m_CurrentFrame;
        std::mutex m_Mutex;

        bool m_Paused;
    };

    class ENGINE_DLL ScopedProfileMarker
    {
    public:
        ScopedProfileMarker( const std::wstring& name, std::shared_ptr<ComputeCommandBuffer> commandBuffer = nullptr )
            : m_CommandBuffer( commandBuffer )
        {
            Profiler::Get().PushProfilingMarker( name, m_CommandBuffer.lock() );
        }

        ~ScopedProfileMarker()
        {
            Profiler::Get().PopProfilingMarker( m_CommandBuffer.lock() );
        }
    private:
        std::weak_ptr<ComputeCommandBuffer> m_CommandBuffer;
    };
}

#if defined(PROFILE)
#define CPU_MARKER(name) Graphics::ScopedProfileMarker profilerMarker##__LINE__( _W2(name) )
#define GPU_MARKER(name,commandBuffer) Graphics::ScopedProfileMarker profilerMarker##__LINE__( _W2(name), std::static_pointer_cast<Graphics::ComputeCommandBuffer>( commandBuffer ) )
#else
#define CPU_MARKER(name)
#define GPU_MARKER(name,commandBuffer)
#endif