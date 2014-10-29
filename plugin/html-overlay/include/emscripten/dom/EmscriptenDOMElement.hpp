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

#if defined(EMSCRIPTEN)
#pragma once

#include "minko/Common.hpp"
#include "minko/Signal.hpp"
#include "minko/dom/AbstractDOMElement.hpp"
#include "minko/dom/AbstractDOMEvent.hpp"
#include "minko/dom/AbstractDOMMouseEvent.hpp"
#include "minko/dom/AbstractDOMTouchEvent.hpp"

namespace emscripten
{
	namespace dom
	{

		class EmscriptenDOMElement : public minko::dom::AbstractDOMElement,
			public std::enable_shared_from_this<EmscriptenDOMElement>
		{
		public:
			typedef std::shared_ptr<EmscriptenDOMElement> Ptr;

		private:
            
            typedef std::shared_ptr<minko::Signal<std::shared_ptr<minko::dom::AbstractDOMEvent>>>       DOMEventSignal;
            typedef std::shared_ptr<minko::Signal<std::shared_ptr<minko::dom::AbstractDOMMouseEvent>>>  DOMMouseEventSignal;
            typedef std::shared_ptr<minko::Signal<std::shared_ptr<minko::dom::AbstractDOMTouchEvent>>>  DOMTouchEventSignal;
            
			EmscriptenDOMElement(const std::string& jsAccessor);

		public:
			~EmscriptenDOMElement()
			{
			};

			static
			Ptr
			getDOMElement(const std::string& jsElement);

			std::string
			getJavascriptAccessor();

			static
			Ptr
			create(const std::string& javascriptAccessor);

			std::string
			id();

			void
			id(const std::string&);

			std::string
			value();

			void
			value(const std::string& value);

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
            value();

            void
            value(const std::string& value);

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

			void
			update();

		private:
			void
			addEventListener(const std::string&);

		public:
			static
			std::list<EmscriptenDOMElement::Ptr> domElements;

		private:
			static
			int
			_elementUid;

			static
			std::map<std::string,Ptr> _accessorToElement;

			std::string _jsAccessor;

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

			bool _onclickSet;
			bool _onmousedownSet;
			bool _onmousemoveSet;
			bool _onmouseupSet;

			bool _onmouseoverSet;
			bool _onmouseoutSet;

			bool _onchangeSet;
			bool _oninputSet;

			bool _ontouchstartSet;
			bool _ontouchendSet;
			bool _ontouchmoveSet;
		};
	}
}
#endif
