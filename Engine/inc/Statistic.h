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
 *  @file Statistic.h
 *  @date April 13, 2016
 *  @author jeremiah
 *
 *  @brief Used for recording numerical statistics.
 */

#include "EngineDefines.h"

namespace Core
{
    template<typename T>
    class Statistic
    {
    public:

        using value_type = T;

        Statistic( uint32_t maxSamples = 1024 )
            : m_MaxSamples( maxSamples )
        {
            m_Samples.resize( m_MaxSamples );
            Reset();
        }

        // Reset samples.
        void Reset()
        {
            m_NumSamples = 0;
            m_Total = 0;
            m_Average = 0;
            m_Min = std::numeric_limits<T>::max();
            m_Max = std::numeric_limits<T>::min();
        }

        void Sample( T value )
        {
            m_Samples[m_NumSamples++ % m_MaxSamples] = value;

            m_Total += value;
            m_Average = m_Total / m_NumSamples;
            m_Max = std::max( m_Max, value );
            m_Min = std::min( m_Min, value );
        }

        uint32_t GetNumSamples() const 
        { 
            return m_NumSamples;
        }

        T GetAverage() const
        {
            return m_Average;
        }

        T GetMax() const
        {
            return m_Max;
        }

        T GetMin() const
        {
            return m_Min;
        }

        T GetLast() const
        {
            return m_Samples[( m_NumSamples + m_MaxSamples - 1 ) % m_MaxSamples];
        }

        /**
         * Get the variance of the samples.
         * Since the variance can only be computed once the average
         * of all of the samples is known, we must compute it each time
         * the value is requested. Use this function sparingly.
         */
        T GetVariance() const
        {
            T variance = 0.0;

            uint32_t numSamples = std::min( m_NumSamples, m_MaxSamples );
            for ( uint32_t i = 0; i < numSamples; ++i )
            {
                variance += std::pow( m_Samples[i] - m_Average, 2 );
            }

            return variance / static_cast<T>( numSamples );
        }

        /**
         * Get the standard deviation of the samples.
         * This requires the variance to be computed.
         */
        double GetStandardDeviation() const
        {
            return std::sqrt( GetVariance() );
        }

        /**
         * Get all the samples recorded.
         * @return A tuple where the first value is a pointer to the samples array
         * and the second value is the number of recorded samples and the third value
         * is the offset into the array to access the first sample in the ring buffer.
         */
        std::tuple<const T*, uint32_t, uint32_t> GetSamples() const
        {
            return std::make_tuple( m_Samples.data(), m_MaxSamples, m_NumSamples % m_MaxSamples );
        }

    private:
        // Maximum number of samples to record.
        uint32_t m_MaxSamples;

        // Number of recorded samples.
        uint32_t m_NumSamples;

        T m_Total;
        T m_Average;
        T m_Min;
        T m_Max;

        std::vector<T> m_Samples;
    };
}