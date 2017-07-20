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
 *  @file PostprocessPass.h
 *  @date May 18, 2016
 *  @author jeremiah
 *
 *  @brief PostprocessPass can be used to perform prosprocess effects.
 */

#include "BasePass.h"

namespace Graphics
{
    class Texture;
}

class PostprocessPass : public BasePass
{
public:
    PostprocessPass( std::shared_ptr<Graphics::Scene> scene, std::shared_ptr<Graphics::GraphicsPipelineState> pipeline, const glm::mat4& projectionMatrix, std::shared_ptr<Graphics::Texture> texture, bool bUseMaterial = true );

    // Render the pass. This should only be called by the RenderTechnique.
    virtual void Render( Core::RenderEventArgs& e ) override;

    virtual void Visit( Graphics::SceneNode& node ) override;
    virtual void Visit( Graphics::Mesh& mesh ) override;

protected:

private:
    glm::mat4 m_ProjectionMatrix;
    std::shared_ptr<Graphics::Texture> m_Texture;
};