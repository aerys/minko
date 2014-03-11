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
#include "minko/dom/AbstractDOM.hpp"
#include "minko/dom/AbstractDOMEngine.hpp"

namespace emscripten
{
	namespace dom
	{
		class EmscriptenDOMEngine : public minko::dom::AbstractDOMEngine
		{
		public:
			typedef std::shared_ptr<EmscriptenDOMEngine> Ptr;

		private:
			EmscriptenDOMEngine();

		public:

			~EmscriptenDOMEngine()
			{
			}

			minko::dom::AbstractDOM::Ptr
			load(std::string uri);

			static
			Ptr
			create();

			void
			clear();

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr
			onload();

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr
			onmessage();
		};
	}
}
#endif