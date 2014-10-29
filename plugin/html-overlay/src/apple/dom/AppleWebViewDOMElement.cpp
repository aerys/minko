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
#include "minko/dom/AbstractDOMEvent.hpp"
#include "apple/dom/AppleWebViewDOMElement.hpp"
#include "apple/dom/AppleWebViewDOMEngine.hpp"
#include "apple/dom/AppleWebViewDOM.hpp"

#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
# include "apple/dom/AppleWebViewDOMTouchEvent.hpp"
#elif TARGET_OS_MAC // OSX
# include "apple/dom/AppleWebViewDOMMouseEvent.hpp"
#endif

using namespace minko;
using namespace minko::dom;
using namespace apple;
using namespace apple::dom;

int
AppleWebViewDOMElement::_elementUid = 0;

std::list<AppleWebViewDOMElement::Ptr>
AppleWebViewDOMElement::domElements;

std::map<std::string,AppleWebViewDOMElement::Ptr>
AppleWebViewDOMElement::_accessorToElement;

AppleWebViewDOMElement::AppleWebViewDOMElement(const std::string& jsAccessor) :
    _jsAccessor(jsAccessor),
    _onclick(Signal<AbstractDOMMouseEvent::Ptr>::create()),
    _onmousedown(Signal<AbstractDOMMouseEvent::Ptr>::create()),
    _onmousemove(Signal<AbstractDOMMouseEvent::Ptr>::create()),
    _onmouseup(Signal<AbstractDOMMouseEvent::Ptr>::create()),
    _onmouseout(Signal<AbstractDOMMouseEvent::Ptr>::create()),
    _onmouseover(Signal<AbstractDOMMouseEvent::Ptr>::create()),
    _oninput(Signal<AbstractDOMEvent::Ptr>::create()),
    _onchange(Signal<AbstractDOMEvent::Ptr>::create()),
    _onclickSet(false),
    _onmousedownSet(false),
    _onmousemoveSet(false),
    _onmouseupSet(false),
    _onmouseoverSet(false),
    _onmouseoutSet(false),
    _oninputSet(false),
    _onchangeSet(false),
    _ontouchstart(Signal<AbstractDOMTouchEvent::Ptr>::create()),
    _ontouchend(Signal<AbstractDOMTouchEvent::Ptr>::create()),
    _ontouchmove(Signal<AbstractDOMTouchEvent::Ptr>::create()),
    _ontouchstartSet(false),
    _ontouchendSet(false),
    _ontouchmoveSet(false),
    _engine(nullptr)
{
}

void
AppleWebViewDOMElement::initialize(std::shared_ptr<AppleWebViewDOMEngine> engine)
{
    _engine = engine;

    std::string jsEval = _jsAccessor + ".minkoName = '" + _jsAccessor + "';";
    _engine->eval(jsEval);
}


AppleWebViewDOMElement::Ptr
AppleWebViewDOMElement::getDOMElement(const std::string& jsElement, std::shared_ptr<AppleWebViewDOMEngine> engine)
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

AppleWebViewDOMElement::Ptr
AppleWebViewDOMElement::create(const std::string& jsAccessor, std::shared_ptr<AppleWebViewDOMEngine> engine)
{
    AppleWebViewDOMElement::Ptr element(new AppleWebViewDOMElement(jsAccessor));
    element->initialize(engine);

    domElements.push_back(element);
    _accessorToElement[jsAccessor] = element;

    return element;
}

std::string
AppleWebViewDOMElement::getJavascriptAccessor()
{
    return _jsAccessor;
}

std::string
AppleWebViewDOMElement::id()
{
    std::string js = "(" + _jsAccessor + ".id)";
    std::string result = _engine->eval(js);

    return result;
}

void
AppleWebViewDOMElement::id(const std::string& newId)
{
    std::string js = _jsAccessor + ".id = '" + newId + "';";
    _engine->eval(js);
}

void
AppleWebViewDOMElement::value(const std::string& newValue)
{
    std::string js = _jsAccessor + ".value = '" + newValue + "';";
    _engine->eval(js);
}

std::string
AppleWebViewDOMElement::value()
{
    std::string js = "(" + _jsAccessor + ".value)";
    std::string result = _engine->eval(js);

    return result;
}

std::string
AppleWebViewDOMElement::className()
{
    std::string js = "(" + _jsAccessor + ".className)";
    std::string result = _engine->eval(js);

    return result;
}

void
AppleWebViewDOMElement::className(const std::string& newClassName)
{
    std::string js = _jsAccessor + ".className = '" + newClassName + "';";
    _engine->eval(js);
}

