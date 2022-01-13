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
#include "minko/dom/AbstractDOM.hpp"

namespace minko
{
	namespace component
	{

		class HtmlOverlay :
			public AbstractComponent
		{
		public:
			typedef std::shared_ptr<HtmlOverlay> Ptr;
			~HtmlOverlay();

		private:
			typedef std::shared_ptr<scene::Node> NodePtr;


			HtmlOverlay(int argc, char** argv);

		public:

			static
			Ptr
			create(int argc, char** argv)
			{
				if (_instance != nullptr)
					throw("Only one instance of HtmlOverlay is permitted");

				Ptr overlay(new HtmlOverlay(argc, argv));

				_instance = overlay;

				return overlay;
			}

			static
			Ptr
			instance()
			{
				return _instance;
			}
			
			minko::dom::AbstractDOM::Ptr
			load(std::string uri)
			{
                if (_domEngine == nullptr || target() == nullptr)
                    throw std::logic_error("HtmlOverlay component should be added to a node and initialized before HtmlOverlay::load is called");

				return _domEngine->load(uri);
			}

			void
			clear()
			{
				if (_cleared)
					return;

				_domEngine->clear();
				_domEngine = nullptr;
				_cleared = true;
			}

			std::shared_ptr<Signal<minko::dom::AbstractDOM::Ptr, std::string>>
			onload()
			{
				return _domEngine->onload();
			}

			std::shared_ptr<Signal<minko::dom::AbstractDOM::Ptr, std::string>>
			onmessage()
			{
				return _domEngine->onmessage();
			}

			dom::AbstractDOM::Ptr mainDOM()
			{
				return _domEngine->mainDOM();
			}

			void
			visible(bool);
            
            bool
			visible();

            void
            secure(bool value)
            {
                _secure = value;
            }
            
            bool
            secure()
            {
                return _secure;
            }

            // Render to texture

            void
            enableRenderToTexture(std::shared_ptr<minko::render::AbstractTexture> texture);

            void
            disableRenderToTexture();

            // Set the origins allowed for the overlays.
            void
            setAllowedOrigins(const std::vector<std::string>& allowedOrigins);

        private:

			minko::dom::AbstractDOMEngine::Ptr
			domEngine()
			{
				return _domEngine;
			}

			void
			targetAdded(NodePtr target);

			void
			targetRemoved(NodePtr target);

			void
			removedHandler(NodePtr node, NodePtr target, NodePtr ancestor);
			
		private:
			static HtmlOverlay::Ptr						    _instance;

			AbstractCanvas::Ptr							    _canvas; 
			SceneManager::Ptr							    _sceneManager;

			Signal<AbstractComponent::Ptr, NodePtr>::Slot   _targetAddedSlot;
			Signal<AbstractComponent::Ptr, NodePtr>::Slot   _targetRemovedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot		    _removedSlot;

			dom::AbstractDOMEngine::Ptr					    _domEngine;

			bool										    _cleared;

            bool                                            _secure;
		};
	}
}
