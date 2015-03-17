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
        class POPGeometryWriterPreprocessor :
            public AbstractWriterPreprocessor<std::shared_ptr<scene::Node>>
        {
        public:
            typedef std::shared_ptr<POPGeometryWriterPreprocessor>  Ptr;

        private:
            typedef std::shared_ptr<scene::Node>                                NodePtr;
            typedef std::shared_ptr<AssetLibrary>                               AssetLibraryPtr;

            typedef std::shared_ptr<component::Surface>                         SurfacePtr;

            typedef std::shared_ptr<geometry::Geometry>                         GeometryPtr;

        public:
            ~POPGeometryWriterPreprocessor() = default;

            inline
            static
            Ptr
            create()
            {
                auto instance = Ptr(new POPGeometryWriterPreprocessor());

                return instance;
            }

            void
            process(NodePtr& node, AssetLibraryPtr assetLibrary);

        private:
            POPGeometryWriterPreprocessor();

            void
            markPOPGeometries(NodePtr root, std::unordered_set<NodePtr> ignoredNodes);

            void
            markPOPGeometry(NodePtr node, SurfacePtr surface, GeometryPtr geometry);

            std::unordered_set<NodePtr>
            collectAnimatedNodes(NodePtr root);
        };
    }
}
