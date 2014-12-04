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

#include <minko/Common.hpp>
#include <minko/BulletCommon.hpp>

#include <minko/component/AbstractComponent.hpp>

namespace minko
{
	namespace component
	{
		namespace bullet
		{
			class ColliderDebug: 
				public AbstractComponent
			{
			public:
				typedef std::shared_ptr<ColliderDebug>				Ptr;

			private:
				typedef std::shared_ptr<scene::Node>				NodePtr;
				typedef std::shared_ptr<AbstractComponent>			AbsCmpPtr;
				typedef std::shared_ptr<Collider>					ColliderPtr;
				typedef std::shared_ptr<math::Matrix4x4>			Matrix4x4Ptr;
				typedef std::shared_ptr<Surface>					SurfacePtr;
				typedef std::shared_ptr<file::AssetLibrary>			AssetLibraryPtr;

			private:
				AssetLibraryPtr										_assets;
				SurfacePtr											_surface;
                NodePtr                                             _node;

				Signal<ColliderPtr, Matrix4x4Ptr>::Slot				_physicsTransformChangedSlot;

				Signal<AbsCmpPtr, NodePtr>::Slot					_targetAddedSlot;
				Signal<AbsCmpPtr, NodePtr>::Slot					_targetRemovedSlot;
				Signal<NodePtr, NodePtr, NodePtr>::Slot				_addedSlot;
				Signal<NodePtr, NodePtr, NodePtr>::Slot				_removedSlot;

			public:
				inline static
				Ptr
				create(AssetLibraryPtr assets)
				{
					Ptr ptr = std::shared_ptr<ColliderDebug>(new ColliderDebug(assets));

					ptr->initialize();

					return ptr;
				}

				AbstractComponent::Ptr
				clone(const CloneOption& option);

			private:
				ColliderDebug(AssetLibraryPtr);

				void
				initialize();

				void
				initializeDisplay();

				void
				targetAddedHandler(AbsCmpPtr, NodePtr);

				void
				targetRemovedHandler(AbsCmpPtr, NodePtr);

				void
				addedHandler(NodePtr, NodePtr, NodePtr);

				void
				removedHandler(NodePtr, NodePtr, NodePtr);

				void
				physicsTransformChangedHandler(ColliderPtr, Matrix4x4Ptr);
			};
		}
	}
}

