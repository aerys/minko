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
#include "android/dom/AndroidWebViewDOM.hpp"
#include "android/dom/AndroidWebViewDOMEngine.hpp"

using namespace minko;
using namespace minko::dom;
using namespace android;
using namespace android::dom;

AndroidWebViewDOM::AndroidWebViewDOM(std::string jsAccessor) :
	_initialized(false),
	_onload(Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(Signal<AbstractDOM::Ptr, std::string>::create()),
	_jsAccessor(jsAccessor),
    _engine(nullptr)
{
}

AndroidWebViewDOM::Ptr
AndroidWebViewDOM::create(std::string jsAccessor, std::shared_ptr<AndroidWebViewDOMEngine> engine)
{
	Ptr dom(new AndroidWebViewDOM(jsAccessor));
    dom->_engine = engine;
    
	return dom;
}

void
AndroidWebViewDOM::sendMessage(std::string message, bool async)
{
	std::string eval = _jsAccessor + ".window.Minko.sendMessage('" + message + "');";

    runScript(eval);
}

void
AndroidWebViewDOM::eval(std::string message, bool async)
{
    std::string ev = _jsAccessor + ".window.eval('" + message + "')";
    _engine->eval(ev);
}

std::vector<AbstractDOMElement::Ptr>
AndroidWebViewDOM::getElementList(std::string expression)
{
    std::vector<minko::dom::AbstractDOMElement::Ptr> l;
    
    expression = "Minko.tmpElements = " + expression;
    
    runScript(expression);
    
    expression = "(Minko.tmpElements.length)";
    int numElements = runScriptInt(expression.c_str());
    
    for(int i = 0; i < numElements; ++i)
        l.push_back(AndroidWebViewDOMElement::getDOMElement("Minko.tmpElements[" + std::to_string(i) + "]", _engine));
    
    return l;
}

AbstractDOMElement::Ptr
AndroidWebViewDOM::createElement(std::string element)
{
	std::string eval = "Minko.tmpElement = " + _jsAccessor + ".document.createElement('" + element + "');";

	runScript(eval);

	return AndroidWebViewDOMElement::getDOMElement("Minko.tmpElement", _engine);
}

AbstractDOMElement::Ptr
AndroidWebViewDOM::getElementById(std::string id)
{
	std::string eval = "Minko.tmpElement = " + _jsAccessor + ".document.getElementById('" + id + "');";

	runScript(eval);

	return AndroidWebViewDOMElement::getDOMElement("Minko.tmpElement", _engine);
}

std::vector<AbstractDOMElement::Ptr>
AndroidWebViewDOM::getElementsByClassName(std::string className)
{
	return getElementList(_jsAccessor + ".document.getElementsByClassName('" + className + "')");
}

std::vector<AbstractDOMElement::Ptr>
AndroidWebViewDOM::getElementsByTagName(std::string tagName)
{
	return getElementList(_jsAccessor + ".document.getElementsByTagName('" + tagName + "')");
}

AbstractDOMElement::Ptr
AndroidWebViewDOM::document()
{
	return _document;
}

AbstractDOMElement::Ptr
AndroidWebViewDOM::body()
{
	return _body;
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
AndroidWebViewDOM::onload()
{
	return _onload;
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
AndroidWebViewDOM::onmessage()
{
	return _onmessage;
}

std::string
AndroidWebViewDOM::fileName()
{
	std::string completeUrl = fullUrl();
	int i = completeUrl.find_last_of('/');

	return completeUrl.substr(i + 1);
}

std::string
AndroidWebViewDOM::fullUrl()
{
	std::string eval = "(document.location.href)";
    
    std::string result = runScriptString(eval);
    
	return result;
}

bool
AndroidWebViewDOM::isMain()
{
	return true;
}

void
AndroidWebViewDOM::initialized(bool v)
{
	if (!_initialized && v)
	{
        runScript(_jsAccessor + " = {};");

		std::string eval = "";
		eval += _jsAccessor + ".window = window;";
		eval += _jsAccessor + ".document = document;";
		eval += _jsAccessor + ".body= document.body;";
		
        runScript(eval);

		_document = AndroidWebViewDOMElement::create(_jsAccessor + ".document", _engine);
		_body = AndroidWebViewDOMElement::create(_jsAccessor + ".body", _engine);
	}

	_initialized = v;
}

bool
AndroidWebViewDOM::initialized()
{
	return _initialized;
}

void
AndroidWebViewDOM::runScript(std::string script)
{
    _engine->eval(script);
}

std::string
AndroidWebViewDOM::runScriptString(std::string script)
{
    return _engine->eval(script);
}

int
AndroidWebViewDOM::runScriptInt(std::string script)
{
    return atoi(_engine->eval(script).c_str());
}