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
#include "minko/Common.hpp"
#include "emscripten/dom/EmscriptenDOMEvent.hpp"

using namespace minko;
using namespace minko::dom;
using namespace emscripten;
using namespace emscripten::dom;

void
EmscriptenDOMEvent::preventDefault()
{
}

void
EmscriptenDOMEvent::stopPropagation()
{
}

std::string
EmscriptenDOMEvent::type()
{
	return "";
}

minko::dom::AbstractDOMElement::Ptr
EmscriptenDOMEvent::target()
{
	return nullptr;
}

int
EmscriptenDOMEvent::clientX()
{
	return 0;
}

int
EmscriptenDOMEvent::clientY()
{
	return 0;
}

int
EmscriptenDOMEvent::pageX()
{
	return 0;
}

int
EmscriptenDOMEvent::pageY()
{
	return 0;
}

int
EmscriptenDOMEvent::layerX()
{
	return 0;
}

int
EmscriptenDOMEvent::layerY()
{
	return 0;
}

int
EmscriptenDOMEvent::screenX()
{
	return 0;
}

int
EmscriptenDOMEvent::screenY()
{
	return 0;
}
#endif