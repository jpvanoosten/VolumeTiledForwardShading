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
 *  @file Mesh.h
 *  @date March 7, 2016
 *  @author jeremiah
 *
 *  @brief Mesh.
 */

#include "../EngineDefines.h"

namespace Core
{
    class RenderEventArgs;
    class SceneVisitor;
}

namespace Graphics
{
    class Device;
    class VertexBuffer;
    class IndexBuffer;
    class Material;

    class ENGINE_DLL Mesh
    {
    public:
        struct alignas(16) Vertex
        {
            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec3 Tangent;
            glm::vec3 BiTangent;
            glm::vec3 TexCoord;
        };

        Mesh( std::shared_ptr<Device> device );
        virtual ~Mesh();

        void SetVertexBuffer( uint32_t slotID, std::shared_ptr<VertexBuffer> vertexBuffer );
        void SetIndexBuffer( std::shared_ptr<IndexBuffer> indexBuffer );

        void SetMaterial( std::shared_ptr<Material> material );
        std::shared_ptr<Material> GetMaterial() const;

        virtual void Render( Core::RenderEventArgs& renderArgs, uint32_t instanceCount = 1, uint32_t firstInstance = 0 );

        virtual void Accept( Core::SceneVisitor& visitor );

    protected:

    private:
        using BufferMap = std::map<uint32_t, std::shared_ptr<VertexBuffer> >;

        std::weak_ptr<Device> m_Device;

        BufferMap m_VertexBuffers;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
        std::shared_ptr<Material> m_Material;
    };
}