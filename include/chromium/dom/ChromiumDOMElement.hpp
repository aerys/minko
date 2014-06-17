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

#if defined(CHROMIUM)
#pragma once

#include "minko/Common.hpp"
#include "minko/Signal.hpp"
#include "minko/dom/AbstractDOMEvent.hpp"
#include "minko/dom/AbstractDOMElement.hpp"
#include "chromium/dom/ChromiumDOMElementV8Handler.hpp"
#include "include/cef_render_process_handler.h"

namespace chromium
{
	namespace dom
	{
		class ChromiumDOMElement : public minko::dom::AbstractDOMElement,
			public std::enable_shared_from_this<ChromiumDOMElement>
		{
		public:
			typedef std::shared_ptr<ChromiumDOMElement> Ptr;
			~ChromiumDOMElement();

		private:
			ChromiumDOMElement(CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>);
			
		public:

			static
			Ptr
			create(CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>);

			static
			Ptr
			getDOMElementFromV8Object(CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>);

			static
			void
			clearAll();

			static
			std::vector<AbstractDOMElement::Ptr>
			v8ElementArrayToList(CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>);

			void
			clear();

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


			AbstractDOMElement::Ptr
			parentNode();

			std::vector<AbstractDOMElement::Ptr>
			childNodes();

			
			std::string
			textContent();

			void
			textContent(std::string content);

			std::string
			innerHTML();

			void
			innerHTML(std::string html);

			AbstractDOMElement::Ptr
			appendChild(AbstractDOMElement::Ptr);

			AbstractDOMElement::Ptr
			removeChild(AbstractDOMElement::Ptr);


			AbstractDOMElement::Ptr
			insertBefore(AbstractDOMElement::Ptr, AbstractDOMElement::Ptr);

			AbstractDOMElement::Ptr
			cloneNode(bool deep);

			std::string
			getAttribute(std::string name);

			void
			setAttribute(std::string name, std::string value);

			std::vector<AbstractDOMElement::Ptr>
			getElementsByTagName(std::string tagName);


			std::string
			style(std::string name);

			void
			style(std::string name, std::string value);

			minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr
			onclick();

			minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr
			onmousedown();

			minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr
			onmousemove();

			minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr
			onmouseup();

			minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr
			onmouseout();

			minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr
			onmouseover();

			minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr
			ontouchdown();

			minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr
			ontouchup();

			minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr
			ontouchmotion();

			static
			void
			addFunction(std::function<void()>);

			static
			void
			update();

		private:

			CefRefPtr<CefV8Value>
			getFunction(std::string name);

			CefRefPtr<CefV8Value>
			getProperty(std::string name);

			void
			setProperty(std::string name, CefRefPtr<CefV8Value>);

			void
			addEventListener(std::string type);

		private:

			static
			std::map<CefRefPtr<CefV8Value>, Ptr> _v8NodeToElement;

			static
			std::map<Ptr, CefRefPtr<CefV8Value>> _elementToV8Object;

			std::atomic<bool>	_blocker;
			
			bool _onclickCallbackSet;
			bool _onmousedownCallbackSet;
			bool _onmousemoveCallbackSet;
			bool _onmouseupCallbackSet;
			bool _onmouseoverCallbackSet;
			bool _onmouseoutCallbackSet;

			std::shared_ptr<minko::Signal<minko::dom::AbstractDOMEvent::Ptr>> _onclick;
			std::shared_ptr<minko::Signal<minko::dom::AbstractDOMEvent::Ptr>> _onmousedown;
			std::shared_ptr<minko::Signal<minko::dom::AbstractDOMEvent::Ptr>> _onmousemove;
			std::shared_ptr<minko::Signal<minko::dom::AbstractDOMEvent::Ptr>> _onmouseup;
			std::shared_ptr<minko::Signal<minko::dom::AbstractDOMEvent::Ptr>> _onmouseover;
			std::shared_ptr<minko::Signal<minko::dom::AbstractDOMEvent::Ptr>> _onmouseout;

			CefRefPtr<ChromiumDOMElementV8Handler> _v8Handler;
			CefRefPtr<CefV8Value> _v8NodeObject;
			CefRefPtr<CefV8Context> _v8Context;

			static std::list<std::function<void()>>	_functionList;

			bool _cleared;
		};
	}
}
#endif