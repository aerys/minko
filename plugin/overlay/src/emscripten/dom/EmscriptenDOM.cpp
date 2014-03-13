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
#include "emscripten/dom/EmscriptenDOM.hpp"
#include "emscripten/emscripten.h"

using namespace minko;
using namespace minko::dom;
using namespace emscripten;
using namespace emscripten::dom;

int
EmscriptenDOM::elementUid = 0;

EmscriptenDOM::EmscriptenDOM() :
	_document(EmscriptenDOMElement::create("Minko.document")),
	_body(EmscriptenDOMElement::create("Minko.document.body")),
	_initialized(false),
	_onload(Signal<AbstractDOM::Ptr, std::string>::create())
{
}

EmscriptenDOM::Ptr
EmscriptenDOM::create()
{
	EmscriptenDOM::Ptr dom(new EmscriptenDOM());
	return dom;
}

std::string
EmscriptenDOM::getNewElementName()
{
	std::string name = "Minko.element" + std::to_string(elementUid++);
	return name;
}

std::list<AbstractDOMElement::Ptr>
EmscriptenDOM::getElementList(std::string expression)
{
	std::list<minko::dom::AbstractDOMElement::Ptr> l;

	std::string jsElements = EmscriptenDOM::getNewElementName();
	expression = jsElements + " = " + expression;

	emscripten_run_script(expression.c_str());

	expression = "(" + jsElements + ".length)";
	int numElements = emscripten_run_script_int(expression.c_str());

	for(int i = 0; i < numElements; ++i)
		l.push_back(EmscriptenDOMElement::create(jsElements + "[" + std::to_string(i) + "]"));

	return l;
}

AbstractDOMElement::Ptr
EmscriptenDOM::createElement(std::string element)
{
	std::string newElement = getNewElementName();
	std::string eval = newElement + " = Minko.document.createElement('" + element + "');";

	emscripten_run_script(eval.c_str());

	return EmscriptenDOMElement::create(newElement);
}

AbstractDOMElement::Ptr
EmscriptenDOM::getElementById(std::string id)
{
	std::string newElement = getNewElementName();
	std::string eval = newElement + " = Minko.document.getElementById('" + id + "');";

	emscripten_run_script(eval.c_str());

	return EmscriptenDOMElement::create(newElement);
}

std::list<AbstractDOMElement::Ptr>
EmscriptenDOM::getElementsByClassName(std::string className)
{
	return getElementList("Minko.document.getElementsByClassName('" + className + "')");
}

std::list<AbstractDOMElement::Ptr>
EmscriptenDOM::getElementsByTagName(std::string tagName)
{
	return getElementList("Minko.document.getElementsByTagName('" + tagName + "')");
}

AbstractDOMElement::Ptr
EmscriptenDOM::document()
{
	return _document;
}

AbstractDOMElement::Ptr
EmscriptenDOM::body()
{
	return _body;
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
EmscriptenDOM::onload()
{
	return _onload;
}

std::string
EmscriptenDOM::fileName()
{
	std::string completeUrl = fullUrl();
	int i = completeUrl.find_last_of('/');

	return completeUrl.substr(i + 1);
}

std::string
EmscriptenDOM::fullUrl()
{
	std::string eval = "(Minko.document.location)";

	char* result = emscripten_run_script_string(eval.c_str());

	return std::string(result);
}

bool
EmscriptenDOM::isMain()
{
	return true;
}

void
EmscriptenDOM::initialized(bool v)
{
	_initialized = v;
}

bool
EmscriptenDOM::initialized()
{
	return _initialized;
}
#endif