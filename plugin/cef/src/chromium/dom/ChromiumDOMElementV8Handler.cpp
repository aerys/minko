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

#include "chromium/dom/ChromiumDOMElementV8Handler.hpp"

using namespace chromium::dom;
using namespace minko::dom;

ChromiumDOMElementV8Handler::ChromiumDOMElementV8Handler() :
_onclick(minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::create()),
_onmousedown(minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::create()),
_onmousemove(minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::create()),
_onmouseup(minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::create()),
_onmouseover(minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::create()),
_onmouseout(minko::Signal<minko::dom::AbstractDOMEvent::Ptr>::create())
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

			if (type == "click" && _onclick->numCallbacks() > 0)
			{
				_onclick->execute(ChromiumDOMEvent::create(event));
			}
			else if (type == "mousedown" && _onmousedown->numCallbacks() > 0)
			{
				_onmousedown->execute(ChromiumDOMEvent::create(event));
			}
			else if (type == "mousemove" && _onmousemove->numCallbacks() > 0)
			{
				_onmousemove->execute(ChromiumDOMEvent::create(event));
			}
			else if (type == "mouseup" && _onmouseup->numCallbacks() > 0)
			{
				_onmouseup->execute(ChromiumDOMEvent::create(event));
			}
			else if (type == "mouseover" && _onmouseover->numCallbacks() > 0)
			{
				_onmouseover->execute(ChromiumDOMEvent::create(event));
			}
			else if (type == "mouseout" && _onmouseout->numCallbacks() > 0)
			{
				_onmouseout->execute(ChromiumDOMEvent::create(event));
			}
		}
	}
	return true;
}

std::shared_ptr<minko::Signal<minko::dom::AbstractDOMEvent::Ptr>>
ChromiumDOMElementV8Handler::onclick()
{
	return _onclick;
}

std::shared_ptr<minko::Signal<minko::dom::AbstractDOMEvent::Ptr>>
ChromiumDOMElementV8Handler::onmousedown()
{
	return _onmousedown;
}

std::shared_ptr<minko::Signal<minko::dom::AbstractDOMEvent::Ptr>>
ChromiumDOMElementV8Handler::onmousemove()
{
	return _onmousemove;
}

std::shared_ptr<minko::Signal<minko::dom::AbstractDOMEvent::Ptr>>
ChromiumDOMElementV8Handler::onmouseup()
{
	return _onmouseup;
}

std::shared_ptr<minko::Signal<minko::dom::AbstractDOMEvent::Ptr>>
ChromiumDOMElementV8Handler::onmouseover()
{
	return _onmouseover;
}

std::shared_ptr<minko::Signal<minko::dom::AbstractDOMEvent::Ptr>>
ChromiumDOMElementV8Handler::onmouseout()
{
	return _onmouseout;
}