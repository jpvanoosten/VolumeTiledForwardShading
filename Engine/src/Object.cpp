#include <EnginePCH.h>

#include <Object.h>

using namespace Core;

Object::Object()
    : m_UUID( boost::uuids::random_generator()( ) )
{}

Object::Object( NoUUID )
{}

Object::~Object()
{}

// Check to see if this object is the same as another.
bool Object::operator==( const Object& rhs ) const
{
    return m_UUID == rhs.m_UUID;
}

void Object::SetName( const std::wstring& name )
{
    m_Name = name;
}

const std::wstring& Object::GetName() const
{
    return m_Name;
}

boost::uuids::uuid Object::GetUUID() const
{
    return m_UUID;
}
