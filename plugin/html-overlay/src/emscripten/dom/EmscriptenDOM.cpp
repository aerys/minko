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

#include "minko/Common.hpp"
#include "emscripten/dom/EmscriptenDOM.hpp"
#include "emscripten/emscripten.h"

using namespace minko;
using namespace minko::dom;
using namespace emscripten;
using namespace emscripten::dom;

EmscriptenDOM::EmscriptenDOM(const std::string& jsAccessor) :
	_initialized(false),
	_onload(Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(Signal<AbstractDOM::Ptr, std::string>::create()),
	_jsAccessor(jsAccessor)
{
}

EmscriptenDOM::Ptr
EmscriptenDOM::create(const std::string& jsAccessor)
{
	EmscriptenDOM::Ptr dom(new EmscriptenDOM(jsAccessor));
	return dom;
}

void
EmscriptenDOM::sendMessage(const std::string& message, bool async)
{
	std::string eval = "if (" + _jsAccessor + " !== undefined && " + _jsAccessor + ".window !== undefined && " + _jsAccessor + ".window.Minko !== undefined && " + _jsAccessor + ".window.Minko.onmessage !== undefined) " + _jsAccessor + ".window.Minko.onmessage('" + message + "');";
	//if (!async)
		emscripten_run_script(eval.c_str());
	//else
	//	emscripten_async_run_script("console.log('toto'); if (" + _jsAccessor + ".window.Minko.onmessage) " + _jsAccessor + ".window.Minko.onmessage('" + message + "');", 1);
}

void
EmscriptenDOM::eval(const std::string& message, bool async)
{
	//if (!async)
		std::string ev = _jsAccessor + ".window.eval('" + message + "')";
		emscripten_run_script(ev.c_str());
	//else
	//	emscripten_async_run_script(message.c_str(), 1);
}

std::vector<AbstractDOMElement::Ptr>
EmscriptenDOM::getElementList(const std::string& expression)
{
	std::vector<minko::dom::AbstractDOMElement::Ptr> l;

	std::string e = "Minko.tmpElements = " + expression;

	emscripten_run_script(e.c_str());

	e = "(Minko.tmpElements.length)";
	int numElements = emscripten_run_script_int(e.c_str());

	for(int i = 0; i < numElements; ++i)
		l.push_back(EmscriptenDOMElement::getDOMElement("Minko.tmpElements[" + std::to_string(i) + "]"));

	return l;
}

AbstractDOMElement::Ptr
EmscriptenDOM::createElement(const std::string& element)
{
	std::string eval = "Minko.tmpElement = " + _jsAccessor + ".document.createElement('" + element + "');";

	emscripten_run_script(eval.c_str());

	return EmscriptenDOMElement::getDOMElement("Minko.tmpElement");
}

AbstractDOMElement::Ptr
EmscriptenDOM::getElementById(const std::string& id)
{
	std::string eval = "Minko.tmpElement = " + _jsAccessor + ".document.getElementById('" + id + "');";

	emscripten_run_script(eval.c_str());

	return EmscriptenDOMElement::getDOMElement("Minko.tmpElement");
}

std::vector<AbstractDOMElement::Ptr>
EmscriptenDOM::getElementsByClassName(const std::string& className)
{
	return getElementList(_jsAccessor + ".document.getElementsByClassName('" + className + "')");
}

std::vector<AbstractDOMElement::Ptr>
EmscriptenDOM::getElementsByTagName(const std::string& tagName)
{
	return getElementList(_jsAccessor + ".document.getElementsByTagName('" + tagName + "')");
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

Signal<AbstractDOM::Ptr, std::string>::Ptr
EmscriptenDOM::onmessage()
{
	return _onmessage;
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
	std::string eval = "(" + _jsAccessor + ".document.location)";

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
	if (!_initialized && v)
	{
		std::string eval = "";
		eval += _jsAccessor + ".window		= Minko.window;\n";
		eval += _jsAccessor + ".document	= Minko.document;\n";
		eval += _jsAccessor + ".body		= Minko.document.body;\n";
		emscripten_run_script(eval.c_str());

		_document	= EmscriptenDOMElement::create(_jsAccessor + ".document");
		_body		= EmscriptenDOMElement::create(_jsAccessor + ".body");
	}

	_initialized = v;
}

bool
EmscriptenDOM::initialized()
{
	return _initialized;
}
