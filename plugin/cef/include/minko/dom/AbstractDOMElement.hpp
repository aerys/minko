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
#include "AbstractDOMEvent.hpp"

namespace minko
{
	namespace dom
	{
		class AbstractDOMElement
		{
		public:
			typedef std::shared_ptr<AbstractDOMElement> Ptr;

			virtual
			std::string
			id() = 0;

			virtual
			std::string
			className() = 0;

			virtual
			std::string
			tagName() = 0;


			virtual
			Ptr
			parentNode() = 0;

			virtual
			std::list<Ptr>
			childNodes() = 0;


			virtual
			Ptr
			appendChild(Ptr) = 0;

			virtual
			Ptr
			removeChild(Ptr) = 0;


			virtual
			Ptr
			insertBefore(Ptr) = 0;

			virtual
			Ptr
			cloneNode(bool deep) = 0;


			virtual
			std::string
			getAttribute(std::string name) = 0;

			virtual
			void
			setAttribute(std::string name, std::string value) = 0;

			virtual
			std::list<Ptr>
			getElementsByTagName(std::string tagName) = 0;


			virtual
			std::string
			style(std::string name) = 0;

			virtual
			std::string
			style(std::string name, std::string value) = 0;

			virtual
			Signal<AbstractDOMEvent::Ptr>::Ptr
			onclick() = 0;

			virtual
			Signal<AbstractDOMEvent::Ptr>::Ptr
			onmousedown() = 0;

			virtual
			Signal<AbstractDOMEvent::Ptr>::Ptr
			onmousemove() = 0;

			virtual
			Signal<AbstractDOMEvent::Ptr>::Ptr
			onmouseup() = 0;

			virtual
			Signal<AbstractDOMEvent::Ptr>::Ptr
			onmouseout() = 0;

			virtual
			Signal<AbstractDOMEvent::Ptr>::Ptr
			onmouseover() = 0;

		private:

		};
	}
}