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
#include "ioswebview/dom/IOSWebViewDOMElement.hpp"
#include "ioswebview/dom/IOSWebViewDOMEvent.hpp"
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
	_onclickSet(false),
	_onmousedownSet(false),
	_onmousemoveSet(false),
	_onmouseupSet(false),
	_onmouseoverSet(false),
	_onmouseoutSet(false)
{
	std::string eval = jsAccessor + ".minkoName = '" + jsAccessor + "';";
	//emscripten_run_script(eval.c_str());
}


IOSWebViewDOMElement::Ptr
IOSWebViewDOMElement::getDOMElement(std::string jsElement)
{
//	std::string eval = "if (" + jsElement + ".minkoName) (" + jsElement + ".minkoName); else ('');";
//	std::string minkoName = std::string(emscripten_run_script_string(eval.c_str()));
//
//	if (minkoName == "")
//	{
//		minkoName = "Minko.element" + std::to_string(_elementUid++);
//
//		eval = minkoName + " = " + jsElement;
//		emscripten_run_script(eval.c_str());
//	}
//	else
//	{
//		auto i = _accessorToElement.find(minkoName);
//
//		if (i != _accessorToElement.end())
//			return i->second;
//	}
//
//	return create(minkoName);
    
    return nullptr;
}

IOSWebViewDOMElement::Ptr
IOSWebViewDOMElement::create(std::string jsAccessor)
{
	IOSWebViewDOMElement::Ptr element(new IOSWebViewDOMElement(jsAccessor));
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
	std::string eval = "(" + _jsAccessor + ".id)";
	//char* result = emscripten_run_script_string(eval.c_str());
	
    return std::string(/*result*/);
}

void
IOSWebViewDOMElement::id(std::string newId)
{
	std::string eval = _jsAccessor + ".id = '" + newId + "';";
	//emscripten_run_script(eval.c_str());
}

std::string
IOSWebViewDOMElement::className()
{
	std::string eval = "(" + _jsAccessor + ".className)";
	//char* result = emscripten_run_script_string(eval.c_str());
	
    return std::string(/*result*/);
}

void
IOSWebViewDOMElement::className(std::string newClassName)
{
	std::string eval = _jsAccessor + ".className = '" + newClassName + "';";
	//emscripten_run_script(eval.c_str());
}

std::string
IOSWebViewDOMElement::tagName()
{
	std::string eval = "(" + _jsAccessor + ".tagName)";
	//char* result = emscripten_run_script_string(eval.c_str());
    
	return std::string(/*result*/);
}

AbstractDOMElement::Ptr
IOSWebViewDOMElement::parentNode()
{
	std::string eval = "Minko.tmpElement  = " + _jsAccessor + ".parentNode;";
	//emscripten_run_script(eval.c_str());

	return IOSWebViewDOMElement::getDOMElement("Minko.tmpElement");
}

std::vector<minko::dom::AbstractDOMElement::Ptr>
IOSWebViewDOMElement::childNodes()
{
	return (IOSWebViewDOM::getElementList(_jsAccessor + ".childNodes"));
}

std::string
IOSWebViewDOMElement::textContent()
{
	std::string eval = "(" + _jsAccessor + ".textContent)";
	//char* result = emscripten_run_script_string(eval.c_str());
    
	return std::string(/*result*/);
}

void
IOSWebViewDOMElement::textContent(std::string newTextContent)
{
	std::string eval = _jsAccessor + ".textContent = '" + newTextContent + "';";
	//emscripten_run_script(eval.c_str());
}

std::string
IOSWebViewDOMElement::innerHTML()
{
	std::string eval = "(" + _jsAccessor + ".innerHTML)";
	//char* result = emscripten_run_script_string(eval.c_str());
	
    return std::string(/*result*/);
}

void
IOSWebViewDOMElement::innerHTML(std::string newInnerHTML)
{
	std::string eval = _jsAccessor + ".innerHTML = '" + newInnerHTML + "';";
	//emscripten_run_script(eval.c_str());
}

AbstractDOMElement::Ptr
IOSWebViewDOMElement::appendChild(minko::dom::AbstractDOMElement::Ptr newChild)
{
	IOSWebViewDOMElement::Ptr child = std::dynamic_pointer_cast<IOSWebViewDOMElement>(newChild);
	std::string eval = _jsAccessor + ".appendChild(" + child->getJavascriptAccessor() + ");";
	//emscripten_run_script(eval.c_str());

	return shared_from_this();
}

AbstractDOMElement::Ptr
IOSWebViewDOMElement::removeChild(minko::dom::AbstractDOMElement::Ptr childToRemove)
{
	IOSWebViewDOMElement::Ptr child = std::dynamic_pointer_cast<IOSWebViewDOMElement>(childToRemove);
	std::string eval = _jsAccessor + ".removeChild(" + child->getJavascriptAccessor() + ");";
	//emscripten_run_script(eval.c_str());

	return shared_from_this();
}

