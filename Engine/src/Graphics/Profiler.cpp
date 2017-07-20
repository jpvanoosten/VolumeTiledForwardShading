#include <EnginePCH.h>

#include <Graphics/Profiler.h>
#include <Graphics/Device.h>
#include <Graphics/Query.h>
#include <Graphics/ComputeCommandQueue.h>
#include <Graphics/ComputeCommandBuffer.h>

#include <Common.h>
#include <ProfilerVisitor.h>

using namespace Graphics;
using namespace std::chrono;

static std::shared_ptr<Profiler> g_Profiler = nullptr;

ProfileNode::ProfileNode( const std::wstring& name, uint32_t queryIndex, std::shared_ptr<Query> gpuQuery, std::shared_ptr<ProfileNode> parent )
    : NameWStr( name )
    , Name( Core::ConvertString(name) )
    , ID( parent ? parent->ID : 0 )
    , IsSelected( false )
    , Parent( parent )
    , QueryIndex( queryIndex )
    , GpuQuery( gpuQuery )
    , HasGpuQueryResults( false )
    , CpuFrame( 0 )
    , GpuFrame( 0 )
{
    boost::hash_combine( ID, name );
}

std::shared_ptr<ProfileNode> ProfileNode::GetChild( const std::wstring& name )
{
    auto iter = ChildrenMap.find( name );
    if ( iter == ChildrenMap.end() )
    {
        std::shared_ptr<Query> gpuQuery;
        uint32_t queryIndex;
        std::tie( gpuQuery, queryIndex ) = g_Profiler->AddQuery();
        std::shared_ptr<ProfileNode> child = std::make_shared<ProfileNode>( name, queryIndex, gpuQuery, shared_from_this() );
        Children.emplace_back( child );
        iter = ChildrenMap.emplace( name, child ).first;
    }

    return iter->second;
}

void ProfileNode::Start( std::shared_ptr<ComputeCommandBuffer> commandBuffer )
{
    StartTime = std::chrono::high_resolution_clock::now();
    if ( commandBuffer )
    {
        commandBuffer->BeginQuery( GpuQuery, QueryIndex );
        commandBuffer->BeginProfilingEvent( NameWStr );
    }
}

void ProfileNode::Stop( std::shared_ptr<ComputeCommandBuffer> commandBuffer, uint64_t frame )
{
    EndTime = std::chrono::high_resolution_clock::now();
    CpuFrame = frame;

    CpuStats.Sample( std::chrono::duration<double>( EndTime - StartTime ).count() );

    if ( commandBuffer )
    {
        commandBuffer->EndProfilingEvent( NameWStr );
        commandBuffer->EndQuery( GpuQuery, QueryIndex );
        HasGpuQueryResults = true;
        GpuFrame = frame;
    }
}

void ProfileNode::UpdateQueryResult( const std::vector<QueryResult>& results )
{
    if ( HasGpuQueryResults )
    {
        GpuStats.Sample( results[QueryIndex].ElapsedTime );
        HasGpuQueryResults = false;
    }

    for ( auto child : Children )
    {
        child->UpdateQueryResult( results );
    }
}

void ProfileNode::DeleteChildren()
{
    Children.clear();
    ChildrenMap.clear();
}


void ProfileNode::Accept( Core::ProfilerVisitor& visitor )
{
    visitor.Visit( *this );
    for ( auto child : Children )
    {
        child->Accept( visitor );
    }
}


Profiler::Profiler( std::shared_ptr<Device> device, uint32_t numProfilingMarkers )
    : m_Device( device )
    , m_NumQueryTimers( 0 )
    , m_Paused( false )
    , m_CurrentFrame( 0 )
{ 
    m_GpuTimestampQuery = device->CreateQuery( QueryType::Timer, numProfilingMarkers );
    m_RootNode = std::make_shared<ProfileNode>( L"Root", m_NumQueryTimers++, m_GpuTimestampQuery, nullptr );
    m_CurrentNode = m_RootNode;
}

Profiler::~Profiler()
{}

Profiler& Profiler::Get()
{
    assert( g_Profiler );
    return *g_Profiler;
}

void Profiler::Init( std::shared_ptr<Device> device, uint32_t numProfilingMarkers )
{
    g_Profiler = std::make_shared<Profiler>( device, numProfilingMarkers );
}

void Profiler::Shutdown()
{
    g_Profiler.reset();
}

void Profiler::SetPaused( bool paused )
{
    m_Paused = paused;
}

bool Profiler::IsPaused() const
{
    return m_Paused;
}

void Profiler::SetCurrentFrame( uint64_t frame )
{
    m_CurrentFrame = frame;
}

uint64_t Profiler::GetCurrentFrame() const
{
    return m_CurrentFrame;
}

void Profiler::PushProfilingMarker( const std::wstring& name, std::shared_ptr<ComputeCommandBuffer> commandBuffer )
{
    if ( !m_Paused )
    {
        scoped_lock lock( m_Mutex );

        m_CurrentNode = m_CurrentNode->GetChild( name );
        m_CurrentNode->Start( commandBuffer );
    }
}

void Profiler::PopProfilingMarker( std::shared_ptr<ComputeCommandBuffer> commandBuffer )
{
    if ( !m_Paused )
    {
        scoped_lock lock( m_Mutex );

        m_CurrentNode->Stop( commandBuffer, m_CurrentFrame );
        m_CurrentNode = m_CurrentNode->Parent.lock();

        m_CurrentNode = m_CurrentNode ? m_CurrentNode : m_RootNode;

        assert( m_CurrentNode );
    }
}

void Profiler::UpdateQueryResults( std::shared_ptr<ComputeCommandQueue> commandQueue )
{
    m_QueryResults = std::move( m_GpuTimestampQuery->GetQueryResults( 0, std::min<uint64_t>( m_NumQueryTimers, m_GpuTimestampQuery->GetQueryCount() ), commandQueue ) );
    m_RootNode->UpdateQueryResult( m_QueryResults );
}

std::shared_ptr<ProfileNode> Profiler::GetRootProfileMarker()
{
    return m_RootNode;
}

void Profiler::ClearAllProfilingData()
{
    scoped_lock lock(m_Mutex);
    m_RootNode->DeleteChildren();
    m_CurrentNode = m_RootNode;
}

void Profiler::Accept( Core::ProfilerVisitor& visitor )
{
    visitor.Visit( *this );
    m_RootNode->Accept( visitor );
}

std::tuple< std::shared_ptr<Query>, uint32_t> Profiler::AddQuery()
{
    uint32_t queryIndex = m_NumQueryTimers++;
    assert( queryIndex < m_GpuTimestampQuery->GetQueryCount() );

    return std::make_tuple( m_GpuTimestampQuery, queryIndex );
}