std::string
AppleWebViewDOMElement::tagName()
{
    std::string js = "(" + _jsAccessor + ".tagName)";
    std::string result = _engine->eval(js);

    return result;
}

AbstractDOMElement::Ptr
AppleWebViewDOMElement::parentNode()
{
    std::string js = "Minko.tmpElement  = " + _jsAccessor + ".parentNode;";
    _engine->eval(js);

    return AppleWebViewDOMElement::getDOMElement("Minko.tmpElement", _engine);
}

std::vector<minko::dom::AbstractDOMElement::Ptr>
AppleWebViewDOMElement::childNodes()
{
    return (_engine->currentDOM()->getElementList(_jsAccessor + ".childNodes"));
}

std::string
AppleWebViewDOMElement::textContent()
{
    std::string js = "(" + _jsAccessor + ".textContent)";
    std::string result = _engine->eval(js);

    return result;
}

void
AppleWebViewDOMElement::textContent(const std::string& newTextContent)
{
    std::string js = _jsAccessor + ".textContent = '" + newTextContent + "';";
    _engine->eval(js);
}

std::string
AppleWebViewDOMElement::innerHTML()
{
    std::string js = "(" + _jsAccessor + ".innerHTML)";
    std::string result = _engine->eval(js);

    return result;
}

void
AppleWebViewDOMElement::innerHTML(const std::string& newInnerHTML)
{
    std::string js = _jsAccessor + ".innerHTML = '" + newInnerHTML + "';";
    _engine->eval(js);
}

AbstractDOMElement::Ptr
AppleWebViewDOMElement::appendChild(minko::dom::AbstractDOMElement::Ptr newChild)
{
    AppleWebViewDOMElement::Ptr child = std::dynamic_pointer_cast<AppleWebViewDOMElement>(newChild);
    std::string js = _jsAccessor + ".appendChild(" + child->getJavascriptAccessor() + ");";
    _engine->eval(js);

    return shared_from_this();
}

AbstractDOMElement::Ptr
AppleWebViewDOMElement::removeChild(minko::dom::AbstractDOMElement::Ptr childToRemove)
{
    AppleWebViewDOMElement::Ptr child = std::dynamic_pointer_cast<AppleWebViewDOMElement>(childToRemove);
    std::string js = _jsAccessor + ".removeChild(" + child->getJavascriptAccessor() + ");";
    _engine->eval(js);

    return shared_from_this();
}

AbstractDOMElement::Ptr
AppleWebViewDOMElement::insertBefore(minko::dom::AbstractDOMElement::Ptr newChild, minko::dom::AbstractDOMElement::Ptr adjacentNode)
{
    AppleWebViewDOMElement::Ptr child = std::dynamic_pointer_cast<AppleWebViewDOMElement>(newChild);
    AppleWebViewDOMElement::Ptr adjNode = std::dynamic_pointer_cast<AppleWebViewDOMElement>(adjacentNode);
    std::string js = _jsAccessor + ".removeChild(" + child->getJavascriptAccessor() + ", " + adjNode->getJavascriptAccessor() + ");";
    _engine->eval(js);

    return shared_from_this();
}

AbstractDOMElement::Ptr
AppleWebViewDOMElement::cloneNode(bool deep)
{
    std::string js = "Minko.tmpElement = " + _jsAccessor + ".cloneNode(" + (deep ? "true" : "false") + ");";
    _engine->eval(js);

    return AppleWebViewDOMElement::create("Minko.tmpElement", _engine);
}

std::string
AppleWebViewDOMElement::getAttribute(const std::string& name)
{
    std::string js = "(" + _jsAccessor + ".getAttribute('" + name + "'))";
    std::string result = _engine->eval(js);

    return result;
}

void
AppleWebViewDOMElement::setAttribute(const std::string& name, const std::string& value)
{
    std::string js = _jsAccessor + ".setAttribute('" + name + "', '" + value + "');";
    _engine->eval(js);
}

std::vector<minko::dom::AbstractDOMElement::Ptr>
AppleWebViewDOMElement::getElementsByTagName(const std::string& tagName)
{
    return (_engine->currentDOM()->getElementList(_jsAccessor + ".getElementsByTagName('" + tagName + "')"));
}

std::string
AppleWebViewDOMElement::style(const std::string& name)
{
    std::string js = "(" + _jsAccessor + ".style." + name + ")";
    std::string result = _engine->eval(js);

    return result;
}

