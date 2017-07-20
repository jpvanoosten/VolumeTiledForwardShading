#include <GamePCH.h>

#include <InvokeFunctionPass.h>

InvokeFunctionPass::InvokeFunctionPass( std::function<void( Core::RenderEventArgs& e )> func )
    : m_Func( func )
{}

InvokeFunctionPass::~InvokeFunctionPass()
{

}

void InvokeFunctionPass::Render( Core::RenderEventArgs& e )
{
    if ( m_Func )
    {
        m_Func( e );
    }
}
