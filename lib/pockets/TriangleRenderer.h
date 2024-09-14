/*
 * Copyright (c) 2013 David Wicks, sansumbrella.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once
#include "Pockets.h"

/**
 A basic renderer for grouping rendered content and rendering in order.

 Renders anything that produces triangle vertices (overridden from IRenderable)
 Vertices are assumed to be in a format that works with GL_TRIANGLE_STRIP

 IRenderable objects' destructors remove them from the renderer, which keeps a
 non-owning raw pointer to renderables, allowing you to manage the object's
 lifetime as you see fit. Symmetrically, the renderer will disown any renderables
 when it is destructed.

 For now, this renders 2D Triangle Strips.
 Things under consideration:
 3D version
 GL_TRIANGLES version
 */

namespace pockets
{
  class TriangleRenderer
  {
  public:
    class IRenderable
    {
    public:
      struct Vertex
      {
        ci::Vec2f     position;
        ci::ColorA8u  color;
        ci::Vec2f     tex_coord;
        // TODO: add alignment padding?
      };
      IRenderable() = default;
      IRenderable( const IRenderable &other );
      IRenderable& operator=(const IRenderable &rhs);
      virtual ~IRenderable();
      //! return vertices as for GL_TRIANGLE_STRIP
      virtual std::vector<Vertex>  getVertices() = 0;
      void setLayer(int layer){ mLayer = layer; }
      int getLayer() const { return mLayer; }
    private:
      friend class TriangleRenderer;
      TriangleRenderer *mHost = nullptr;
      int               mLayer = 0;
    };
    typedef std::function<bool (const IRenderable*, const IRenderable*)> SortFn;
    TriangleRenderer() = default;
    ~TriangleRenderer();

    void add( IRenderable *renderable );
    void remove( IRenderable *renderable );
    void sort( const SortFn &fn = sortByLayerAscending );
    void render();
    static bool sortByLayerAscending( const IRenderable *lhs, const IRenderable *rhs );
  private:
    std::vector<IRenderable*>         mRenderables;
    std::vector<IRenderable::Vertex>  mVertices;
  };
}