void
AppleWebViewDOMElement::style(const std::string& name, const std::string& value)
{
    std::string js = _jsAccessor + ".style." + name + " = '" + value + "';";
    _engine->eval(js);
}

void
AppleWebViewDOMElement::addEventListener(const std::string& type)
{
    std::string js = "Minko.addListener(" + _jsAccessor + ", '" + type + "');";

    _engine->eval(js);
}

// Events

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
AppleWebViewDOMElement::onchange()
{
    if (!_onchangeSet)
    {
        addEventListener("change");
        _onchangeSet = true;
    }

    return _onchange;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
AppleWebViewDOMElement::oninput()
{
    if (!_oninputSet)
    {
        addEventListener("input");
        _oninputSet = true;
    }

    return _oninput;
}

Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
AppleWebViewDOMElement::onclick()
{
    if (!_onclickSet)
    {
        addEventListener("click");
        _onclickSet = true;
    }

    return _onclick;
}

Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
AppleWebViewDOMElement::onmousedown()
{
    if (!_onmousedownSet)
    {
        addEventListener("mousedown");
        _onmousedownSet = true;
    }

    return _onmousedown;
}

Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
AppleWebViewDOMElement::onmouseup()
{
    if (!_onmouseupSet)
    {
        addEventListener("mouseup");
        _onmouseupSet = true;
    }

    return _onmouseup;
}


Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
AppleWebViewDOMElement::onmousemove()
{
    if (!_onmousemoveSet)
    {
        addEventListener("mousemove");
        _onmousemoveSet = true;
    }

    return _onmousemove;
}

Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
AppleWebViewDOMElement::onmouseout()
{
    if (!_onmouseoutSet)
    {
        addEventListener("mouseout");
        _onmouseoutSet = true;
    }

    return _onmouseout;
}

Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
AppleWebViewDOMElement::onmouseover()
{
    if (!_onmouseoverSet)
    {
        addEventListener("mouseover");
        _onmouseoverSet = true;
    }

    return _onmouseover;
}

Signal<std::shared_ptr<AbstractDOMTouchEvent>>::Ptr
AppleWebViewDOMElement::ontouchstart()
{
    if (!_ontouchstartSet)
    {
        addEventListener("touchstart");
        _ontouchstartSet = true;
    }

    return _ontouchstart;
}

Signal<std::shared_ptr<AbstractDOMTouchEvent>>::Ptr
AppleWebViewDOMElement::ontouchend()
{
    if (!_ontouchendSet)
    {
        addEventListener("touchend");
        _ontouchendSet = true;
    }

    return _ontouchend;
}

Signal<std::shared_ptr<AbstractDOMTouchEvent>>::Ptr
AppleWebViewDOMElement::ontouchmove()
{
    if (!_ontouchmoveSet)
    {
        addEventListener("touchmove");
        _ontouchmoveSet = true;
    }

    return _ontouchmove;
}

void
AppleWebViewDOMElement::update()
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

            auto type = _engine->eval(eventName + ".type");

            // It's a touch event ?
            if (type == "change")
                _onchange->execute(AppleWebViewDOMEvent::create(eventName, _engine));
            else if (type == "input")
                _oninput->execute(AppleWebViewDOMEvent::create(eventName, _engine));
            else if (type.find("touch") == 0)
            {
                std::string js = eventName + ".changedTouches.length";
                int touchNumber = atoi(_engine->eval(js).c_str());

                for (auto i = 0; i < touchNumber; i++)
                {
                    // Create the touch event
                    AppleWebViewDOMTouchEvent::Ptr event = AppleWebViewDOMTouchEvent::create(eventName, i, _engine);

                    std::string type = event->type();

                    if (type == "touchstart")
                    {
                        _ontouchstart->execute(event);

                        if (_engine->numTouches() == 0 && i == 0)
                        {
                            _onmousedown->execute(event);
                        }
                    }
                    else if (type == "touchend")
                    {
                        _ontouchend->execute(event);

                        if (event->identifier() == _engine->firstTouchIdentifier())
                        {
                            _onclick->execute(event);
                            _onmouseup->execute(event);
                        }
                    }
                    else if (type == "touchmove")
                    {
                        _ontouchmove->execute(event);

                        if (_engine->numTouches() == 0 || event->identifier() == _engine->firstTouchIdentifier())
                        {
                            _onmousemove->execute(event);
                        }

                    }
                }
            }

            AppleWebViewDOMMouseEvent::Ptr event = AppleWebViewDOMMouseEvent::create(eventName, _engine);

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
