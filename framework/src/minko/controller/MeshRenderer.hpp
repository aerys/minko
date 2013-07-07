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

#include "minko/controller/AbstractController.hpp"
#include "minko/Signal.hpp"

#if _MSC_VER <= 1700
	// avoid Warning C4503 with VS on ControllerChangedSlot
	#pragma warning(disable:4503)
#endif

namespace minko
{
	namespace controller
	{
		class MeshRenderer :
			public AbstractController,
			public std::enable_shared_from_this<MeshRenderer>
		{
		public:
			typedef std::shared_ptr<MeshRenderer>	Ptr;

		private:
			typedef std::shared_ptr<scene::Node>									NodePtr;
			typedef Signal<NodePtr, NodePtr, AbstractController::Ptr>::Slot			ControllerChangedSlot;
			typedef std::unordered_map<NodePtr, std::vector<ControllerChangedSlot>>	ControllerSlotMap;

		private:
			bool											_enabled;

			Signal<AbstractController::Ptr, NodePtr>::Slot	_targetAddedSlot;
			Signal<AbstractController::Ptr, NodePtr>::Slot	_targetRemovedSlot;
			ControllerSlotMap								_controllerSlots;

		public:
			inline static
			Ptr
			create()
			{
				auto mr = std::shared_ptr<MeshRenderer>(new MeshRenderer());

				mr->initialize();

				return mr;
			}

			inline
			bool
			enabled()
			{
				return _enabled;
			}

			inline
			void
			enabled(bool enabled)
			{
				_enabled = enabled;
				// FIXME: actually disable the renderer
			}

		private:
			MeshRenderer();

			void
			initialize();

			void
			targetAddedHandler(AbstractController::Ptr ctrl, NodePtr node);

			void
			targetRemovedHandler(AbstractController::Ptr ctrl, NodePtr node);

			void
			controllerAddedHandler(NodePtr node, NodePtr target, AbstractController::Ptr ctrl);

			void
			controllerRemovedHandler(NodePtr node, NodePtr target, AbstractController::Ptr ctrl);

			void
			addSurface(std::shared_ptr<Surface> surface);

			void
			removeSurface(std::shared_ptr<Surface> surface);
		};
	}
}
