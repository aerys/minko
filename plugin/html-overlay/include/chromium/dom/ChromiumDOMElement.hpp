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
#include "minko/dom/AbstractDOMMouseEvent.hpp"
#include "minko/dom/AbstractDOMTouchEvent.hpp"
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


            inline
            std::string
            id()
            {
                return getProperty<std::string>("id");
            }

            inline
            void
            id(std::string newId)
            {
                setProperty<std::string>("id", newId);
            }

            inline
            std::string
            className()
            {
                return getProperty<std::string>("className");
            }

            inline
            void
            className(std::string newClass)
            {
                setProperty<std::string>("className", newClass);
            }

            inline
            std::string
            tagName()
            {
                return getProperty<std::string>("tagName");
            }

            inline
            AbstractDOMElement::Ptr
            parentNode()
            {
                return getProperty<AbstractDOMElement::Ptr>("parentNode");
            }

            inline
            std::vector<AbstractDOMElement::Ptr>
            childNodes()
            {
                return getProperty<std::vector<AbstractDOMElement::Ptr>>("childNodes");
            }

            inline
            std::string
            textContent()
            {
                return getProperty<std::string>("textContent");
            }

            inline
            void
            textContent(std::string content)
            {
                setProperty<std::string>("textContent", content);
            }

            inline
            std::string
            value()
            {
                return getProperty<std::string>("value");
            }

            inline
            void
            value(const std::string& v)
            {
                setProperty<std::string>("value", v);
            }

            inline
            std::string
            innerHTML()
            {
                return getProperty<std::string>("innerHTML");
            }

            inline
            void
            innerHTML(std::string html)
            {
                setProperty<std::string>("innerHTML", html);
            }

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

			minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Ptr
			onclick();

			minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Ptr
			onmousedown();

			minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Ptr
			onmousemove();

			minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Ptr
			onmouseup();

			minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Ptr
			onmouseout();

			minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Ptr
			onmouseover();

            minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr
            onchange();

            minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr
            oninput();

			static
			void
			addFunction(std::function<void()>);

			static
			void
			update();

		private:

			CefRefPtr<CefV8Value>
			getFunction(std::string name);
            
            template <typename T>
            T
            getProperty(std::string name);

            template <typename T>
            void
            setProperty(std::string name, T);

            template <typename T>
			T
			getV8Property(std::string name);

            template <typename T>
			void
			setV8Property(std::string name, T);

			void
			addEventListener(std::string type);

		private:

			static
			std::map<CefRefPtr<CefV8Value>, Ptr> _v8NodeToElement;

			static
			std::map<Ptr, CefRefPtr<CefV8Value>> _elementToV8Object;

			std::atomic<bool>	_blocker;

            std::shared_ptr<minko::Signal<minko::dom::AbstractDOMEvent::Ptr>>      _onchange;
            std::shared_ptr<minko::Signal<minko::dom::AbstractDOMEvent::Ptr>>      _oninput;

			std::shared_ptr<minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>> _onclick;
			std::shared_ptr<minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>> _onmousedown;
			std::shared_ptr<minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>> _onmousemove;
			std::shared_ptr<minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>> _onmouseup;
			
			std::shared_ptr<minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>> _onmouseover;
			std::shared_ptr<minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>> _onmouseout;

            bool _onchangeCallbackSet;
            bool _oninputCallbackSet;

			bool _onclickCallbackSet;
			bool _onmousedownCallbackSet;
			bool _onmousemoveCallbackSet;
			bool _onmouseupCallbackSet;

			bool _onmouseoverCallbackSet;
			bool _onmouseoutCallbackSet;

			CefRefPtr<ChromiumDOMElementV8Handler> _v8Handler;
			CefRefPtr<CefV8Value> _v8NodeObject;
			CefRefPtr<CefV8Context> _v8Context;

			static std::list<std::function<void()>>	_functionList;
			static std::mutex _functionListMutex;

			bool _cleared;
		};
	}
}
#endif