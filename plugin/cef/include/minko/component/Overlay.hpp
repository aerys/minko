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
#include "minko/AbstractCanvas.hpp"
#include "minko/component/AbstractComponent.hpp"
#include "minko/dom/AbstractDOMElement.hpp"
#include "minko/dom/AbstractDOMEngine.hpp"

namespace minko
{
	namespace component
	{

		class Overlay :
			public AbstractComponent,
			public std::enable_shared_from_this<Overlay>
		{
		public:
			typedef std::shared_ptr<Overlay> Ptr;

		private:
			typedef std::shared_ptr<scene::Node>							NodePtr;


			Overlay();

			void
			initialize(AbstractCanvas::Ptr);

		public:

			static
			Ptr
			create(AbstractCanvas::Ptr canvas)
			{
				Ptr overlay(new Overlay());
				overlay->initialize(canvas);

				return overlay;
			}
			
			void
			load(std::string uri)
			{
				_domEngine->load(uri);
			}

			void
			unload()
			{
				_domEngine->unload();
				_domEngine = nullptr;
			}

			std::shared_ptr<Signal<std::string>>
			onload()
			{
				return _domEngine->onload();
			}

			std::shared_ptr<Signal<std::string>>
			onmessage()
			{
				return _domEngine->onmessage();
			}

			minko::dom::AbstractDOMEngine::Ptr
			domEngine()
			{
				return _domEngine;
			}

		private:

			void
			targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target);

			void
			targetRemovedHandler(AbstractComponent::Ptr ctrl, NodePtr target);

			void
			removedHandler(NodePtr node, NodePtr target, NodePtr ancestor);
			
		private:
			AbstractCanvas::Ptr														_canvas;

			Signal<AbstractComponent::Ptr, NodePtr>::Slot							_targetAddedSlot;
			Signal<AbstractComponent::Ptr, NodePtr>::Slot							_targetRemovedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot									_removedSlot;

			dom::AbstractDOMEngine::Ptr												_domEngine;
		};
	}
}