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
#include "minko/input/Mouse.hpp"
#include "osxwebview/dom/OSXWebViewDOMElement.hpp"
#include "osxwebview/dom/OSXWebViewDOMMouseEvent.hpp"
#include "osxwebview/dom/OSXWebViewDOMEngine.hpp"
#include "osxwebview/dom/OSXWebViewDOM.hpp"
#include "minko/dom/AbstractDOMEvent.hpp"


using namespace minko;
using namespace minko::dom;
using namespace osxwebview;
using namespace osxwebview::dom;


int
OSXWebViewDOMElement::_elementUid = 0;

std::list<OSXWebViewDOMElement::Ptr>
OSXWebViewDOMElement::domElements;

std::map<std::string,OSXWebViewDOMElement::Ptr>
OSXWebViewDOMElement::_accessorToElement;

OSXWebViewDOMElement::OSXWebViewDOMElement(std::string jsAccessor) :
	_jsAccessor(jsAccessor),
	_onclick(Signal<AbstractDOMMouseEvent::Ptr>::create()),
	_onmousedown(Signal<AbstractDOMMouseEvent::Ptr>::create()),
	_onmousemove(Signal<AbstractDOMMouseEvent::Ptr>::create()),
	_onmouseup(Signal<AbstractDOMMouseEvent::Ptr>::create()),
    _onmouseout(Signal<AbstractDOMMouseEvent::Ptr>::create()),
    _onmouseover(Signal<AbstractDOMMouseEvent::Ptr>::create()),
    _onclickSet(false),
    _onmousedownSet(false),
    _onmousemoveSet(false),
    _onmouseupSet(false),
    _onmouseoverSet(false),
    _onmouseoutSet(false),
    _engine(nullptr)
{
}

void
OSXWebViewDOMElement::initialize(std::shared_ptr<OSXWebViewDOMEngine> engine)
{
    _engine = engine;
    
    std::string jsEval = _jsAccessor + ".minkoName = '" + _jsAccessor + "';";
    _engine->eval(jsEval);
}


OSXWebViewDOMElement::Ptr
OSXWebViewDOMElement::getDOMElement(std::string jsElement, std::shared_ptr<OSXWebViewDOMEngine> engine)
{
	std::string js = "if (" + jsElement + ".minkoName) (" + jsElement + ".minkoName); else ('');";
	std::string minkoName = std::string(engine->eval(js.c_str()));

	if (minkoName == "")
	{
		minkoName = "Minko.element" + std::to_string(_elementUid++);

		js = minkoName + " = " + jsElement;
		engine->eval(js);
	}
	else
	{
		auto i = _accessorToElement.find(minkoName);

		if (i != _accessorToElement.end())
			return i->second;
	}

	return create(minkoName, engine);
}

OSXWebViewDOMElement::Ptr
OSXWebViewDOMElement::create(std::string jsAccessor, std::shared_ptr<OSXWebViewDOMEngine> engine)
{
	OSXWebViewDOMElement::Ptr element(new OSXWebViewDOMElement(jsAccessor));
    element->initialize(engine);
    
    domElements.push_back(element);
	_accessorToElement[jsAccessor] = element;
    
	return element;
}

std::string
OSXWebViewDOMElement::getJavascriptAccessor()
{
	return _jsAccessor;
}

std::string
OSXWebViewDOMElement::id()
{
	std::string js = "(" + _jsAccessor + ".id)";
    std::string result = _engine->eval(js);
    
    return result;
}

void
OSXWebViewDOMElement::id(std::string newId)
{
	std::string js = _jsAccessor + ".id = '" + newId + "';";
	_engine->eval(js);
}

std::string
OSXWebViewDOMElement::className()
{
	std::string js = "(" + _jsAccessor + ".className)";
    std::string result = _engine->eval(js);
	
    return result;
}

void
OSXWebViewDOMElement::className(std::string newClassName)
{
	std::string js = _jsAccessor + ".className = '" + newClassName + "';";
	_engine->eval(js);
}

std::string
OSXWebViewDOMElement::tagName()
{
	std::string js = "(" + _jsAccessor + ".tagName)";
	std::string result = _engine->eval(js);
	
    return result;
}

AbstractDOMElement::Ptr
OSXWebViewDOMElement::parentNode()
{
	std::string js = "Minko.tmpElement  = " + _jsAccessor + ".parentNode;";
	_engine->eval(js);

	return OSXWebViewDOMElement::getDOMElement("Minko.tmpElement", _engine);
}

std::vector<minko::dom::AbstractDOMElement::Ptr>
OSXWebViewDOMElement::childNodes()
{
	return (_engine->currentDOM()->getElementList(_jsAccessor + ".childNodes"));
}

std::string
OSXWebViewDOMElement::textContent()
{
	std::string js = "(" + _jsAccessor + ".textContent)";
	std::string result = _engine->eval(js);
	
    return result;
}

void
OSXWebViewDOMElement::textContent(std::string newTextContent)
{
	std::string js = _jsAccessor + ".textContent = '" + newTextContent + "';";
	_engine->eval(js);
}

std::string
OSXWebViewDOMElement::innerHTML()
{
	std::string js = "(" + _jsAccessor + ".innerHTML)";
	std::string result = _engine->eval(js);
	
    return result;
}

void
OSXWebViewDOMElement::innerHTML(std::string newInnerHTML)
{
	std::string js = _jsAccessor + ".innerHTML = '" + newInnerHTML + "';";
	_engine->eval(js);
}

