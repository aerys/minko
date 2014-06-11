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

#if defined(TARGET_IPHONE_SIMULATOR) or defined(TARGET_OS_IPHONE) // iOS

#include "minko/Common.hpp"
#include "minko/input/Mouse.hpp"
#include "ioswebview/dom/IOSWebViewDOMElement.hpp"
#include "ioswebview/dom/IOSWebViewDOMEvent.hpp"
#include "ioswebview/dom/IOSWebViewDOMEngine.hpp"
#include "ioswebview/dom/IOSWebViewDOM.hpp"
#include "minko/dom/AbstractDOMEvent.hpp"


using namespace minko;
using namespace minko::dom;
using namespace ioswebview;
using namespace ioswebview::dom;


int
IOSWebViewDOMElement::_elementUid = 0;

std::list<IOSWebViewDOMElement::Ptr>
IOSWebViewDOMElement::domElements;

std::map<std::string,IOSWebViewDOMElement::Ptr>
IOSWebViewDOMElement::_accessorToElement;

IOSWebViewDOMElement::IOSWebViewDOMElement(std::string jsAccessor) :
	_jsAccessor(jsAccessor),
	_onclick(Signal<AbstractDOMEvent::Ptr>::create()),
	_onmousedown(Signal<AbstractDOMEvent::Ptr>::create()),
	_onmousemove(Signal<AbstractDOMEvent::Ptr>::create()),
	_onmouseup(Signal<AbstractDOMEvent::Ptr>::create()),
	_onmouseover(Signal<AbstractDOMEvent::Ptr>::create()),
	_onmouseout(Signal<AbstractDOMEvent::Ptr>::create()),
    _ontouchdown(Signal<AbstractDOMEvent::Ptr>::create()),
    _ontouchup(Signal<AbstractDOMEvent::Ptr>::create()),
    _ontouchmotion(Signal<AbstractDOMEvent::Ptr>::create()),
	_onmouseoverSet(false),
	_onmouseoutSet(false),
    _ontouchdownSet(false),
    _ontouchupSet(false),
    _ontouchmotionSet(false),
    _engine(nullptr)
{
	
}

void
IOSWebViewDOMElement::initialize(std::shared_ptr<IOSWebViewDOMEngine> engine)
{
    _engine = engine;
    
    std::string jsEval = _jsAccessor + ".minkoName = '" + _jsAccessor + "';";
    _engine->eval(jsEval);
}


IOSWebViewDOMElement::Ptr
IOSWebViewDOMElement::getDOMElement(std::string jsElement, std::shared_ptr<IOSWebViewDOMEngine> engine)
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

IOSWebViewDOMElement::Ptr
IOSWebViewDOMElement::create(std::string jsAccessor, std::shared_ptr<IOSWebViewDOMEngine> engine)
{
	IOSWebViewDOMElement::Ptr element(new IOSWebViewDOMElement(jsAccessor));
    element->initialize(engine);
    
    domElements.push_back(element);
	_accessorToElement[jsAccessor] = element;
    
	return element;
}

std::string
IOSWebViewDOMElement::getJavascriptAccessor()
{
	return _jsAccessor;
}

std::string
IOSWebViewDOMElement::id()
{
	std::string js = "(" + _jsAccessor + ".id)";
    std::string result = _engine->eval(js);
    
    return result;
}

void
IOSWebViewDOMElement::id(std::string newId)
{
	std::string js = _jsAccessor + ".id = '" + newId + "';";
	_engine->eval(js);
}

std::string
IOSWebViewDOMElement::className()
{
	std::string js = "(" + _jsAccessor + ".className)";
    std::string result = _engine->eval(js);
	
    return result;
}

void
IOSWebViewDOMElement::className(std::string newClassName)
{
	std::string js = _jsAccessor + ".className = '" + newClassName + "';";
	_engine->eval(js);
}

std::string
IOSWebViewDOMElement::tagName()
{
	std::string js = "(" + _jsAccessor + ".tagName)";
	std::string result = _engine->eval(js);
	
    return result;
}

AbstractDOMElement::Ptr
IOSWebViewDOMElement::parentNode()
{
	std::string js = "Minko.tmpElement  = " + _jsAccessor + ".parentNode;";
	_engine->eval(js);

	return IOSWebViewDOMElement::getDOMElement("Minko.tmpElement", _engine);
}

std::vector<minko::dom::AbstractDOMElement::Ptr>
IOSWebViewDOMElement::childNodes()
{
	return (_engine->currentDOM()->getElementList(_jsAccessor + ".childNodes"));
}

std::string
IOSWebViewDOMElement::textContent()
{
	std::string js = "(" + _jsAccessor + ".textContent)";
	std::string result = _engine->eval(js);
	
    return result;
}

void
IOSWebViewDOMElement::textContent(std::string newTextContent)
{
	std::string js = _jsAccessor + ".textContent = '" + newTextContent + "';";
	_engine->eval(js);
}

std::string
IOSWebViewDOMElement::innerHTML()
{
	std::string js = "(" + _jsAccessor + ".innerHTML)";
	std::string result = _engine->eval(js);
	
    return result;
}

void
IOSWebViewDOMElement::innerHTML(std::string newInnerHTML)
{
	std::string js = _jsAccessor + ".innerHTML = '" + newInnerHTML + "';";
	_engine->eval(js);
}

AbstractDOMElement::Ptr
IOSWebViewDOMElement::appendChild(minko::dom::AbstractDOMElement::Ptr newChild)
{
	IOSWebViewDOMElement::Ptr child = std::dynamic_pointer_cast<IOSWebViewDOMElement>(newChild);
	std::string js = _jsAccessor + ".appendChild(" + child->getJavascriptAccessor() + ");";
	_engine->eval(js);

	return shared_from_this();
}

