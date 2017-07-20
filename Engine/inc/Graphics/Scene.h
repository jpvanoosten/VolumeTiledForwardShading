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
 *  @file Scene.h
 *  @date March 7, 2016
 *  @author jeremiah
 *
 *  @brief Base class for a scene object.
 */

#include "../EngineDefines.h"
#include "../Events.h"
#include "Object.h"


namespace Core
{
    class SceneVisitor;
}

namespace Graphics
{
    class Device;
    class SceneNode;
    class Material;
    class Mesh;
    class CopyCommandBuffer;
    class ComputeCommandBuffer;

    class ENGINE_DLL Scene : public Core::Object
    {
    public:

        /**
        * Load a scene from a file on disc.
        */
        virtual bool LoadFromFile( std::shared_ptr<ComputeCommandBuffer> computeCommandBuffer, const std::wstring& fileName ) = 0;

        /**
        * Load a scene from a string.
        * The scene can be preloaded into a byte array and the
        * scene can be loaded from the loaded byte array.
        *
        * @param scene The byte encoded scene file.
        * @param format The format of the scene file. Supported formats are: TODO: get a list of supported formats from ASSIMP.
        */
        virtual bool LoadFromString( std::shared_ptr<ComputeCommandBuffer> computeCommandBuffer, const std::string& scene, const std::string& format ) = 0;
        virtual void Render( Core::RenderEventArgs& renderEventArgs ) = 0;

        virtual std::shared_ptr<SceneNode> GetRootNode() const = 0;

        virtual void Accept( Core::SceneVisitor& visitor ) = 0;

        // Register for the progress callback to be notified of scene loading progress.
        Core::ProgressEvent LoadingProgress;

    protected:
        virtual void OnLoadingProgress( Core::ProgressEventArgs& e );

    };
}