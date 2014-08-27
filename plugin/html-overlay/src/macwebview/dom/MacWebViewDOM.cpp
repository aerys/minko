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
#include "macwebview/dom/MacWebViewDOM.hpp"
#include "macwebview/dom/MacWebViewDOMEngine.hpp"

using namespace minko;
using namespace minko::dom;
using namespace macwebview;
using namespace macwebview::dom;

MacWebViewDOM::MacWebViewDOM(std::string jsAccessor) :
	_initialized(false),
	_onload(Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(Signal<AbstractDOM::Ptr, std::string>::create()),
	_jsAccessor(jsAccessor),
    _engine(nullptr)
{
}

MacWebViewDOM::Ptr
MacWebViewDOM::create(std::string jsAccessor, std::shared_ptr<MacWebViewDOMEngine> engine)
{
	Ptr dom(new MacWebViewDOM(jsAccessor));
    dom->_engine = engine;
    
	return dom;
}

std::string replaceAll(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    
    while((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    
    return str;
}

void
MacWebViewDOM::sendMessage(std::string message, bool async)
{
    message = replaceAll(message, "\\", "\\\\");
    message = replaceAll(message, "'", "\\'");

	std::string eval = "if (" + _jsAccessor + ".window.Minko.onmessage) " + _jsAccessor + ".window.Minko.onmessage('" + message + "');";

    runScript(eval);
}

void
MacWebViewDOM::eval(std::string message, bool async)
{
    std::string ev = _jsAccessor + ".window.eval('" + message + "')";
    _engine->eval(ev);
}

std::vector<AbstractDOMElement::Ptr>
MacWebViewDOM::getElementList(std::string expression)
{
    std::vector<minko::dom::AbstractDOMElement::Ptr> l;
    
    expression = "Minko.tmpElements = " + expression;
    
    runScript(expression);
    
    expression = "(Minko.tmpElements.length)";
    int numElements = runScriptInt(expression.c_str());
    
    for(int i = 0; i < numElements; ++i)
        l.push_back(MacWebViewDOMElement::getDOMElement("Minko.tmpElements[" + std::to_string(i) + "]", _engine));
    
    return l;
}

AbstractDOMElement::Ptr
MacWebViewDOM::createElement(std::string element)
{
	std::string eval = "Minko.tmpElement = " + _jsAccessor + ".document.createElement('" + element + "');";

	runScript(eval);

	return MacWebViewDOMElement::getDOMElement("Minko.tmpElement", _engine);
}

AbstractDOMElement::Ptr
MacWebViewDOM::getElementById(std::string id)
{
	std::string eval = "Minko.tmpElement = " + _jsAccessor + ".document.getElementById('" + id + "');";

	runScript(eval);

	return MacWebViewDOMElement::getDOMElement("Minko.tmpElement", _engine);
}

std::vector<AbstractDOMElement::Ptr>
MacWebViewDOM::getElementsByClassName(std::string className)
{
	return getElementList(_jsAccessor + ".document.getElementsByClassName('" + className + "')");
}

std::vector<AbstractDOMElement::Ptr>
MacWebViewDOM::getElementsByTagName(std::string tagName)
{
	return getElementList(_jsAccessor + ".document.getElementsByTagName('" + tagName + "')");
}

AbstractDOMElement::Ptr
MacWebViewDOM::document()
{
	return _document;
}

AbstractDOMElement::Ptr
MacWebViewDOM::body()
{
	return _body;
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
MacWebViewDOM::onload()
{
	return _onload;
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
MacWebViewDOM::onmessage()
{
	return _onmessage;
}

std::string
MacWebViewDOM::fileName()
{
	std::string completeUrl = fullUrl();
	int i = completeUrl.find_last_of('/');

	return completeUrl.substr(i + 1);
}

std::string
MacWebViewDOM::fullUrl()
{
	std::string eval = "(Minko.iframeElement.src)";
    
    std::string result = runScriptString(eval);
    
	return result;
}

bool
MacWebViewDOM::isMain()
{
	return true;
}

void
MacWebViewDOM::initialized(bool v)
{
	if (!_initialized && v)
	{
        runScript(_jsAccessor + " = {};");
        
		std::string eval = "";
		eval += _jsAccessor + ".window		= Minko.iframeElement.contentWindow;\n";
		eval += _jsAccessor + ".document	= Minko.iframeElement.contentDocument;\n";
		eval += _jsAccessor + ".body		= Minko.iframeElement.contentDocument.body;\n";
		
        runScript(eval);

		_document	= MacWebViewDOMElement::create(_jsAccessor + ".document", _engine);
		_body		= MacWebViewDOMElement::create(_jsAccessor + ".body", _engine);
	}

	_initialized = v;
}

bool
MacWebViewDOM::initialized()
{
	return _initialized;
}

void
MacWebViewDOM::runScript(std::string script)
{
    _engine->eval(script);
}

std::string
MacWebViewDOM::runScriptString(std::string script)
{
    return _engine->eval(script);
}

int
MacWebViewDOM::runScriptInt(std::string script)
{
    return atoi(_engine->eval(script).c_str());
}