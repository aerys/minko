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
#include "chromium/dom/ChromiumDOMEvent.hpp"
#include "include/cef_runnable.h"
#include "include/cef_task.h"

using namespace chromium;
using namespace chromium::dom;
using namespace minko;
using namespace minko::dom;

ChromiumDOMMouseEvent::ChromiumDOMMouseEvent(CefRefPtr<CefV8Value> v8NodeObject, CefRefPtr<CefV8Context> v8Context)
    : ChromiumDOMEvent(v8NodeObject, v8Context)
{
	if (!v8NodeObject->IsObject())
		throw;
}

ChromiumDOMMouseEvent::~ChromiumDOMMouseEvent()
{
    clear();
}

ChromiumDOMMouseEvent::Ptr
ChromiumDOMMouseEvent::create(CefRefPtr<CefV8Value> v8NodeObject, CefRefPtr<CefV8Context> v8Context)
{
	ChromiumDOMMouseEvent::Ptr event(new ChromiumDOMMouseEvent(v8NodeObject, v8Context));
	_events.push_back(event);
	return event;
}
#endif