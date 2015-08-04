/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "minko/file/AbstractWriterPreprocessor.hpp"

namespace minko
{
    namespace file
    {
        class SceneTreeFlattener :
            public AbstractWriterPreprocessor<std::shared_ptr<scene::Node>>
        {
        public:
            typedef std::shared_ptr<SceneTreeFlattener>  Ptr;

        private:
            typedef std::shared_ptr<scene::Node>                                NodePtr;
            typedef std::shared_ptr<AssetLibrary>                               AssetLibraryPtr;

            typedef std::shared_ptr<component::Surface>                         SurfacePtr;

            typedef std::shared_ptr<geometry::Geometry>                         GeometryPtr;

            struct RetargetedSurface
            {
                SurfacePtr surface;
                math::mat4 matrix;

                RetargetedSurface(SurfacePtr surface, const math::mat4 matrix) :
                    surface(surface),
                    matrix(matrix)
                {
                }
            };

        public:
            ~SceneTreeFlattener() = default;

            inline
            static
            Ptr
            create()
            {
                auto instance = Ptr(new SceneTreeFlattener());

                return instance;
            }

            void
            process(NodePtr& node, AssetLibraryPtr assetLibrary) override;

        private:
            SceneTreeFlattener();

            bool
            collapseNode(NodePtr                        node,
                         NodePtr                        parent,
                         NodePtr                        root,
                         std::list<RetargetedSurface>&  retargetedSurfaces);

            void
            patchNode(NodePtr node, const std::list<RetargetedSurface>& retargetedSurfaces);
        };
    }
}
