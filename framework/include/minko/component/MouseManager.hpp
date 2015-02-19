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

#include "minko/Common.hpp"

#include "minko/Signal.hpp"
#include "minko/component/AbstractComponent.hpp"
#include "minko/input/Mouse.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/scene/Node.hpp"

namespace minko
{
	namespace component
	{
		class MouseManager :
			public AbstractComponent
		{
		private:
			typedef std::shared_ptr<scene::Node>	NodePtr;
			typedef std::shared_ptr<math::Ray>		RayPtr;
			typedef std::shared_ptr<input::Mouse>	MousePtr;

		public:
			typedef std::pair<NodePtr, float>		Hit;
			typedef std::list<Hit>					HitList;
			typedef std::shared_ptr<MouseManager>	Ptr;

		private:
			MousePtr										_mouse;
			math::vec3									    _previousRayOrigin;
			NodePtr											_lastItemUnderCursor;

			RayPtr											_ray;

			Signal<AbstractComponent::Ptr, NodePtr>::Slot	_targetAddedSlot;
			Signal<AbstractComponent::Ptr, NodePtr>::Slot	_targetRemovedSlot;
			Signal<MousePtr, int, int>::Slot				_mouseMoveSlot;
			Signal<MousePtr>::Slot							_mouseLeftButtonDownSlot;

		public:
			inline static
			Ptr
			create(std::shared_ptr<input::Mouse> mouse = nullptr)
			{
                return std::shared_ptr<MouseManager>(new MouseManager(mouse));
			}

			inline
			MousePtr
			mouse()
			{
				return _mouse;
			}

			void
			pick(std::shared_ptr<math::Ray> ray);

        protected:
            void
            targetAdded(scene::Node::Ptr target);

            void
            targetRemoved(scene::Node::Ptr target);

		private:
            MouseManager(std::shared_ptr<input::Mouse> mouse);

			void
			initialize();
		};
	}
}
