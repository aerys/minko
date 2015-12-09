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

#include "minko/data/Store.hpp"
#include "minko/Signal.hpp"

namespace minko
{
	namespace math
	{
		class OctTree :
			public std::enable_shared_from_this<OctTree>
		{
		public:
			typedef std::shared_ptr<OctTree>                                                Ptr;
		
		private:
            typedef std::weak_ptr<OctTree>                                                  WeakPtr;

			typedef std::shared_ptr<scene::Node>                                            NodePtr;
			typedef std::unordered_map<NodePtr, data::Store::PropertyChangedSignal::Slot>   PropertyChangedSlotMap;

            typedef std::shared_ptr<math::Box>                                              BoxPtr;

            struct NodeEntry
            {
                NodePtr node;
                BoxPtr  box;

                NodeEntry(NodePtr node, BoxPtr box) :
                    node(node),
                    box(box)
                {}
            };

		private:
			static const uint _k = 2;

		private:
			uint								        _maxDepth;
			uint								        _depth;
			bool								        _splitted;
            WeakPtr									    _parent;
            WeakPtr                                     _root;
			std::vector<Ptr>					        _children; //x, y, z in {0, 1}, child index : x + y << 1 + z << 2 
			std::vector<NodeEntry>					    _content;
            std::vector<NodePtr>                        _childrenContent;
            std::unordered_map<NodePtr, Ptr>            _nodeToOctant;
			float								        _worldSize;
			math::vec3							        _center;
			PropertyChangedSlotMap				        _nodeToTransformChangedSlot;
			std::shared_ptr<math::Box>			        _octantBox;

			bool 								        _inside;
			NodePtr 							        _debugNode;

            unsigned int 								_frustumLastPlaneId;

            std::unordered_set<NodePtr>                 _invalidNodes;
			
		public:
			inline static
			Ptr
			create(float				worldSize,
				   uint					maxDepth,
				   const math::vec3&	center,
				   uint					depth = 0)
			{
                auto instance = Ptr(new OctTree(worldSize, maxDepth, center, depth));

                instance->_root = instance;

                return instance;
			}

			Ptr
			insert(NodePtr node);

			Ptr
			remove(NodePtr node);

			uint
			computeDepth(NodePtr node);

			NodePtr
			generateVisual(std::shared_ptr<file::AssetLibrary>	assetLibrary, 
						   NodePtr								rootNode = nullptr);

			void
			testFrustum(std::shared_ptr<math::AbstractShape>				frustum, 
						std::function<void(std::shared_ptr<scene::Node>)>	insideFrustumCallback,
						std::function<void(std::shared_ptr<scene::Node>)>	outsideFustumCallback);

		private:
            bool
            nodeChangedOctant(NodePtr node);

            bool
            intersects(NodePtr node);

            bool
            childOctantsIntersection(NodePtr            node,
                                     std::vector<Ptr>&  octants);

            Ptr
            findNodeOctant(NodePtr node);

			void
			nodeModelToWorldChanged(NodePtr node);

            void
            invalidateNode(NodePtr node);

            Ptr
            doInsert(NodePtr node, unsigned int currentDepth, unsigned int optimalDepth);

            Ptr
            doRemove(NodePtr node);

			void
			split();

			float
			computeSize(std::shared_ptr<component::BoundingBox> boundingBox);

            void
            addToContent(NodePtr node);

            bool
            removeFromContent(NodePtr node);

			void
			addToChildContent(NodePtr node);

			float
			edgeLength();

			OctTree(float				worldSize,
					uint				maxDepth,
					const math::vec3& 	center,
					uint				depth);
		};
	}
}
