#include <EnginePCH.h>

#include <Graphics/ComputeCommandQueue.h>

#include <Graphics/DX12/QueryDX12.h>
#include <Graphics/DX12/DeviceDX12.h>

#include <LogManager.h>

using namespace Graphics;

D3D12_QUERY_HEAP_TYPE TranslateQueryHeapType( QueryType queryType )
{
    D3D12_QUERY_HEAP_TYPE res = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
    switch ( queryType )
    {
    case QueryType::Timer:
        res = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
        break;
    case QueryType::Occlusion:
    case QueryType::OcclusionPredicate:
        res = D3D12_QUERY_HEAP_TYPE_OCCLUSION;
        break;
    }

    return res;
}

D3D12_QUERY_TYPE TranslateQueryType( QueryType queryType )
{
    D3D12_QUERY_TYPE res = D3D12_QUERY_TYPE_TIMESTAMP;
    switch ( queryType )
    {
    case QueryType::Timer:
        res = D3D12_QUERY_TYPE_TIMESTAMP;
        break;
    case QueryType::Occlusion:
        res = D3D12_QUERY_TYPE_OCCLUSION;
        break;
    case QueryType::OcclusionPredicate:
        res = D3D12_QUERY_TYPE_BINARY_OCCLUSION;
        break;
    }

    return res;
}

QueryDX12::QueryDX12( std::shared_ptr<DeviceDX12> device, QueryType queryType, uint32_t numQueries )
    : m_Device( device )
    , m_d3d12Device( device->GetD3D12Device() )
    , m_d3d12QueryType( TranslateQueryType(queryType) )
    , m_QueryType( queryType )
    , m_NumQueries( numQueries )
    , m_QueryMultiplyer( queryType == QueryType::Timer ? 2 : 1 )
{
    uint64_t bufferSize = m_NumQueries * m_QueryMultiplyer * sizeof(uint64_t);
    // Create a resource to read back the query results.
    if ( FAILED( m_d3d12Device->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_READBACK ), D3D12_HEAP_FLAG_NONE,
                                                         &CD3DX12_RESOURCE_DESC::Buffer( bufferSize ),
                                                         D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS( &m_d3d12QueryResult ) ) ) )
    {
        LOG_ERROR( "Failed to create readback buffer for query results." );
    }
    else
    {
        D3D12_QUERY_HEAP_DESC queryHeapDesc = {};
        queryHeapDesc.Count = m_NumQueries * m_QueryMultiplyer; // Multiply by 2 for timestamp queries that need a begin and end timestamp.
        queryHeapDesc.NodeMask = 1;
        queryHeapDesc.Type = TranslateQueryHeapType( m_QueryType );

        if ( FAILED( m_d3d12Device->CreateQueryHeap( &queryHeapDesc, IID_PPV_ARGS( &m_d3d12QueryHeap ) ) ) )
        {
            LOG_ERROR( "Failed to create query heap." );
        }
    }
}

QueryType QueryDX12::GetQueryType()
{
    return m_QueryType;
}

QueryResult QueryDX12::GetQueryResult( int64_t index, std::shared_ptr<ComputeCommandQueue> commandQueue )
{
    return GetQueryResults( index, 1, commandQueue )[0];
}

std::vector<QueryResult> QueryDX12::GetQueryResults( uint64_t startIndex, uint64_t numQueries, std::shared_ptr<ComputeCommandQueue> commandQueue )
{
    assert( startIndex + numQueries < m_NumQueries );

    std::vector<QueryResult> results( numQueries );

    if ( m_d3d12QueryResult )
    {
        void* pData = nullptr;
        D3D12_RANGE readRange;
        readRange.Begin = ( startIndex * m_QueryMultiplyer ) * sizeof(uint64_t);
        readRange.End = ( startIndex + numQueries ) * m_QueryMultiplyer * sizeof(uint64_t);

        if ( FAILED( m_d3d12QueryResult->Map( 0, &readRange, &pData ) ) )
        {
            LOG_ERROR( "Failed to read query results from readback buffer." );
        }
        else
        {
            if ( m_QueryType == QueryType::Timer )
            {
                uint64_t* pQueryResult = reinterpret_cast<uint64_t*>( pData );

                double gpuFrequency = static_cast<double>( commandQueue->GetGPUFrequency() );
                for ( uint64_t i = 0; i < numQueries; ++i )
                {
                    uint64_t t0 = pQueryResult[i * m_QueryMultiplyer];
                    uint64_t t1 = pQueryResult[i * m_QueryMultiplyer + 1];

                    double elapsedTime = ( t1 - t0 ) / gpuFrequency;

                    results[i].ElapsedTime = elapsedTime;
                    results[i].IsValid = ( elapsedTime > 0.0 );
                }
            }
            else
            {
                uint64_t* pQueryResult = reinterpret_cast<uint64_t*>( pData );

                for ( uint64_t i = 0; i < numQueries; ++i )
                {
                    uint64_t numSamples = pQueryResult[i];
                    results[i].NumSamples = numSamples;
                    results[i].IsValid = true;
                }
            }

            D3D12_RANGE writeRange = {}; // Nothing was written.
            m_d3d12QueryResult->Unmap( 0, &writeRange );
        }
    }

    return std::move( results );
}

uint32_t QueryDX12::GetQueryCount() const
{
    return m_NumQueries;
}


Microsoft::WRL::ComPtr<ID3D12QueryHeap> QueryDX12::GetD3D12QueryHeap() const
{
    return m_d3d12QueryHeap;
}

D3D12_QUERY_TYPE QueryDX12::GetD3D12QueryType() const
{
    return m_d3d12QueryType;
}

Microsoft::WRL::ComPtr<ID3D12Resource> QueryDX12::GetD3D12QueryResource() const
{
    return m_d3d12QueryResult;
}
