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

#include "chromium/dom/ChromiumDOMMouseEvent.hpp"
#include "minko/dom/AbstractDOMMouseEvent.hpp"
#include "chromium/dom/ChromiumDOMElement.hpp"
#include "chromium/dom/ChromiumDOMObject.hpp"
#include "include/cef_runnable.h"
#include "include/cef_task.h"

using namespace chromium;
using namespace chromium::dom;
using namespace minko;
using namespace minko::dom;

std::list<ChromiumDOMEvent::Ptr>
ChromiumDOMEvent::_events;

ChromiumDOMEvent::ChromiumDOMEvent(CefRefPtr<CefV8Value> v8NodeObject, CefRefPtr<CefV8Context> v8Context) :
_cleared(false),
ChromiumDOMObject()
{
    if (!v8NodeObject->IsObject())
        throw;

    _blocker = false;

    _v8NodeObject = v8NodeObject;
    _v8Context = v8Context;
}

ChromiumDOMEvent::~ChromiumDOMEvent()
{
    clear();
}

void
ChromiumDOMEvent::clear()
{
    if (_cleared)
        return;

    _v8NodeObject = nullptr;
    _cleared = true;
}

void
ChromiumDOMEvent::clearAll()
{
    for (Ptr event : _events)
    {
        event->clear();
    }

    _events.clear();
}

std::string
ChromiumDOMEvent::accessor()
{
    return "";
}

std::string
ChromiumDOMEvent::type()
{
    return getProperty<std::string>("type");
}

AbstractDOMElement::Ptr
ChromiumDOMEvent::target()
{
    return getProperty<AbstractDOMElement::Ptr>("target");
}
#endif