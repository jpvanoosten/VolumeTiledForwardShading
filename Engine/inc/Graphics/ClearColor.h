#pragma once

#include <cstdint>
#include "GraphicsEnums.h"
#include "../EngineDefines.h"

namespace Graphics
{
    struct ENGINE_DLL ClearColor
    {
        union
        {
            struct
            {
                float R, G, B, A;
            };
            float RGBA[4];
        };

        /**
         * Assign colors as floating point values.
         */
        explicit ClearColor( float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f )
            : R( r )
            , G( g )
            , B( b )
            , A( a )
        {}

        static ClearColor FromARGB( uint32_t argb )
        {
            return ClearColor(
                ( ( ( argb & 0x00ff0000 ) >> 16 ) / 255.0f ),
                ( ( ( argb & 0x0000ff00 ) >> 8  ) / 255.0f ),
                ( ( ( argb & 0x000000ff ) >> 0  ) / 255.0f ),
                ( ( ( argb & 0xff000000 ) >> 24 ) / 255.0f ) );
        }

        static ClearColor FromARGB( uint8_t a, uint8_t r, uint8_t g, uint8_t b )
        {
            return ClearColor(
                r / 255.0f,
                g / 255.0f,
                b / 255.0f,
                a / 255.0f );
        }

        static const ClearColor AliceBlue;
        static const ClearColor AntiqueWhite;
        static const ClearColor Aqua;
        static const ClearColor Aquamarine;
        static const ClearColor Azure;
        static const ClearColor Beige;
        static const ClearColor Bisque;
        static const ClearColor Black;
        static const ClearColor BlanchedAlmond;
        static const ClearColor Blue;
        static const ClearColor BlueViolet;
        static const ClearColor Brown;
        static const ClearColor BurlyWood;
        static const ClearColor CadetBlue;
        static const ClearColor Chartreuse;
        static const ClearColor Chocolate;
        static const ClearColor Coral;
        static const ClearColor CornflowerBlue;
        static const ClearColor Cornsilk;
        static const ClearColor Crimson;
        static const ClearColor Cyan;
        static const ClearColor DarkBlue;
        static const ClearColor DarkCyan;
        static const ClearColor DarkGoldenrod;
        static const ClearColor DarkGray;
        static const ClearColor DarkGreen;
        static const ClearColor DarkKhaki;
        static const ClearColor DarkMagenta;
        static const ClearColor DarkOliveGreen;
        static const ClearColor DarkOrange;
        static const ClearColor DarkOrchid;
        static const ClearColor DarkRed;
        static const ClearColor DarkSalmon;
        static const ClearColor DarkSeaGreen;
        static const ClearColor DarkSlateBlue;
        static const ClearColor DarkSlateGray;
        static const ClearColor DarkTurquoise;
        static const ClearColor DarkViolet;
        static const ClearColor DeepPink;
        static const ClearColor DeepSkyBlue;
        static const ClearColor DimGray;
        static const ClearColor DodgerBlue;
        static const ClearColor Firebrick;
        static const ClearColor FloralWhite;
        static const ClearColor ForestGreen;
        static const ClearColor Fuchsia;
        static const ClearColor Gainsboro;
        static const ClearColor GhostWhite;
        static const ClearColor Gold;
        static const ClearColor Goldenrod;
        static const ClearColor Gray;
        static const ClearColor Green;
        static const ClearColor GreenYellow;
        static const ClearColor Honeydew;
        static const ClearColor HotPink;
        static const ClearColor IndianRed;
        static const ClearColor Indigo;
        static const ClearColor Ivory;
        static const ClearColor Khaki;
        static const ClearColor Lavender;
        static const ClearColor LavenderBlush;
        static const ClearColor LawnGreen;
        static const ClearColor LemonChiffon;
        static const ClearColor LightBlue;
        static const ClearColor LightCoral;
        static const ClearColor LightCyan;
        static const ClearColor LightGoldenrodYellow;
        static const ClearColor LightGray;
        static const ClearColor LightGreen;
        static const ClearColor LightPink;
        static const ClearColor LightSalmon;
        static const ClearColor LightSeaGreen;
        static const ClearColor LightSkyBlue;
        static const ClearColor LightSlateGray;
        static const ClearColor LightSteelBlue;
        static const ClearColor LightYellow;
        static const ClearColor Lime;
        static const ClearColor LimeGreen;
        static const ClearColor Linen;
        static const ClearColor Magenta;
        static const ClearColor Maroon;
        static const ClearColor MediumAquamarine;
        static const ClearColor MediumBlue;
        static const ClearColor MediumOrchid;
        static const ClearColor MediumPurple;
        static const ClearColor MediumSeaGreen;
        static const ClearColor MediumSlateBlue;
        static const ClearColor MediumSpringGreen;
        static const ClearColor MediumTurquoise;
        static const ClearColor MediumVioletRed;
        static const ClearColor MidnightBlue;
        static const ClearColor MintCream;
        static const ClearColor MistyRose;
        static const ClearColor Moccasin;
        static const ClearColor NavajoWhite;
        static const ClearColor Navy;
        static const ClearColor OldLace;
        static const ClearColor Olive;
        static const ClearColor OliveDrab;
        static const ClearColor Orange;
        static const ClearColor OrangeRed;
        static const ClearColor Orchid;
        static const ClearColor PaleGoldenrod;
        static const ClearColor PaleGreen;
        static const ClearColor PaleTurquoise;
        static const ClearColor PaleVioletRed;
        static const ClearColor PapayaWhip;
        static const ClearColor PeachPuff;
        static const ClearColor Peru;
        static const ClearColor Pink;
        static const ClearColor Plum;
        static const ClearColor PowderBlue;
        static const ClearColor Purple;
        static const ClearColor Red;
        static const ClearColor RosyBrown;
        static const ClearColor RoyalBlue;
        static const ClearColor SaddleBrown;
        static const ClearColor Salmon;
        static const ClearColor SandyBrown;
        static const ClearColor SeaGreen;
        static const ClearColor SeaShell;
        static const ClearColor Sienna;
        static const ClearColor Silver;
        static const ClearColor SkyBlue;
        static const ClearColor SlateBlue;
        static const ClearColor SlateGray;
        static const ClearColor Snow;
        static const ClearColor SpringGreen;
        static const ClearColor SteelBlue;
        static const ClearColor Tan;
        static const ClearColor Teal;
        static const ClearColor Thistle;
        static const ClearColor Tomato;
        static const ClearColor Transparent;
        static const ClearColor Turquoise;
        static const ClearColor Violet;
        static const ClearColor Wheat;
        static const ClearColor White;
        static const ClearColor WhiteSmoke;
        static const ClearColor Yellow;
        static const ClearColor YellowGreen;
        static const ClearColor TransparentBlack;
        static const ClearColor TransparentWhite;
    };
}