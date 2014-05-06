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
#include "minko/component/SceneManager.hpp"
#include "minko/AbstractCanvas.hpp"
#include "AbstractDOMElement.hpp"

namespace minko
{
	namespace dom
	{
		class AbstractDOM
		{
		public:
			typedef std::shared_ptr<AbstractDOM> Ptr;

			virtual
			void
			sendMessage(std::string, bool async = true) = 0;

			virtual
			void
			eval(std::string, bool async = true) = 0;

			virtual
			AbstractDOMElement::Ptr
			createElement(std::string) = 0;

			virtual
			AbstractDOMElement::Ptr
			getElementById(std::string) = 0;

			virtual
			std::vector<AbstractDOMElement::Ptr>
			getElementsByClassName(std::string) = 0;

			virtual
			std::vector<AbstractDOMElement::Ptr>
			getElementsByTagName(std::string) = 0;

			virtual
			AbstractDOMElement::Ptr
			document() = 0;

			virtual
			AbstractDOMElement::Ptr
			body() = 0;

			virtual
			Signal<Ptr, std::string>::Ptr
			onload() = 0;

			virtual
			std::string
			fileName() = 0;

			virtual
			std::string
			fullUrl() = 0;

			virtual
			bool
			isMain() = 0;
		};
	}
}