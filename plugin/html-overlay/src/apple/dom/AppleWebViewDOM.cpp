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
#include "apple/dom/AppleWebViewDOM.hpp"
#include "apple/dom/AppleWebViewDOMEngine.hpp"

using namespace minko;
using namespace minko::dom;
using namespace apple;
using namespace apple::dom;

AppleWebViewDOM::AppleWebViewDOM(const std::string& jsAccessor) :
	_initialized(false),
	_onload(Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(Signal<AbstractDOM::Ptr, std::string>::create()),
	_jsAccessor(jsAccessor),
    _engine(nullptr)
{
}

AppleWebViewDOM::Ptr
AppleWebViewDOM::create(const std::string& jsAccessor, std::shared_ptr<AppleWebViewDOMEngine> engine)
{
	Ptr dom(new AppleWebViewDOM(jsAccessor));
    dom->_engine = engine;
    
	return dom;
}

std::string
replaceAll(std::string str, const std::string& from, const std::string& to)
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
AppleWebViewDOM::sendMessage(const std::string& message, bool async)
{
	std::string m = message;

    m = replaceAll(m, "\\", "\\\\");
    m = replaceAll(m, "'", "\\'");

	std::string eval = "if (" + _jsAccessor + ".window.Minko.dispatchMessage) " + _jsAccessor + ".window.Minko.dispatchMessage('" + m + "');";

    runScript(eval);
}

void
AppleWebViewDOM::eval(const std::string& message, bool async)
{
    std::string ev = _jsAccessor + ".window.eval('" + message + "')";
    _engine->eval(ev);
}

std::vector<AbstractDOMElement::Ptr>
AppleWebViewDOM::getElementList(const std::string& expression)
{
    std::vector<minko::dom::AbstractDOMElement::Ptr> l;
    std::string e = "Minko.tmpElements = " + expression;
    
    runScript(e);
    
    e = "(Minko.tmpElements.length)";

    int numElements = runScriptInt(e.c_str());
    
    for(int i = 0; i < numElements; ++i)
        l.push_back(AppleWebViewDOMElement::getDOMElement("Minko.tmpElements[" + std::to_string(i) + "]", _engine));
    
    return l;
}

AbstractDOMElement::Ptr
AppleWebViewDOM::createElement(const std::string& element)
{
	std::string eval = "Minko.tmpElement = " + _jsAccessor + ".document.createElement('" + element + "');";

	runScript(eval);

	return AppleWebViewDOMElement::getDOMElement("Minko.tmpElement", _engine);
}

AbstractDOMElement::Ptr
AppleWebViewDOM::getElementById(const std::string& id)
{
	std::string eval = "Minko.tmpElement = " + _jsAccessor + ".document.getElementById('" + id + "');";

	runScript(eval);

	return AppleWebViewDOMElement::getDOMElement("Minko.tmpElement", _engine);
}

std::vector<AbstractDOMElement::Ptr>
AppleWebViewDOM::getElementsByClassName(const std::string& className)
{
	return getElementList(_jsAccessor + ".document.getElementsByClassName('" + className + "')");
}

std::vector<AbstractDOMElement::Ptr>
AppleWebViewDOM::getElementsByTagName(const std::string& tagName)
{
	return getElementList(_jsAccessor + ".document.getElementsByTagName('" + tagName + "')");
}

AbstractDOMElement::Ptr
AppleWebViewDOM::document()
{
	return _document;
}

AbstractDOMElement::Ptr
AppleWebViewDOM::body()
{
	return _body;
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
AppleWebViewDOM::onload()
{
	return _onload;
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
AppleWebViewDOM::onmessage()
{
	return _onmessage;
}

std::string
AppleWebViewDOM::fileName()
{
	std::string completeUrl = fullUrl();
	int i = completeUrl.find_last_of('/');

	return completeUrl.substr(i + 1);
}

std::string
AppleWebViewDOM::fullUrl()
{
	std::string eval = "(document.location.href)";
    
    std::string result = runScriptString(eval);
    
	return result;
}

bool
AppleWebViewDOM::isMain()
{
	return true;
}

void
AppleWebViewDOM::initialized(bool v)
{
	if (!_initialized && v)
	{
        runScript(_jsAccessor + " = {};");
        
		std::string eval = "";
		eval += _jsAccessor + ".window		= window;\n";
		eval += _jsAccessor + ".document	= document;\n";
		eval += _jsAccessor + ".body		= document.body;\n";
		
        runScript(eval);

		_document	= AppleWebViewDOMElement::create(_jsAccessor + ".document", _engine);
		_body		= AppleWebViewDOMElement::create(_jsAccessor + ".body", _engine);
	}

	_initialized = v;
}

bool
AppleWebViewDOM::initialized()
{
	return _initialized;
}

void
AppleWebViewDOM::runScript(const std::string& script)
{
    _engine->eval(script);
}

std::string
AppleWebViewDOM::runScriptString(const std::string& script)
{
    return _engine->eval(script);
}

int
AppleWebViewDOM::runScriptInt(const std::string& script)
{
    return atoi(_engine->eval(script).c_str());
}