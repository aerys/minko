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

#include "minko/Common.hpp"
#include "minko/data/Container.hpp"
#include "minko/Signal.hpp"

namespace minko
{
    namespace math
    {
        class OctTree :
            public std::enable_shared_from_this<OctTree>
        {
        public:
            typedef std::shared_ptr<OctTree> Ptr;

        private:
            typedef std::shared_ptr<scene::Node> NodePtr;

        private:
            static const uint _k = 2;

        private:
            uint                                                                        _maxDepth;
            uint                                                                        _depth;
            bool                                                                        _splitted;
            Ptr                                                                         _parent;
            std::vector<Ptr>                                                            _children; //x, y, z in {0, 1}, child index : x + y << 1 + z << 2
            std::list<NodePtr>                                                          _content;
            std::list<NodePtr>                                                          _childrenContent;
            float                                                                       _worldSize;
            std::shared_ptr<math::Vector3>                                              _center;
            std::unordered_map<NodePtr, Ptr>                                            _nodeToOctant;
            std::unordered_map<NodePtr, data::Container::PropertyChangedSignal::Slot>   _nodeToTransformChangedSlot;
            std::unordered_map<std::shared_ptr<math::Matrix4x4>, NodePtr>               _matrixToNode;
            std::shared_ptr<math::Box>                                                  _octantBox;


            bool _inside;
            NodePtr debugNode;

        public:
            inline static
            Ptr
            create(float                            worldSize,
                   uint                             maxDepth,
                   std::shared_ptr<math::Vector3>   center,
                   uint                             depth = 0)
            {
                return std::shared_ptr<OctTree>(new OctTree(worldSize, maxDepth, center, depth));
            }

            Ptr
            insert(NodePtr node);

            Ptr
            remove(NodePtr node);

            uint
            computeDepth(NodePtr node);

            NodePtr
            generateVisual(std::shared_ptr<file::AssetLibrary>              assetLibrary,
                           NodePtr                                          rootNode = nullptr);

            uint
            testFrustum(std::shared_ptr<math::Frustum>                      frustum);

            void
            testFrustum(std::shared_ptr<math::AbstractShape>                frustum,
                        std::function<void(std::shared_ptr<scene::Node>)>   insideFrustumCallback,
                        std::function<void(std::shared_ptr<scene::Node>)>   outsideFustumCallback);

        private:

            bool
            nodeChangedOctant(NodePtr node);

            void
            nodeModelToWorldChanged(std::shared_ptr<data::Container>        data,
                                    const std::string&                      propertyName);

            void
            split();

            float
            computeRadius(std::shared_ptr<component::BoundingBox>           boundingBox);

            void
            addChildContent(NodePtr                                         node);

            void
            removeChildContent(NodePtr                                      node);

            float
            edgeLength();

            OctTree(float                                                   worldSize,
                     uint                                                   maxDepth,
                     std::shared_ptr<math::Vector3>                         center,
                     uint                                                   depth);
        };
    }
}
