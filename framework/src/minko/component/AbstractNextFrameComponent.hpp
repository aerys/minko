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

#include "minko/Signal.hpp"
#include "minko/component/AbstractComponent.hpp"

namespace minko
{
	namespace component
	{
		class AbstractNextFrameComponent :
			public AbstractComponent,
			public std::enable_shared_from_this<AbstractNextFrameComponent>
		{
		public:
			typedef std::shared_ptr<AbstractNextFrameComponent>	Ptr;

		private:
			typedef std::shared_ptr<render::Texture>						TexturePtr;
			typedef std::shared_ptr<scene::Node>							NodePtr;
			typedef std::shared_ptr<AbstractComponent>						AbsCmpPtr;
			typedef std::shared_ptr<SceneManager>							SceneMgrPtr;
			typedef std::function<void(Ptr, SceneMgrPtr)>					FrameCallback;
			typedef std::function<void(Ptr, SceneMgrPtr)>					CullingCallback;
			typedef std::function<void(Ptr, SceneMgrPtr, uint, TexturePtr)>	RenderCallback;

		private:
			Signal<Ptr>::Ptr							_nextFrameBegin;
			Signal<Ptr>::Ptr							_nextFrameEnd;
			Signal<Ptr>::Ptr							_nextCullingBegin;
			Signal<Ptr>::Ptr							_nextCullingEnd;
			Signal<Ptr>::Ptr							_nextRenderBegin;
			Signal<Ptr>::Ptr							_nextRenderEnd;

			std::list<Signal<Ptr>::Slot>				_nextFrameBeginSlots;
			std::list<Signal<Ptr>::Slot>				_nextFrameEndSlots;
			std::list<Signal<Ptr>::Slot>				_nextCullingBeginSlots;
			std::list<Signal<Ptr>::Slot>				_nextCullingEndSlots;
			std::list<Signal<Ptr>::Slot>				_nextRenderBeginSlots;
			std::list<Signal<Ptr>::Slot>				_nextRenderEndSlots;

			Signal<AbsCmpPtr, NodePtr>::Slot			_targetAddedSlot;
			Signal<AbsCmpPtr, NodePtr>::Slot			_targetRemovedSlot;
			Signal<NodePtr, NodePtr, AbsCmpPtr>::Slot	_componentAddedSlot;
			Signal<NodePtr, NodePtr, AbsCmpPtr>::Slot	_componentRemovedSlot;

		protected:
			virtual
			void
			initialize();

			virtual
			void
			targetAddedHandler(AbsCmpPtr cmp, NodePtr target);

			virtual
			void
			targetRemovedHandler(AbsCmpPtr cmp, NodePtr target);

			virtual
			void
			componentAddedHandler(NodePtr node, NodePtr target, AbsCmpPtr cmp);

			virtual
			void
			componentRemovedHandler(NodePtr node, NodePtr target, AbsCmpPtr cmp);

			inline
			void
			nextFrameBegin(std::function<void(Ptr)> f)
			{
				_nextFrameBeginSlots.push_front(_nextFrameBegin->connect(f));
			}

			inline
			void
			nextFrameEnd(std::function<void(Ptr)> f)
			{
				_nextFrameEndSlots.push_front(_nextFrameEnd->connect(f));
			}

			inline
			void
			nextCullingBegin(std::function<void(Ptr)> f)
			{
				_nextCullingBeginSlots.push_front(_nextCullingBegin->connect(f));
			}

			inline
			void
			nextCullingEnd(std::function<void(Ptr)> f)
			{
				_nextCullingEndSlots.push_front(_nextCullingEnd->connect(f));
			}

			inline
			void
			nextRenderBegin(std::function<void(Ptr)> f)
			{
				_nextRenderBeginSlots.push_front(_nextRenderBegin->connect(f));
			}

			inline
			void
			nextRenderEnd(std::function<void(Ptr)> f)
			{
				_nextRenderEndSlots.push_front(_nextRenderEnd->connect(f));
			}

		private:
			void
			setSceneManager(std::shared_ptr<SceneManager> sceneManager);
		};
	}
}
