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
#include "minko/dom/AbstractDOMEvent.hpp"
#include "minko/dom/AbstractDOMElement.hpp"
#include "chromium/dom/ChromiumDOMElementV8Handler.hpp"
#include "include/cef_render_process_handler.h"

namespace chromium
{
	namespace dom
	{
		class ChromiumDOMElement : minko::dom::AbstractDOMElement,
			public std::enable_shared_from_this<ChromiumDOMElement>
		{
		public:
			typedef std::shared_ptr<ChromiumDOMElement> Ptr;

		private:
			ChromiumDOMElement(CefRefPtr<CefV8Value>);

			Ptr
			initialize();

		public:

			static
			Ptr
			create(CefRefPtr<CefV8Value>);

			static
			Ptr
			getDOMElementFromV8Object(CefRefPtr<CefV8Value>);


			std::string
			id();

			std::string
			className();

			std::string
			tagName();


			AbstractDOMElement::Ptr
			parentNode();

			std::list<AbstractDOMElement::Ptr>
			childNodes();

			AbstractDOMElement::Ptr
			appendChild(AbstractDOMElement::Ptr);

			AbstractDOMElement::Ptr
			removeChild(AbstractDOMElement::Ptr);


			AbstractDOMElement::Ptr
			insertBefore(AbstractDOMElement::Ptr);

			AbstractDOMElement::Ptr
			cloneNode(bool deep);

			std::string
			getAttribute(std::string name);

			void
			setAttribute(std::string name, std::string value);

			std::list<AbstractDOMElement::Ptr>
			getElementsByTagName(std::string tagName);


			std::string
			style(std::string name);

			std::string
			style(std::string name, std::string value);

			Signal<AbstractDOMEvent::Ptr>::Ptr
			onclick();

			Signal<AbstractDOMEvent::Ptr>::Ptr
			onmousedown();

			Signal<AbstractDOMEvent::Ptr>::Ptr
			onmousemove();

			Signal<AbstractDOMEvent::Ptr>::Ptr
			onmouseup();

			Signal<AbstractDOMEvent::Ptr>::Ptr
			onmouseout();

			Signal<AbstractDOMEvent::Ptr>::Ptr
			onmouseover();

		private:

			CefRefPtr<CefV8Value>
			getFunction(std::string name);

			CefRefPtr<CefV8Value>
			getProperty(std::string name);

			std::list<AbstractDOMElement::Ptr>
			v8ElementArrayToList(CefRefPtr<CefV8Value>);

			void
			addEventListener(std::string type);

		private:

			static
			std::map<CefRefPtr<CefV8Value>, Ptr> _v8NodeToElement;

			static
			std::map<AbstractDOMElement::Ptr, CefRefPtr<CefV8Value>> _elementToV8Object;

			bool _onclickCallbackSet;
			bool _onmousedownCallbackSet;
			bool _onmousemoveCallbackSet;
			bool _onmouseupCallbackSet;
			bool _onmouseoverCallbackSet;
			bool _onmouseoutCallbackSet;

			CefRefPtr<ChromiumDOMElementV8Handler> _v8Handler;
			CefRefPtr<CefV8Value> _v8NodeObject;
		};
	}
}