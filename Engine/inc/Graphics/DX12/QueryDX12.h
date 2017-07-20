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
 *  @file QueryDX12.h
 *  @date April 6, 2016
 *  @author jeremiah
 *
 *  @brief DirectX12 implementation of query.
 */

#include "../Query.h"

namespace Graphics
{
    class DeviceDX12;

    class QueryDX12 : public Query
    {
    public:
        QueryDX12( std::shared_ptr<DeviceDX12> device, QueryType queryType, uint32_t numQueries );

        /**
        * What type of query this is.
        */
        virtual QueryType GetQueryType() override;

        /**
        * Retrieve the query results from the query object.
        * The type of result returned is determined by the query type.
        * Retrieving the results could result in a GPU stall if you request
        * the current frame's result because the CPU has to wait until all of the
        * GPU commands that were enqued on the GPU before Query::End was called.
        * You should instead request the query results from 2 or 3 frames previous
        * (if you can wait a few frames for the result). If you absolutely need the current frame's result,
        * you can query the current frames result but will almost always result in a GPU stall.
        * IMPORTANT: Be sure to check the QueryResult::IsValid flag before using the result.
        */
        virtual QueryResult GetQueryResult( int64_t index, std::shared_ptr<ComputeCommandQueue> commandQueue = nullptr ) override;
        virtual std::vector<QueryResult> GetQueryResults( uint64_t startIndex, uint64_t numQueries = 1, std::shared_ptr<ComputeCommandQueue> commandQueue = nullptr ) override;


        /**
        * GPU queries can generally be multi-buffered to reduce
        * GPU stalls when getting the query result. If the implementation provides
        * multi-buffered queries this method will return the number of buffered query
        * results that are available.
        */
        virtual uint32_t GetQueryCount() const override;

        Microsoft::WRL::ComPtr<ID3D12QueryHeap> GetD3D12QueryHeap() const;
        D3D12_QUERY_TYPE GetD3D12QueryType() const;
        Microsoft::WRL::ComPtr<ID3D12Resource> GetD3D12QueryResource() const;

    private:
        std::weak_ptr<DeviceDX12> m_Device;

        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12Device;
        Microsoft::WRL::ComPtr<ID3D12QueryHeap> m_d3d12QueryHeap;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_d3d12QueryResult;
        D3D12_QUERY_TYPE m_d3d12QueryType;

        QueryType m_QueryType;
        uint32_t m_NumQueries;
        // Double queries for timer queris.
        uint32_t m_QueryMultiplyer;
    };
}