/*
Copyright (c) 2013 Aerys

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

#include <minko/component/AbstractComponent.hpp>
#include "minko/component/Skin.hpp"

namespace minko
{
	namespace component
	{
		class Skinning:
			public AbstractComponent,
			public std::enable_shared_from_this<Skinning>
		{
		public:	
			typedef std::shared_ptr<Skinning>						Ptr;
	
		private:
			typedef std::shared_ptr<scene::Node>					NodePtr;
			typedef std::shared_ptr<component::AbstractComponent>	AbsCmpPtr;
			typedef std::shared_ptr<component::SceneManager>		SceneManagerPtr;
			typedef std::shared_ptr<geometry::Geometry>				GeometryPtr;

			typedef Signal<AbsCmpPtr, NodePtr>						TargetAddedOrRemovedSignal;
			typedef Signal<NodePtr, NodePtr, NodePtr>				AddedOrRemovedSignal;
			typedef Signal<SceneManagerPtr>							SceneManagerSignal;

		private:
			const Skin									_skin;

			std::unordered_map<NodePtr, GeometryPtr>	_targetGeometry;
			std::unordered_map<NodePtr, clock_t>		_targetStartTime;
			
			TargetAddedOrRemovedSignal::Slot			_targetAddedSlot;
			TargetAddedOrRemovedSignal::Slot			_targetRemovedSlot;
			AddedOrRemovedSignal::Slot					_addedSlot;
			AddedOrRemovedSignal::Slot					_removedSlot;
			SceneManagerSignal::Slot					_frameBeginSlot;

		public:
			inline static
			Ptr
			create(const Skin& skin)
			{
				Ptr ptr(new Skinning(skin));

				ptr->initialize();

				return ptr;
			}

		private:
			Skinning(const Skin& skin);

			void
			initialize();

			void
			targetAddedHandler(AbsCmpPtr, NodePtr);

			void
			targetRemovedHandler(AbsCmpPtr, NodePtr);

			void
			addedHandler(NodePtr, NodePtr, NodePtr);

			void
			removedHandler(NodePtr, NodePtr, NodePtr);

			void
			findSceneManager();

			void
			setSceneManager(SceneManagerPtr);

			void
			frameBeginHandler(SceneManagerPtr);

			void
			update(NodePtr, float time);

			void
			updateFrame(NodePtr, unsigned int frameId);

		};
	}
}
