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

#include "minko/component/AbstractComponent.hpp"
#include "minko/Signal.hpp"
#include "minko/scene/Layout.hpp"

namespace minko
{
	namespace component
	{
		class MousePicking :
			public AbstractComponent
		{
		private:
			typedef std::shared_ptr<scene::Node>			NodePtr;
			typedef std::shared_ptr<math::Ray>				RayPtr;

		public:
			typedef std::pair<NodePtr, float>				Hit;
			typedef std::list<Hit>							HitList;
			typedef std::shared_ptr<MousePicking>			Ptr;
			typedef Signal<Ptr, HitList&, RayPtr>			MouseSignal;
			typedef MouseSignal::Ptr						MouseSignalPtr;

		private:
			MouseSignalPtr									_move;
			MouseSignalPtr									_over;
			MouseSignalPtr									_out;
			MouseSignalPtr									_rollOver;
			MouseSignalPtr									_rollOut;
			MouseSignalPtr									_leftButtonUp;
			MouseSignalPtr									_leftButtonDown;

			math::vec3										_previousRayOrigin;
			NodePtr											_lastItemUnderCursor;

			Signal<AbstractComponent::Ptr, NodePtr>::Slot	_targetAddedSlot;
			Signal<AbstractComponent::Ptr, NodePtr>::Slot	_targetRemovedSlot;

		public:
			inline static
			Ptr
			create()
			{
				auto mp = std::shared_ptr<MousePicking>(new MousePicking());

				mp->initialize();

				return mp;
			}

			inline
			MouseSignalPtr
			move()
			{
				return _move;
			}

			inline
			MouseSignalPtr
			over()
			{
				return _over;
			}

			void
			pick(RayPtr);

		private:
			MousePicking();

			void
			initialize();
		};
	}
}
