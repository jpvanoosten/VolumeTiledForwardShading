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
 *  @file SceneDX12.h
 *  @date March 9, 2016
 *  @author jeremiah
 *
 *  @brief 
 */

#include "../Scene.h"

class ProgressHandler;

namespace Core
{
    class SceneVisitor;
}

namespace Graphics
{
    class DeviceDX12;

    class SceneDX12 : public Scene
    {
    public:
        SceneDX12( std::shared_ptr<DeviceDX12> device );
        virtual ~SceneDX12();

        /**
        * Load a scene from a file on disc.
        */
        virtual bool LoadFromFile( std::shared_ptr<ComputeCommandBuffer> computeCommandBuffer, const std::wstring& fileName ) override;

        /**
        * Load a scene from a string.
        * The scene can be preloaded into a byte array and the
        * scene can be loaded from the loaded byte array.
        *
        * @param scene The byte encoded scene file.
        * @param format The format of the scene file.
        */
        virtual bool LoadFromString( std::shared_ptr<ComputeCommandBuffer> computeCommandBuffer, const std::string& scene, const std::string& format ) override;
        virtual void Render( Core::RenderEventArgs& renderEventArgs ) override;

        virtual std::shared_ptr<SceneNode> GetRootNode() const override;

        virtual void Accept( Core::SceneVisitor& visitor ) override;

    protected:

    private:
        friend class ProgressHandler;

        void ImportMaterial( std::shared_ptr<ComputeCommandBuffer> computeCommandBuffer, const aiMaterial& material, fs::path parentPath );
        void ImportMesh( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, const aiMesh& mesh );
        std::shared_ptr<SceneNode> ImportSceneNode( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, std::shared_ptr<SceneNode> parent, aiNode* aiNode );

        using MaterialMap = std::map<std::string, std::shared_ptr<Material> >;
        using MaterialList = std::vector < std::shared_ptr<Material> >;
        using MeshList = std::vector< std::shared_ptr<Mesh> >;

        std::weak_ptr<DeviceDX12> m_Device;

        MaterialMap m_MaterialMap;
        MaterialList m_Materials;
        MeshList m_Meshes;

        std::shared_ptr<SceneNode> m_RootNode;

        std::wstring m_SceneFile;
    };
}