AbstractDOMElement::Ptr
IOSWebViewDOMElement::insertBefore(minko::dom::AbstractDOMElement::Ptr newChild, minko::dom::AbstractDOMElement::Ptr adjacentNode)
{
	IOSWebViewDOMElement::Ptr child = std::dynamic_pointer_cast<IOSWebViewDOMElement>(newChild);
	IOSWebViewDOMElement::Ptr adjNode = std::dynamic_pointer_cast<IOSWebViewDOMElement>(adjacentNode);
	std::string eval = _jsAccessor + ".removeChild(" + child->getJavascriptAccessor() + ", " + adjNode->getJavascriptAccessor() + ");";
	//emscripten_run_script(eval.c_str());

	return shared_from_this();
}

AbstractDOMElement::Ptr
IOSWebViewDOMElement::cloneNode(bool deep)
{
	std::string eval = "Minko.tmpElement = " + _jsAccessor + ".cloneNode(" + (deep ? "true" : "false") + ");";
	//emscripten_run_script(eval.c_str());

	return IOSWebViewDOMElement::create("Minko.tmpElement");
}

std::string
IOSWebViewDOMElement::getAttribute(std::string name)
{
	std::string eval = "(" + _jsAccessor + ".getAttribute('" + name + "'))";
	//char* result = emscripten_run_script_string(eval.c_str());
	
    return std::string(/*result*/);
}

void
IOSWebViewDOMElement::setAttribute(std::string name, std::string value)
{
	std::string eval = _jsAccessor + ".setAttribute('" + name + "', '" + value + "');";
	//emscripten_run_script(eval.c_str());
}

std::vector<minko::dom::AbstractDOMElement::Ptr>
IOSWebViewDOMElement::getElementsByTagName(std::string tagName)
{
	return (EmscriptenDOM::getElementList(_jsAccessor + ".getElementsByTagName('" + tagName + "')"));
}


std::string
IOSWebViewDOMElement::style(std::string name)
{
	std::string eval = "(" + _jsAccessor + ".style." + name + ")";
	//char* result = emscripten_run_script_string(eval.c_str());
	
    return std::string(/*result*/);
}

void
IOSWebViewDOMElement::style(std::string name, std::string value)
{
	std::string eval = _jsAccessor + ".style." + name + " = '" + value + "';";
	//emscripten_run_script(eval.c_str());
}

void
IOSWebViewDOMElement::addEventListener(std::string type)
{
	std::string eval = "Minko.addListener(" + _jsAccessor + ", '" + type + "');";

	//emscripten_run_script(eval.c_str());
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
IOSWebViewDOMElement::onclick()
{
	if (!_onclickSet)
	{
		addEventListener("click");
		_onclickSet = false;
	}

	return _onclick;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
IOSWebViewDOMElement::onmousedown()
{
	if (!_onmousedownSet)
	{
		addEventListener("mousedown");
		_onmousedownSet = false;
	}

	return _onmousedown;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
IOSWebViewDOMElement::onmousemove()
{
	if (!_onmousemoveSet)
	{
		addEventListener("mousemove");
		_onmousemoveSet = false;
	}
	
	return _onmousemove;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
IOSWebViewDOMElement::onmouseup()
{
	if (!_onmouseupSet)
	{
		addEventListener("mouseup");
		_onmouseupSet = false;
	}

	return _onmouseup;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
IOSWebViewDOMElement::onmouseout()
{
	if (!_onmouseoutSet)
	{
		addEventListener("mouseout");
		_onmouseoutSet = false;
	}

	return _onmouseout;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
IOSWebViewDOMElement::onmouseover()
{
	if (!_onmouseoverSet)
	{
		addEventListener("mouseover");
		_onmouseoverSet = false;
	}

	return _onmouseover;
}

void
IOSWebViewDOMElement::update()
{ 
	std::string eval = "(Minko.getEventsCount(" + _jsAccessor + "))";
	//int l = emscripten_run_script_int(eval.c_str());

	for(int i = 0; i < l; ++i)
	{
		std::string eventName = "Minko.event" + std::to_string(_elementUid++);
		eval =  eventName + " = " + _jsAccessor + ".minkoEvents[" + std::to_string(i) + "];";
		//emscripten_run_script(eval.c_str());

		EmscriptenDOMEvent::Ptr event = EmscriptenDOMEvent::create(eventName);

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

	eval = "Minko.clearEvents(" + _jsAccessor + ");";
	//emscripten_run_script(eval.c_str());
}

#endif