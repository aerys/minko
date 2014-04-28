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
#include "minko/component/SceneManager.hpp"
#include "minko/AbstractCanvas.hpp"
#include "minko/dom/AbstractDOMElement.hpp"
#include "EmscriptenDOMElement.hpp"
#include "minko/dom/AbstractDOM.hpp"

namespace emscripten
{
	namespace dom
	{
		class EmscriptenDOM : public minko::dom::AbstractDOM
		{
		public:
			typedef std::shared_ptr<EmscriptenDOM> Ptr;

		private:
			EmscriptenDOM(std::string);

		public:
			static
			Ptr
			create(std::string);

			void
			sendMessage(std::string, bool async);

			minko::dom::AbstractDOMElement::Ptr
			createElement(std::string);

			minko::dom::AbstractDOMElement::Ptr
			getElementById(std::string);

			std::list<minko::dom::AbstractDOMElement::Ptr>
			getElementsByClassName(std::string);

			std::list<minko::dom::AbstractDOMElement::Ptr>
			getElementsByTagName(std::string);

			minko::dom::AbstractDOMElement::Ptr
			document();

			minko::dom::AbstractDOMElement::Ptr
			body();

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr
			onload();

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr
			onmessage();

			std::string
			fileName();

			std::string
			fullUrl();

			bool
			isMain();

			bool
			initialized();

			void
			initialized(bool);

			static
			std::list<minko::dom::AbstractDOMElement::Ptr>
			getElementList(std::string);

		private:
			bool _initialized;

			std::string _jsAccessor;

			EmscriptenDOMElement::Ptr _document;
			EmscriptenDOMElement::Ptr _body;

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr _onload;
			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr _onmessage;
		};
	}
}
#endif