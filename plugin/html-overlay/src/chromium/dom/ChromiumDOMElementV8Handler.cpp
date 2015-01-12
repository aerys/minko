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

#if defined(CHROMIUM)
#include "chromium/dom/ChromiumDOMElementV8Handler.hpp"
#include "chromium/dom/ChromiumDOMElement.hpp"

using namespace chromium::dom;
using namespace minko::dom;
using namespace minko;

ChromiumDOMElementV8Handler::ChromiumDOMElementV8Handler(ChromiumDOMElement::Ptr element) :
	_element(element)
{
}

bool
ChromiumDOMElementV8Handler::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception)
{
	if (arguments.size() > 0)
	{
		CefRefPtr<CefV8Value> event = arguments[0];

		if (event->IsObject())
		{
			std::string type = event->GetValue("type")->GetStringValue();

            if (type == "change" && _element->onchange()->numCallbacks() > 0)
                _element->onchange()->execute(ChromiumDOMEvent::create(event, CefV8Context::GetCurrentContext()));
            else if (type == "input" && _element->oninput()->numCallbacks() > 0)
                _element->oninput()->execute(ChromiumDOMEvent::create(event, CefV8Context::GetCurrentContext()));
            else if (type.substr(0, 5) == "touch")
            {
                minko::Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Ptr touchSignal;

                if (type == "touchstart" && _element->ontouchstart()->numCallbacks() > 0)
				    touchSignal = _element->ontouchstart();
			    else if (type == "touchend" && _element->ontouchend()->numCallbacks() > 0)
				    touchSignal = _element->ontouchend();
			    else if (type == "touchmove" && _element->ontouchmove()->numCallbacks() > 0)
				    touchSignal = _element->ontouchmove();
            
			    if (touchSignal != nullptr)
			    {
				    ChromiumDOMTouchEvent::Ptr domTouchEvent = ChromiumDOMTouchEvent::create(event, CefV8Context::GetCurrentContext());
				    _element->addFunction([=]()
				    {
					    touchSignal->execute(domTouchEvent);
				    });
			    }
            }
            else
            {
                minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Ptr mouseSignal;

			    if (type == "click" && _element->onclick()->numCallbacks() > 0)
				    mouseSignal = _element->onclick();
			    else if (type == "mousedown" && _element->onmousedown()->numCallbacks() > 0)
				    mouseSignal = _element->onmousedown();
			    else if (type == "mousemove" && _element->onmousemove()->numCallbacks() > 0)
				    mouseSignal = _element->onmousemove();
			    else if (type == "mouseup" && _element->onmouseup()->numCallbacks() > 0)
				    mouseSignal = _element->onmouseup();
			    else if (type == "mouseover" && _element->onmouseover()->numCallbacks() > 0)
				    mouseSignal = _element->onmouseover();
			    else if (type == "mouseout" && _element->onmouseout()->numCallbacks() > 0)
				    mouseSignal = _element->onmouseout();
            
			    if (mouseSignal != nullptr)
			    {
				    ChromiumDOMMouseEvent::Ptr domMouseEvent = ChromiumDOMMouseEvent::create(event, CefV8Context::GetCurrentContext());
				    _element->addFunction([=]()
				    {
					    mouseSignal->execute(domMouseEvent);
				    });
			    }
            }
		}
	}
	return true;
}
#endif