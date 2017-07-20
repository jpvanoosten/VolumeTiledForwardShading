#pragma once

#include "AbstractPass.h"

class PopProfileMarkerPass : public AbstractPass
{
public:
    PopProfileMarkerPass();
    virtual ~PopProfileMarkerPass();

    virtual void Render( Core::RenderEventArgs& e ) override;

};