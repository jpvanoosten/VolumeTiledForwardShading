#pragma once

#include "../EngineDefines.h"

namespace Graphics
{
    struct ENGINE_DLL Viewport
    {
        explicit Viewport( float x = 0.0f, float y = 0.0f, float width = 0.0f, float height = 0.0f, float minDepth = 0.0f, float maxDepth = 1.0f )
            : X( x )
            , Y( y )
            , Width( width )
            , Height( height )
            , MinDepth( minDepth )
            , MaxDepth( maxDepth )
        {}

        float X;
        float Y;
        float Width;
        float Height;
        float MinDepth;
        float MaxDepth;
    };
}
