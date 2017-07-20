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
 *  @file CompositePass.h
 *  @date May 17, 2016
 *  @author Jeremiah
 *
 *  @brief A composite pass allows you to create a rendering pass that consists of 
 *  several nested passes. A composite pass can contain composite passes making
 *  it also a hierarchical composite pass.
 *  Composite passes are useful if you want to group several passes together so they
 *  can be enabled and disabled together as a single pass.
 */

#include "AbstractPass.h"

class CompositePass : public AbstractPass
{
public:
    CompositePass();
    virtual ~CompositePass();

    /**
     * Add a pass to this composite pass.
     * @param renderPass The render pass to add to this composite pass.
     * @return A reference to this composite pass so you can easily add more
     * passes using method chaining.
     */
    CompositePass& AddPass( std::shared_ptr<RenderPass> renderPass );
    virtual void Render( Core::RenderEventArgs& e ) override;

protected:

private:

    using RenderPassList = std::vector< std::shared_ptr<RenderPass> >;
    RenderPassList m_CompositePasses;
};