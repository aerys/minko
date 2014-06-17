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
		class AbstractDOMEvent;

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
			id(std::string) = 0;

			virtual
			std::string
			className() = 0;

			virtual
			void
			className(std::string) = 0;

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
			void
			textContent(std::string) = 0;

			virtual
			std::string
			innerHTML() = 0;

			virtual
			void
			innerHTML(std::string) = 0;

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
			getAttribute(std::string name) = 0;

			virtual
			void
			setAttribute(std::string name, std::string value) = 0;

			virtual
			std::vector<Ptr>
			getElementsByTagName(std::string tagName) = 0;


			virtual
			std::string
			style(std::string name) = 0;

			virtual
			void
			style(std::string name, std::string value) = 0;

			virtual
			Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
			onclick() = 0;

			virtual
			Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
			onmousedown() = 0;

			virtual
			Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
			onmousemove() = 0;

			virtual
			Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
			onmouseup() = 0;

			virtual
			Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
			onmouseout() = 0;

			virtual
			Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
			onmouseover() = 0;
            
            virtual
			Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
			ontouchdown() = 0;
            
            virtual
			Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
			ontouchup() = 0;
            
            virtual
			Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
			ontouchmotion() = 0;

		private:

		};
	}
}