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
#include "minko/dom/AbstractDOMElement.hpp"
#include "minko/dom/AbstractDOMMouseEvent.hpp"
#include "minko/dom/AbstractDOMTouchEvent.hpp"

namespace android
{
	namespace dom
	{
        class AndroidWebViewDOMEngine;

		class AndroidWebViewDOMElement : public minko::dom::AbstractDOMElement,
			public std::enable_shared_from_this<AndroidWebViewDOMElement>
		{
		public:
			typedef std::shared_ptr<AndroidWebViewDOMElement> Ptr;

		private:
			AndroidWebViewDOMElement(const std::string& jsAccessor);

		public:
			~AndroidWebViewDOMElement()
			{
			};

			static
			Ptr
			getDOMElement(const std::string& jsElement, std::shared_ptr<AndroidWebViewDOMEngine> engine);

			std::string
			getJavascriptAccessor();

			static
			Ptr
			create(const std::string& javascriptAccessor, std::shared_ptr<AndroidWebViewDOMEngine> engine);

			std::string
			id();

			void
			id(const std::string&);

			std::string
			className();

			void
			className(const std::string&);

			std::string
			tagName();

			minko::dom::AbstractDOMElement::Ptr
			parentNode();

			std::vector<minko::dom::AbstractDOMElement::Ptr>
			childNodes();

			std::string
			textContent();

			void
			textContent(const std::string&);

			std::string
			innerHTML();

			void
			innerHTML(const std::string&);

			minko::dom::AbstractDOMElement::Ptr
			appendChild(minko::dom::AbstractDOMElement::Ptr);

			minko::dom::AbstractDOMElement::Ptr
			removeChild(minko::dom::AbstractDOMElement::Ptr);

			minko::dom::AbstractDOMElement::Ptr
			insertBefore(minko::dom::AbstractDOMElement::Ptr, minko::dom::AbstractDOMElement::Ptr);

			minko::dom::AbstractDOMElement::Ptr
			cloneNode(bool deep = true);

			std::string
			getAttribute(const std::string& name);

			void
			setAttribute(const std::string& name, const std::string& value);

			std::vector<minko::dom::AbstractDOMElement::Ptr>
			getElementsByTagName(const std::string& tagName);

			std::string
			value();

			void
			value(const std::string&);

			std::string
			style(const std::string& name);

			void
			style(const std::string& name, const std::string& value);

            // Events
            minko::Signal<std::shared_ptr<minko::dom::AbstractDOMMouseEvent>>::Ptr
			onclick();

			minko::Signal<std::shared_ptr<minko::dom::AbstractDOMMouseEvent>>::Ptr
			onmousedown();

			minko::Signal<std::shared_ptr<minko::dom::AbstractDOMMouseEvent>>::Ptr
			onmousemove();

			minko::Signal<std::shared_ptr<minko::dom::AbstractDOMMouseEvent>>::Ptr
			onmouseup();

			minko::Signal<std::shared_ptr<minko::dom::AbstractDOMMouseEvent>>::Ptr
			onmouseout();

			minko::Signal<std::shared_ptr<minko::dom::AbstractDOMMouseEvent>>::Ptr
			onmouseover();

            minko::Signal<std::shared_ptr<minko::dom::AbstractDOMTouchEvent>>::Ptr
            ontouchstart();

            minko::Signal<std::shared_ptr<minko::dom::AbstractDOMTouchEvent>>::Ptr
            ontouchend();

            minko::Signal<std::shared_ptr<minko::dom::AbstractDOMTouchEvent>>::Ptr
            ontouchmove();

            minko::Signal<std::shared_ptr<minko::dom::AbstractDOMEvent>>::Ptr
            onchange();

            minko::Signal<std::shared_ptr<minko::dom::AbstractDOMEvent>>::Ptr
            oninput();
            
            void
            update();
		private:
			void
			addEventListener(const std::string&);
            
            void
            initialize(std::shared_ptr<AndroidWebViewDOMEngine> engine);

		public:
			static
			std::list<AndroidWebViewDOMElement::Ptr> domElements;

		private:
			static
			int
			_elementUid;
			
			static
			std::map<std::string,Ptr> _accessorToElement;

			static
			std::mutex _accessorToElementMutex;

			std::string _jsAccessor;

            // Events
			minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Ptr _onclick;
			minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Ptr _onmousedown;
			minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Ptr _onmousemove;
			minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Ptr _onmouseup;

            minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Ptr _onmouseout;
            minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Ptr _onmouseover;

            minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr _oninput;
            minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::Ptr _onchange;
            
            bool _onclickSet;
            bool _onmousedownSet;
            bool _onmousemoveSet;
            bool _onmouseupSet;
            
            bool _onmouseoverSet;
            bool _onmouseoutSet;
            
            minko::Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Ptr _ontouchstart;
            minko::Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Ptr _ontouchend;
            minko::Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Ptr _ontouchmove;
            
            bool _ontouchstartSet;
            bool _ontouchendSet;
            bool _ontouchmoveSet;

            bool _oninputSet;
            bool _onchangeSet;
            
            std::shared_ptr<AndroidWebViewDOMEngine> _engine;
		};
	}
}