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
#include "osxwebview/dom/OSXWebViewDOM.hpp"
#include "osxwebview/dom/OSXWebViewDOMEngine.hpp"

using namespace minko;
using namespace minko::dom;
using namespace osxwebview;
using namespace osxwebview::dom;

OSXWebViewDOM::OSXWebViewDOM(std::string jsAccessor) :
	_initialized(false),
	_onload(Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(Signal<AbstractDOM::Ptr, std::string>::create()),
	_jsAccessor(jsAccessor),
    _engine(nullptr)
{
}

OSXWebViewDOM::Ptr
OSXWebViewDOM::create(std::string jsAccessor, std::shared_ptr<OSXWebViewDOMEngine> engine)
{
	Ptr dom(new OSXWebViewDOM(jsAccessor));
    dom->_engine = engine;
    
	return dom;
}

void
OSXWebViewDOM::sendMessage(std::string message, bool async)
{
	std::string eval = "if (" + _jsAccessor + ".window.Minko.onmessage) " + _jsAccessor + ".window.Minko.onmessage('" + message + "');";

    runScript(eval);
}

void
OSXWebViewDOM::eval(std::string message, bool async)
{
    std::string ev = _jsAccessor + ".window.eval('" + message + "')";
    _engine->eval(ev);
}

std::vector<AbstractDOMElement::Ptr>
OSXWebViewDOM::getElementList(std::string expression)
{
    std::vector<minko::dom::AbstractDOMElement::Ptr> l;
    
    expression = "Minko.tmpElements = " + expression;
    
    runScript(expression);
    
    expression = "(Minko.tmpElements.length)";
    int numElements = runScriptInt(expression.c_str());
    
    for(int i = 0; i < numElements; ++i)
        l.push_back(OSXWebViewDOMElement::getDOMElement("Minko.tmpElements[" + std::to_string(i) + "]", _engine));
    
    return l;
}

AbstractDOMElement::Ptr
OSXWebViewDOM::createElement(std::string element)
{
	std::string eval = "Minko.tmpElement = " + _jsAccessor + ".document.createElement('" + element + "');";

	runScript(eval);

	return OSXWebViewDOMElement::getDOMElement("Minko.tmpElement", _engine);
}

AbstractDOMElement::Ptr
OSXWebViewDOM::getElementById(std::string id)
{
	std::string eval = "Minko.tmpElement = " + _jsAccessor + ".document.getElementById('" + id + "');";

	runScript(eval);

	return OSXWebViewDOMElement::getDOMElement("Minko.tmpElement", _engine);
}

std::vector<AbstractDOMElement::Ptr>
OSXWebViewDOM::getElementsByClassName(std::string className)
{
	return getElementList(_jsAccessor + ".document.getElementsByClassName('" + className + "')");
}

std::vector<AbstractDOMElement::Ptr>
OSXWebViewDOM::getElementsByTagName(std::string tagName)
{
	return getElementList(_jsAccessor + ".document.getElementsByTagName('" + tagName + "')");
}

AbstractDOMElement::Ptr
OSXWebViewDOM::document()
{
	return _document;
}

AbstractDOMElement::Ptr
OSXWebViewDOM::body()
{
	return _body;
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
OSXWebViewDOM::onload()
{
	return _onload;
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
OSXWebViewDOM::onmessage()
{
	return _onmessage;
}

std::string
OSXWebViewDOM::fileName()
{
	std::string completeUrl = fullUrl();
	int i = completeUrl.find_last_of('/');

	return completeUrl.substr(i + 1);
}

std::string
OSXWebViewDOM::fullUrl()
{
	std::string eval = "(Minko.iframeElement.src)";
    
    std::string result = runScriptString(eval);
    
	return result;
}

bool
OSXWebViewDOM::isMain()
{
	return true;
}

void
OSXWebViewDOM::initialized(bool v)
{
	if (!_initialized && v)
	{
        runScript(_jsAccessor + " = {};");
        
		std::string eval = "";
		eval += _jsAccessor + ".window		= Minko.iframeElement.contentWindow;\n";
		eval += _jsAccessor + ".document	= Minko.iframeElement.contentDocument;\n";
		eval += _jsAccessor + ".body		= Minko.iframeElement.contentDocument.body;\n";
		
        runScript(eval);

		_document	= OSXWebViewDOMElement::create(_jsAccessor + ".document", _engine);
		_body		= OSXWebViewDOMElement::create(_jsAccessor + ".body", _engine);
	}

	_initialized = v;
}

bool
OSXWebViewDOM::initialized()
{
	return _initialized;
}

void
OSXWebViewDOM::runScript(std::string script)
{
    _engine->eval(script);
}

std::string
OSXWebViewDOM::runScriptString(std::string script)
{
    return _engine->eval(script);
}

int
OSXWebViewDOM::runScriptInt(std::string script)
{
    return atoi(_engine->eval(script).c_str());
}