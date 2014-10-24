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

namespace minko
{
	namespace dom
	{
		struct JSEventData;
		class AbstractDOMEvent;
        class AbstractDOMMouseEvent;
        class AbstractDOMTouchEvent;
        
		class AbstractDOMElement
		{
		public:
			typedef std::shared_ptr<AbstractDOMElement> Ptr;

			virtual
			~AbstractDOMElement()
			{
			};

			virtual
			std::string
			id() = 0;

			virtual
			void
            id(const std::string&) = 0;

			virtual
			std::string
			className() = 0;

			virtual
			void
            className(const std::string&) = 0;

			virtual
			std::string
			tagName() = 0;

			virtual
			Ptr
			parentNode() = 0;

			virtual
			std::vector<Ptr>
			childNodes() = 0;

            virtual
			std::string
			textContent() = 0;

			virtual
			std::string
			value() = 0;

            virtual
            void
            value(const std::string& value) = 0;

			virtual
			void
            textContent(const std::string&) = 0;

			virtual
			std::string
			innerHTML() = 0;

			virtual
			void
            innerHTML(const std::string&) = 0;

			virtual
			Ptr
			appendChild(Ptr) = 0;

			virtual
			Ptr
			removeChild(Ptr) = 0;

			virtual
			Ptr
			insertBefore(Ptr, Ptr) = 0;

			virtual
			Ptr
			cloneNode(bool deep = true) = 0;
            
			virtual
			std::string
			getAttribute(const std::string& name) = 0;

			virtual
			void
            setAttribute(const std::string& name, const std::string& value) = 0;

			virtual
			std::vector<Ptr>
            getElementsByTagName(const std::string& tagName) = 0;

			virtual
			std::string
            style(const std::string& name) = 0;

			virtual
			void
            style(const std::string& name, const std::string& value) = 0;

			virtual
			Signal<JSEventData>::Ptr
			onclick() = 0;

			virtual
			Signal<JSEventData>::Ptr
			onmousedown() = 0;

			virtual
			Signal<JSEventData>::Ptr
			onmousemove() = 0;

			virtual
			Signal<JSEventData>::Ptr
			onmouseup() = 0;

			virtual
			Signal<JSEventData>::Ptr
			onmouseout() = 0;

			virtual
			Signal<JSEventData>::Ptr
			onmouseover() = 0;

            virtual
            Signal<JSEventData>::Ptr
            onchange() = 0;

            virtual
            Signal<JSEventData>::Ptr
            oninput() = 0;

		private:

		};
	}
}