#include <EnginePCH.h>

#include <Graphics/IndirectArgument.h>

using namespace Graphics;

IndirectArgument::IndirectArgument( IndirectArgumentType indirectArgumentType )
    : m_IndirectArgumentType( indirectArgumentType )
{}

IndirectArgumentType IndirectArgument::GetArgumentType() const
{
    return m_IndirectArgumentType;
}
