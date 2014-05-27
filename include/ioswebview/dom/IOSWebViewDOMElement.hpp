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

#if defined(TARGET_IPHONE_SIMULATOR) or defined(TARGET_OS_IPHONE) // iOS
#pragma once

#include "minko/Common.hpp"
#include "minko/Signal.hpp"
#include "minko/dom/AbstractDOMElement.hpp"
#include "minko/dom/AbstractDOMEvent.hpp"

namespace ioswebview
{
	namespace dom
	{
        class IOSWebViewDOMEngine;

		class IOSWebViewDOMElement : public minko::dom::AbstractDOMElement,
			public std::enable_shared_from_this<IOSWebViewDOMElement>
		{
		public:
			typedef std::shared_ptr<IOSWebViewDOMElement> Ptr;

		private:
			IOSWebViewDOMElement(std::string jsAccessor);

		public:
			~IOSWebViewDOMElement()
			{
			};

			static
			Ptr
			getDOMElement(std::string jsElement, std::shared_ptr<IOSWebViewDOMEngine> engine);

			std::string
			getJavascriptAccessor();

			static
			Ptr
			create(std::string javascriptAccessor, std::shared_ptr<IOSWebViewDOMEngine> engine);

			std::string
			id();

			void
			id(std::string);

			std::string
			className();

			void
			className(std::string);

			std::string
			tagName();

			minko::dom::AbstractDOMElement::Ptr
			parentNode();

			std::vector<minko::dom::AbstractDOMElement::Ptr>
			childNodes();

			std::string
			textContent();

			void
			textContent(std::string);

			std::string
			innerHTML();

			void
			innerHTML(std::string);

			minko::dom::AbstractDOMElement::Ptr
			appendChild(minko::dom::AbstractDOMElement::Ptr);

			minko::dom::AbstractDOMElement::Ptr
			removeChild(minko::dom::AbstractDOMElement::Ptr);

			minko::dom::AbstractDOMElement::Ptr
			insertBefore(minko::dom::AbstractDOMElement::Ptr, minko::dom::AbstractDOMElement::Ptr);

			minko::dom::AbstractDOMElement::Ptr
			cloneNode(bool deep = true);

			std::string
			getAttribute(std::string name);

			void
			setAttribute(std::string name, std::string value);

			std::vector<minko::dom::AbstractDOMElement::Ptr>
			getElementsByTagName(std::string tagName);


			std::string
			style(std::string name);

			void
			style(std::string name, std::string value);

			minko::Signal<std::shared_ptr<minko::dom::AbstractDOMEvent>>::Ptr
			onclick();

			minko::Signal<std::shared_ptr<minko::dom::AbstractDOMEvent>>::Ptr
			onmousedown();

			minko::Signal<std::shared_ptr<minko::dom::AbstractDOMEvent>>::Ptr
			onmousemove();

			minko::Signal<std::shared_ptr<minko::dom::AbstractDOMEvent>>::Ptr
			onmouseup();

			minko::Signal<std::shared_ptr<minko::dom::AbstractDOMEvent>>::Ptr
			onmouseout();

			minko::Signal<std::shared_ptr<minko::dom::AbstractDOMEvent>>::Ptr
			onmouseover();

			void
			update();

		private:
			void
			addEventListener(std::string);
            
            void
            initialize(std::shared_ptr<IOSWebViewDOMEngine> engine);

		public:
			static
			std::list<IOSWebViewDOMElement::Ptr> domElements;

		private:
			static
			int
			_elementUid;
			
			static
			std::map<std::string,Ptr> _accessorToElement;

			std::string _jsAccessor;

			minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr _onclick;
			minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr _onmousedown;
			minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr _onmousemove;
			minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr _onmouseup;
			minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr _onmouseover;
			minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr _onmouseout;

			bool _onclickSet;
			bool _onmousedownSet;
			bool _onmousemoveSet;
			bool _onmouseupSet;
			bool _onmouseoverSet;
			bool _onmouseoutSet;
            
            std::shared_ptr<IOSWebViewDOMEngine> _engine;
		};
	}
}
#endif