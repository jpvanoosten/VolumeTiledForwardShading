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
 *  @file DepthStencilState.h
 *  @date February 17, 2016
 *  @author jeremiah
 *
 *  @brief Depth/Stencil state object.
 */

#include "../EngineDefines.h"
#include "GraphicsEnums.h"

namespace Graphics
{
    struct ENGINE_DLL DepthMode
    {
        /**
        * Set to true to enable depth testing.
        * If enabled, the DepthFunction will be used to check if a pixel should
        * be written to the back buffer or discarded.
        * Default: True
        * @see DepthStencilState::DepthMode::DepthFunction
        */
        bool DepthEnable;

        /**
        * Enable or disable writing to the depth buffer.
        * This should be enabled for opaque geometry and disabled
        * for partially transparent objects.
        * Default: Enabled.
        */
        DepthWrite DepthWriteMask;

        /**
        * If the depth comparison function evaluates to TRUE then the pixel
        * is rasterized. If the depth comparison function evaluates to FALSE then
        * the pixel is discarded and not written to the back buffer.
        * The default value is CompareFunc::Less which means that source pixels
        * that are closer to the camera will pass the compare function and source
        * pixels that appear further away from the camera are discarded.
        */
        CompareFunction DepthFunction;

        explicit DepthMode( bool depthEnable = true,
                            DepthWrite depthWrite = DepthWrite::Enable,
                            CompareFunction depthFunction = CompareFunction::Less )
            : DepthEnable( depthEnable )
            , DepthWriteMask( depthWrite )
            , DepthFunction( depthFunction )
        {}
    };

    /**
    * The operation to perform on the stencil buffer can be specified
    * depending on whether the primitive that is being tested is front-facing
    * or back facing (according to the value of RasterizerState::FrontFacing).
    */
    struct ENGINE_DLL FaceOperation
    {
        /**
        * The operation to perform on the value in the stencil buffer if the
        * stencil comparision function (specified by FaceOperation::StencilFunction)
        * returns FALSE.
        * Default: StencilOperation::Keep
        */
        StencilOperation StencilFail;

        /**
        * The operation to perform on the value in the stencil buffer if the
        * stencil comparision function (specified by FaceOperation::StencilFunction)
        * returns TRUE and depth comparision function (determined by DepthMode::DepthFunction)
        * returns FALSE.
        * Default: StencilOperation::Keep
        */
        StencilOperation StencilPassDepthFail;

        /**
        * The operation to perform if both depth comparison function
        * (determined by DepthMode::DepthFunction) and stencil comparision
        * function (specified by FaceOperation::StencilFunction) returns TRUE.
        * Default: StencilOperation::Keep
        */
        StencilOperation StencilDepthPass;

        /**
        * The the comparison method to use for stencil operations.
        * Default: CompareFunction::Always (Tests always passes).
        * @see DepthStencilState::CompareFunction
        */
        CompareFunction StencilFunction;

        explicit FaceOperation( StencilOperation stencilFail = StencilOperation::Keep,
                                StencilOperation stencilPassDepthFail = StencilOperation::Keep,
                                StencilOperation stencilDepthPass = StencilOperation::Keep,
                                CompareFunction stencilFunction = CompareFunction::Always )
            : StencilFail( stencilFail )
            , StencilPassDepthFail( stencilPassDepthFail )
            , StencilDepthPass( stencilDepthPass )
            , StencilFunction( stencilFunction )
        {}
    };

    struct ENGINE_DLL StencilMode
    {
        /**
        * Set to true to enable stencil testing.
        * Default: false.
        */
        bool StencilEnabled;

        /**
        * A mask that is AND'd to the value in the stencil buffer before it is read.
        * Default: 0xff
        */
        uint8_t ReadMask;

        /**
        * A mask that is AND'd to the value in the stencil buffer before it is written.
        * Default: 0xff
        */
        uint8_t WriteMask;

        /**
        * The value to set the stencil buffer to if any of the StencilOperation
        * members of the FaceOperation struct is set to StencilOperation::Reference.
        * Default: 0
        */
        uint32_t StencilReference;

        /**
        * The compare function and pass/fail operations to perform on the stencil
        * buffer for front-facing polygons.
        */
        FaceOperation FrontFace;

        /**
        * The compare function and pass/fail operations to perform on the stencil
        * buffer for back-facing polygons.
        */
        FaceOperation BackFace;

        StencilMode( bool stencilEnabled = false,
                     uint8_t readMask = 0xff,
                     uint8_t writeMask = 0xff,
                     uint32_t stencilReference = 0,
                     FaceOperation frontFace = FaceOperation(),
                     FaceOperation backFace = FaceOperation() )
            : StencilEnabled( stencilEnabled )
            , ReadMask( readMask )
            , WriteMask( writeMask )
            , StencilReference( stencilReference )
            , FrontFace( frontFace )
            , BackFace( backFace )
        {}
    };

    class ENGINE_DLL DepthStencilState
    {
    public:
        virtual void SetDepthMode( const DepthMode& depthMode ) = 0;
        virtual const DepthMode& GetDepthMode() const = 0;

        virtual void SetStencilMode( const StencilMode& stencilMode ) = 0;
        virtual const StencilMode& GetStencilMode() const = 0;
    };
}