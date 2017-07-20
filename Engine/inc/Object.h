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
 *  @file Object.h
 *  @date December 18, 2015
 *  @author Jeremiah
 *
 *  @brief Object base class for all classes.
 */

#include "NonCopyable.h"

#include <boost/uuid/uuid.hpp>

namespace Core
{
    class ENGINE_DLL Object : public NonCopyable
    {
    public:

        // Specify the name of an object.
        // Primarily used for debugging.
        virtual void SetName( const std::wstring& name );
        virtual const std::wstring& GetName() const;

        // Check to see if this object is the same as another.
        virtual bool operator==( const Object& rhs ) const;

        // Retrieve the UUID associated to this object.
        boost::uuids::uuid GetUUID() const;

    protected:
        // Use this enum type in the Object constructor to avoid creation of the
        // UUID.
        enum NoUUID
        {
            ctor
        };

        // Objects should not be created or destroyed unless explicitly stated
        // by overriding these methods
        Object();

        // The construction of UUIDs is expensive.
        // When creating temporary object, we can use this constructor.
        // to avoid the construction of the UUID.
        Object( NoUUID );


        virtual ~Object();

    private:
        std::wstring m_Name;
        boost::uuids::uuid m_UUID;
    };
}