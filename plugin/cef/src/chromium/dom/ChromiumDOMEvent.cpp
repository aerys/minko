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

#pragma once

#include "chromium/dom/ChromiumDOMEvent.hpp"
#include "minko/dom/AbstractDOMEvent.hpp"
#include "chromium/dom/ChromiumDOMElement.hpp"

using namespace chromium;
using namespace chromium::dom;
using namespace minko;
using namespace minko::dom;

ChromiumDOMEvent::ChromiumDOMEvent(CefRefPtr<CefV8Value> v8NodeObject)
{
	if (!v8NodeObject->IsObject())
		throw;

	_v8NodeObject = v8NodeObject;
}

ChromiumDOMEvent::Ptr
ChromiumDOMEvent::initialize()
{
	return shared_from_this();
}

ChromiumDOMEvent::Ptr
ChromiumDOMEvent::create(CefRefPtr<CefV8Value> v8NodeObject)
{
	ChromiumDOMEvent* event = new ChromiumDOMEvent(v8NodeObject);
	return event->initialize();
}


CefRefPtr<CefV8Value>
ChromiumDOMEvent::getFunction(std::string name)
{
	CefRefPtr<CefV8Value> func = _v8NodeObject->GetValue(name);

	if (!func->IsFunction())
		func = nullptr;

	return func;
}

CefRefPtr<CefV8Value>
ChromiumDOMEvent::getProperty(std::string name)
{
	CefRefPtr<CefV8Value> prop = _v8NodeObject->GetValue(name);

	if (prop->IsFunction())
		prop = nullptr;

	return prop;
}


void
ChromiumDOMEvent::preventDefault()
{
	CefV8ValueList args;
	getFunction("preventDefault")->ExecuteFunction(_v8NodeObject, args);
}

void
ChromiumDOMEvent::stopPropagation()
{
	CefV8ValueList args;
	getFunction("stopPropagation")->ExecuteFunction(_v8NodeObject, args);
}


std::string
ChromiumDOMEvent::type()
{
	return getProperty("type")->GetStringValue();
}

minko::dom::AbstractDOMElement::Ptr
ChromiumDOMEvent::target()
{
	return ChromiumDOMElement::getDOMElementFromV8Object(getProperty("target"));
}


int
ChromiumDOMEvent::clientX()
{
	return getProperty("clientX")->GetIntValue();
}

int
ChromiumDOMEvent::clientY()
{
	return getProperty("clientY")->GetIntValue();
}


int
ChromiumDOMEvent::pageX()
{
	return getProperty("pageX")->GetIntValue();
}

int
ChromiumDOMEvent::pageY()
{
	return getProperty("pageY")->GetIntValue();
}


int
ChromiumDOMEvent::layerX()
{
	return getProperty("layerX")->GetIntValue();
}

int
ChromiumDOMEvent::layerY()
{
	return getProperty("layerY")->GetIntValue();
}


int
ChromiumDOMEvent::screenX()
{
	return getProperty("screenX")->GetIntValue();
}

int
ChromiumDOMEvent::screenY()
{
	return getProperty("screenY")->GetIntValue();
}