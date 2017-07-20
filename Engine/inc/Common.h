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
 *  @file Common.h
 *  @date January 13, 2016
 *  @author Jeremiah van Oosten
 *
 *  @brief Commonly used functions.
 */

#include <string>
#include <locale>
#include <codecvt>

#include "EngineDefines.h"
#include "bitmask_operators.hpp"

namespace Math
{
    constexpr float PI = 3.1415926535897932384626433832795f;
    constexpr float _2PI = 2.0f * PI;
    // Convert radians to degrees.
    constexpr float Degrees( const float radians )
    {
        return radians * ( 180.0f / PI );
    }

    // Convert degrees to radians.
    constexpr float Radians( const float degrees )
    {
        return degrees * ( PI / 180.0f );
    }

    template<typename T>
    inline T Deadzone( T val, T deadzone )
    {
        if ( std::abs( val ) < deadzone )
        {
            return T( 0 );
        }

        return val;
    }

    // Normalize a value in the range [min - max]
    template<typename T, typename U>
    inline T NormalizeRange( U x, U min, U max )
    {
        return T( x - min ) / T( max - min );
    }

    // Shift and bias a value into another range.
    template<typename T, typename U>
    inline T ShiftBias( U x, U shift, U bias )
    {
        return T( x * bias ) + T( shift );
    }

    /***************************************************************************
     * These functions were taken from the MiniEngine.
     * Source code available here:
     * https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Math/Common.h
     * Retrieved: January 13, 2016
     **************************************************************************/
    template <typename T>
    inline T AlignUpWithMask( T value, size_t mask )
    {
        return (T)( ( (size_t)value + mask ) & ~mask );
    }

    template <typename T>
    inline T AlignDownWithMask( T value, size_t mask )
    {
        return (T)( (size_t)value & ~mask );
    }

    template <typename T>
    inline T AlignUp( T value, size_t alignment )
    {
        return AlignUpWithMask( value, alignment - 1 );
    }

    template <typename T>
    inline T AlignDown( T value, size_t alignment )
    {
        return AlignDownWithMask( value, alignment - 1 );
    }

    template <typename T>
    inline bool IsAligned( T value, size_t alignment )
    {
        return 0 == ( (size_t)value & ( alignment - 1 ) );
    }

    template <typename T>
    inline T DivideByMultiple( T value, size_t alignment )
    {
        return (T)( ( value + alignment - 1 ) / alignment );
    }
    /***************************************************************************/

    /**
     * Round up to the next highest power of 2.
     * @source: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
     * @retrieved: January 16, 2016
     */
    inline uint32_t NextHighestPow2( uint32_t v )
    {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;

        return v;
    }

    /**
    * Round up to the next highest power of 2.
    * @source: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    * @retrieved: January 16, 2016
    */
    inline uint64_t NextHighestPow2( uint64_t v )
    {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v |= v >> 32;
        v++;

        return v;
    }
}

namespace Core
{
    // Convert a multi-byte character string (UTF-8) to a wide (UTF-16) encoded string.
    inline std::wstring ConvertString( const std::string& string )
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes( string );
    }

    // Converts a wide (UTF-16) encoded string into a multi-byte (UTF-8) character string.
    inline std::string ConvertString( const std::wstring& wstring )
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.to_bytes( wstring );
    }

    // Gets a string resource from the module's resources.
    ENGINE_DLL std::string GetStringResource( int ID, const std::string& type );

}

inline std::wstring to_wstring( const std::string& s )
{
    return Core::ConvertString( s );
}

inline const std::wstring& to_wstring( const std::wstring& s )
{
    return s;
}

inline std::wstring to_wstring( char c )
{
    return to_wstring( std::string( 1, c ) );
}
