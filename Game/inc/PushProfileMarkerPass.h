#pragma once

#include "AbstractPass.h"

class PushProfileMarkerPass : public AbstractPass
{
public:
    PushProfileMarkerPass( const std::wstring& profileMarker );
    virtual ~PushProfileMarkerPass();

    virtual void Render( Core::RenderEventArgs& e ) override;

private:

    std::wstring m_ProfilerMarker;
};