AbstractDOMElement::Ptr
IOSWebViewDOMElement::removeChild(minko::dom::AbstractDOMElement::Ptr childToRemove)
{
	IOSWebViewDOMElement::Ptr child = std::dynamic_pointer_cast<IOSWebViewDOMElement>(childToRemove);
	std::string js = _jsAccessor + ".removeChild(" + child->getJavascriptAccessor() + ");";
	_engine->eval(js);

	return shared_from_this();
}

AbstractDOMElement::Ptr
IOSWebViewDOMElement::insertBefore(minko::dom::AbstractDOMElement::Ptr newChild, minko::dom::AbstractDOMElement::Ptr adjacentNode)
{
	IOSWebViewDOMElement::Ptr child = std::dynamic_pointer_cast<IOSWebViewDOMElement>(newChild);
	IOSWebViewDOMElement::Ptr adjNode = std::dynamic_pointer_cast<IOSWebViewDOMElement>(adjacentNode);
	std::string js = _jsAccessor + ".removeChild(" + child->getJavascriptAccessor() + ", " + adjNode->getJavascriptAccessor() + ");";
	_engine->eval(js);

	return shared_from_this();
}

AbstractDOMElement::Ptr
IOSWebViewDOMElement::cloneNode(bool deep)
{
	std::string js = "Minko.tmpElement = " + _jsAccessor + ".cloneNode(" + (deep ? "true" : "false") + ");";
	_engine->eval(js);

	return IOSWebViewDOMElement::create("Minko.tmpElement", _engine);
}

std::string
IOSWebViewDOMElement::getAttribute(std::string name)
{
	std::string js = "(" + _jsAccessor + ".getAttribute('" + name + "'))";
	std::string result = _engine->eval(js);
	
    return result;
}

void
IOSWebViewDOMElement::setAttribute(std::string name, std::string value)
{
	std::string js = _jsAccessor + ".setAttribute('" + name + "', '" + value + "');";
	_engine->eval(js);
}

std::vector<minko::dom::AbstractDOMElement::Ptr>
IOSWebViewDOMElement::getElementsByTagName(std::string tagName)
{
	return (_engine->currentDOM()->getElementList(_jsAccessor + ".getElementsByTagName('" + tagName + "')"));
}

std::string
IOSWebViewDOMElement::style(std::string name)
{
	std::string js = "(" + _jsAccessor + ".style." + name + ")";
	std::string result = _engine->eval(js);
	
    return result;
}

void
IOSWebViewDOMElement::style(std::string name, std::string value)
{
	std::string js = _jsAccessor + ".style." + name + " = '" + value + "';";
	_engine->eval(js);
}

void
IOSWebViewDOMElement::addEventListener(std::string type)
{
	std::string js = "Minko.addListener(" + _jsAccessor + ", '" + type + "');";

	_engine->eval(js);
}

// Events

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
IOSWebViewDOMElement::onclick()
{
	if (!_ontouchupSet)
	{
		addEventListener("touchend");
		_ontouchupSet = true;
	}

	return _onclick;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
IOSWebViewDOMElement::onmousedown()
{
	if (!_ontouchdownSet)
	{
		addEventListener("touchstart");
		_ontouchdownSet = true;
	}

	return _onmousedown;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
IOSWebViewDOMElement::onmousemove()
{
	if (!_ontouchmotionSet)
	{
		addEventListener("touchmove");
		_ontouchmotionSet = true;
	}
	
	return _onmousemove;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
IOSWebViewDOMElement::onmouseup()
{
	if (!_ontouchupSet)
	{
		addEventListener("touchend");
		_ontouchupSet = true;
	}

	return _onmouseup;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
IOSWebViewDOMElement::ontouchdown()
{
    if (!_ontouchdownSet)
	{
		addEventListener("touchstart");
		_ontouchdownSet = true;
	}
    
    return _ontouchdown;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
IOSWebViewDOMElement::ontouchup()
{
    if (!_ontouchupSet)
	{
		addEventListener("touchend");
		_ontouchupSet = true;
	}
    
    return _ontouchup;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
IOSWebViewDOMElement::ontouchmotion()
{
    if (!_ontouchmotionSet)
	{
		addEventListener("touchmove");
		_ontouchmotionSet = true;
	}
    
    return _ontouchmotion;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
IOSWebViewDOMElement::onmouseout()
{
    if (!_onmouseoutSet)
    {
        addEventListener("mouseout");
        _onmouseoutSet = true;
    }
    
    return _onmouseout;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
IOSWebViewDOMElement::onmouseover()
{
    if (!_onmouseoverSet)
    {
        addEventListener("mouseover");
        _onmouseoverSet = true;
    }
    
    return _onmouseover;
}

void
IOSWebViewDOMElement::update()
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
            
            IOSWebViewDOMEvent::Ptr event = IOSWebViewDOMEvent::create(eventName, _engine);
            
            std::string type = event->type();
            
            if (type == "touchstart")
            {
                _ontouchdown->execute(event);
                _onmousedown->execute(event);
            }
            else if (type == "touchend")
            {
                _ontouchup->execute(event);
                _onclick->execute(event);
                _onmouseup->execute(event);
            }
            else if (type == "touchmove")
            {
                _ontouchmotion->execute(event);
                _onmousemove->execute(event);
            }
        }
        
        js = "Minko.clearEvents(" + _jsAccessor + ");";
        _engine->eval(js);
    }
}

#endif