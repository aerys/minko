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
#include "chromium/dom/ChromiumDOMObject.hpp"
#include "chromium/dom/ChromiumDOMElementV8Handler.hpp"
#include "include/cef_render_process_handler.h"

namespace chromium
{
	namespace dom
	{
		class ChromiumDOMElement : public virtual minko::dom::AbstractDOMElement,
            public chromium::dom::ChromiumDOMObject,
			public std::enable_shared_from_this<ChromiumDOMElement>
		{
		public:
			typedef std::shared_ptr<ChromiumDOMElement> Ptr;
			~ChromiumDOMElement();

		private:
			ChromiumDOMElement(CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>);
            
            typedef std::shared_ptr<minko::Signal<std::shared_ptr<minko::dom::AbstractDOMEvent>>>       DOMEventSignal;
            typedef std::shared_ptr<minko::Signal<std::shared_ptr<minko::dom::AbstractDOMMouseEvent>>>  DOMMouseEventSignal;
            typedef std::shared_ptr<minko::Signal<std::shared_ptr<minko::dom::AbstractDOMTouchEvent>>>  DOMTouchEventSignal;
			
		public:

            static
            void
            createWrapper(CefRefPtr<CefV8Value>*, CefRefPtr<CefV8Context>*, std::atomic<bool>*, ChromiumDOMElement::Ptr*);

            static
            void
            appendChildWrapper(AbstractDOMElement::Ptr*, ChromiumDOMElement* target);

            static
            void
            removeChildWrapper(AbstractDOMElement::Ptr*, ChromiumDOMElement* target);

            static
            void
            insertBeforeWrapper(AbstractDOMElement::Ptr*, AbstractDOMElement::Ptr*, ChromiumDOMElement*);

            static
            void
            cloneNodeWrapper(AbstractDOMElement::Ptr*, bool*, ChromiumDOMElement*);

            static
            void
            getAttributeWrapper(const std::string*, std::string*, ChromiumDOMElement*);

            static
            void
            setAttributeWrapper(const std::string*, const std::string*, ChromiumDOMElement* target);

            static
            void
            getElementsByTagNameWrapper(const std::string*, std::vector<AbstractDOMElement::Ptr>*, ChromiumDOMElement* target);

            static
            void
            styleResultWrapper(const std::string*, std::string*, ChromiumDOMElement* target);

            static
            void
            styleValueWrapper(const std::string*, const std::string*, ChromiumDOMElement* target);

            void
            static
            addEventListenerWrapper(std::string*, ChromiumDOMElement* target);

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
            id(const std::string& newId)
            {
                setProperty<const std::string&>("id", newId);
            }

            inline
            std::string
            className()
            {
                return getProperty<std::string>("className");
            }

            inline
            void
            className(const std::string& newClass)
            {
                setProperty<const std::string&>("className", newClass);
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
            textContent(const std::string& content)
            {
                setProperty<const std::string&>("textContent", content);
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
                setProperty<const std::string&>("value", v);
            }

            inline
            std::string
            innerHTML()
            {
                return getProperty<std::string>("innerHTML");
            }

            inline
            void
            innerHTML(const std::string& html)
            {
                setProperty<const std::string&>("innerHTML", html);
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
            getAttribute(const std::string& name);

			void
            setAttribute(const std::string& name, const std::string& value);

			std::vector<AbstractDOMElement::Ptr>
			getElementsByTagName(const std::string& tagName);


			std::string
			style(const std::string& name);

			void
			style(const std::string& name, const std::string& value);

			DOMMouseEventSignal
			onclick();

			DOMMouseEventSignal
			onmousedown();

			DOMMouseEventSignal
			onmousemove();

			DOMMouseEventSignal
			onmouseup();

			DOMMouseEventSignal
			onmouseout();

			DOMMouseEventSignal
			onmouseover();

            DOMEventSignal
            onchange();

            DOMEventSignal
            oninput();

            DOMTouchEventSignal
            ontouchstart();

            DOMTouchEventSignal
            ontouchend();

            DOMTouchEventSignal
            ontouchmove();

			static
			void
			addFunction(std::function<void()>);

			static
			void
			update();

		private:

			void
			addEventListener(std::string type);

		private:

			static
			std::map<CefRefPtr<CefV8Value>, Ptr> _v8NodeToElement;

			static
			std::map<Ptr, CefRefPtr<CefV8Value>> _elementToV8Object;
            
            DOMEventSignal       _onchange;
            DOMEventSignal       _oninput;

			DOMMouseEventSignal  _onclick;
			DOMMouseEventSignal  _onmousedown;
			DOMMouseEventSignal  _onmousemove;
			DOMMouseEventSignal  _onmouseup;
			
			DOMMouseEventSignal  _onmouseover;
			DOMMouseEventSignal  _onmouseout;
            
            DOMTouchEventSignal  _ontouchstart;
            DOMTouchEventSignal  _ontouchend;
            DOMTouchEventSignal  _ontouchmove;

            bool _onchangeCallbackSet;
            bool _oninputCallbackSet;

			bool _onclickCallbackSet;
			bool _onmousedownCallbackSet;
			bool _onmousemoveCallbackSet;
			bool _onmouseupCallbackSet;

			bool _onmouseoverCallbackSet;
			bool _onmouseoutCallbackSet;

			bool _ontouchstartCallbackSet;
			bool _ontouchendCallbackSet;
			bool _ontouchmoveCallbackSet;

			CefRefPtr<ChromiumDOMElementV8Handler> _v8Handler;

			static std::list<std::function<void()>>	_functionList;
			static std::mutex _functionListMutex;

			bool _cleared;
		};
	}
}
#endif