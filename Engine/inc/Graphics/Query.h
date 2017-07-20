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
 *  @file Query.h
 *  @date April 6, 2016
 *  @author jeremiah
 *
 *  @brief Interface for GPU query objects.
 */

#include "../EngineDefines.h"
#include "GraphicsEnums.h"

namespace Graphics
{
    class ComputeCommandQueue;

    struct ENGINE_DLL QueryResult
    {
        union
        {
            double   ElapsedTime;                   // Valid for QueryType::Timer queries. Returns the elapsed time in seconds between Query::Begin and Query::End.
            uint64_t NumSamples;                    // Valid for QueryType::Occlusion. Returns the number of samples written by the fragment shader between Query::Begin and Query::End.
            bool     AnySamples;                    // Valid for QueryType::OcclusionPredicate. Returns true if any samples were written by the fragment shader between Query::Begin and Query::End.
        };
        // Are the results of the query valid?
        // You should check this before using the value.
        bool IsValid;
    };

    class ENGINE_DLL Query
    {
    public:

        /**
         * What type of query this is.
         */
        virtual QueryType GetQueryType() = 0;

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
        virtual QueryResult GetQueryResult( int64_t index, std::shared_ptr<ComputeCommandQueue> commandQueue = nullptr ) = 0;
        virtual std::vector<QueryResult> GetQueryResults( uint64_t startIndex, uint64_t numQueries = 1, std::shared_ptr<ComputeCommandQueue> commandQueue = nullptr ) = 0;

        /**
        * GPU queries can generally be multi-buffered to reduce
        * GPU stalls when getting the query result. If the implementation provides
        * multi-buffered queries this method will return the number of buffered query
        * results that are available.
        */
        virtual uint32_t GetQueryCount() const = 0;
    };
}