AbstractDOMElement::Ptr
OSXWebViewDOMElement::appendChild(minko::dom::AbstractDOMElement::Ptr newChild)
{
	OSXWebViewDOMElement::Ptr child = std::dynamic_pointer_cast<OSXWebViewDOMElement>(newChild);
	std::string js = _jsAccessor + ".appendChild(" + child->getJavascriptAccessor() + ");";
	_engine->eval(js);

	return shared_from_this();
}

AbstractDOMElement::Ptr
OSXWebViewDOMElement::removeChild(minko::dom::AbstractDOMElement::Ptr childToRemove)
{
	OSXWebViewDOMElement::Ptr child = std::dynamic_pointer_cast<OSXWebViewDOMElement>(childToRemove);
	std::string js = _jsAccessor + ".removeChild(" + child->getJavascriptAccessor() + ");";
	_engine->eval(js);

	return shared_from_this();
}

AbstractDOMElement::Ptr
OSXWebViewDOMElement::insertBefore(minko::dom::AbstractDOMElement::Ptr newChild, minko::dom::AbstractDOMElement::Ptr adjacentNode)
{
	OSXWebViewDOMElement::Ptr child = std::dynamic_pointer_cast<OSXWebViewDOMElement>(newChild);
	OSXWebViewDOMElement::Ptr adjNode = std::dynamic_pointer_cast<OSXWebViewDOMElement>(adjacentNode);
	std::string js = _jsAccessor + ".removeChild(" + child->getJavascriptAccessor() + ", " + adjNode->getJavascriptAccessor() + ");";
	_engine->eval(js);

	return shared_from_this();
}

AbstractDOMElement::Ptr
OSXWebViewDOMElement::cloneNode(bool deep)
{
	std::string js = "Minko.tmpElement = " + _jsAccessor + ".cloneNode(" + (deep ? "true" : "false") + ");";
	_engine->eval(js);

	return OSXWebViewDOMElement::create("Minko.tmpElement", _engine);
}

std::string
OSXWebViewDOMElement::getAttribute(std::string name)
{
	std::string js = "(" + _jsAccessor + ".getAttribute('" + name + "'))";
	std::string result = _engine->eval(js);
	
    return result;
}

void
OSXWebViewDOMElement::setAttribute(std::string name, std::string value)
{
	std::string js = _jsAccessor + ".setAttribute('" + name + "', '" + value + "');";
	_engine->eval(js);
}

std::vector<minko::dom::AbstractDOMElement::Ptr>
OSXWebViewDOMElement::getElementsByTagName(std::string tagName)
{
	return (_engine->currentDOM()->getElementList(_jsAccessor + ".getElementsByTagName('" + tagName + "')"));
}

std::string
OSXWebViewDOMElement::style(std::string name)
{
	std::string js = "(" + _jsAccessor + ".style." + name + ")";
	std::string result = _engine->eval(js);
	
    return result;
}

void
OSXWebViewDOMElement::style(std::string name, std::string value)
{
	std::string js = _jsAccessor + ".style." + name + " = '" + value + "';";
	_engine->eval(js);
}

void
OSXWebViewDOMElement::addEventListener(std::string type)
{
	std::string js = "Minko.addListener(" + _jsAccessor + ", '" + type + "');";

	_engine->eval(js);
}

// Events

Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
OSXWebViewDOMElement::onclick()
{
	if (!_onclickSet)
	{
		addEventListener("click");
		_onclickSet = true;
	}

	return _onclick;
}

Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
OSXWebViewDOMElement::onmousedown()
{
	if (!_onmousedownSet)
	{
		addEventListener("mousedown");
		_onmousedownSet = true;
	}

	return _onmousedown;
}

Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
OSXWebViewDOMElement::onmouseup()
{
    if (!_onmouseupSet)
    {
        addEventListener("mouseup");
        _onmouseupSet = true;
    }
    
    return _onmouseup;
}


Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
OSXWebViewDOMElement::onmousemove()
{
	if (!_onmousemoveSet)
	{
		addEventListener("mousemove");
		_onmousemoveSet = true;
	}
	
	return _onmousemove;
}

Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
OSXWebViewDOMElement::onmouseout()
{
    if (!_onmouseoutSet)
    {
        addEventListener("mouseout");
        _onmouseoutSet = true;
    }
    
    return _onmouseout;
}


Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
OSXWebViewDOMElement::onmouseover()
{
	if (!_onmouseoverSet)
	{
		addEventListener("mouseover");
		_onmouseoverSet = true;
	}

	return _onmouseover;
}

void
OSXWebViewDOMElement::update()
{
    if (_engine->isReady())
    {
        std::string js = "(Minko.getEventsCount(" + _jsAccessor + "))";
        int l = atoi(_engine->eval(js).c_str());
        
        for(int i = 0; i < l; ++i)
        {
            std::string eventName = "Minko.event" + std::to_string(_elementUid++);
            js =  eventName + " = " + _jsAccessor + ".minkoEvents[" + std::to_string(i) + "];";
            _engine->eval(js);
            
            OSXWebViewDOMMouseEvent::Ptr event = OSXWebViewDOMMouseEvent::create(eventName, _engine);
            
            std::string type = event->type();
            
            if (type == "click")
                _onclick->execute(event);
            else if (type == "mousedown")
                _onmousedown->execute(event);
            else if (type == "mouseup")
                _onmouseup->execute(event);
            else if (type == "mousemove")
                _onmousemove->execute(event);
            else if (type == "mouseover")
                _onmouseover->execute(event);
            else if (type == "mouseout")
                _onmouseout->execute(event);
        }
        
        js = "Minko.clearEvents(" + _jsAccessor + ");";
        _engine->eval(js);
    }
}