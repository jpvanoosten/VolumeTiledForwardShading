#pragma once

#include "AbstractPass.h"

// A pass that will invoke a function object.
// This is required when it would be overkill to fully implement
// a render pass for a small amount of code that needs to be executed.
// For example, if shader variables need to be updated between draw calls.

class InvokeFunctionPass : public AbstractPass
{
public:
    InvokeFunctionPass( std::function<void( Core::RenderEventArgs& e )> func );
    virtual ~InvokeFunctionPass();

    virtual void Render( Core::RenderEventArgs& e ) override;

private:
    std::function<void( Core::RenderEventArgs& e )> m_Func;
};