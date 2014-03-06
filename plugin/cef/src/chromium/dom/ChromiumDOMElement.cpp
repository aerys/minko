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

#include "chromium/dom/ChromiumDOMElement.hpp"

using namespace chromium;
using namespace chromium::dom;
using namespace minko;
using namespace minko::dom;

ChromiumDOMElement::ChromiumDOMElement(CefRefPtr<CefV8Value> v8NodeObject) :
	_v8Handler(new ChromiumDOMElementV8Handler())
{
	if (!v8NodeObject->IsObject())
		throw;

	_v8NodeObject = v8NodeObject;
}

ChromiumDOMElement::Ptr
ChromiumDOMElement::initialize()
{
	_v8NodeToElement[_v8NodeObject] = shared_from_this();
	return _v8NodeToElement[_v8NodeObject];
}

ChromiumDOMElement::Ptr
ChromiumDOMElement::create(CefRefPtr<CefV8Value> v8NodeObject)
{
	ChromiumDOMElement* element = new ChromiumDOMElement(v8NodeObject);
	return element->initialize();
}

ChromiumDOMElement::Ptr
ChromiumDOMElement::getDOMElementFromV8Object(CefRefPtr<CefV8Value> v8Object)
{
	std::map<CefRefPtr<CefV8Value>, ChromiumDOMElement::Ptr>::iterator it = _v8NodeToElement.find(v8Object);

	if (it != _v8NodeToElement.end())
		return it->second;
	else
		return create(v8Object);
}


CefRefPtr<CefV8Value>
ChromiumDOMElement::getFunction(std::string name)
{
	CefRefPtr<CefV8Value> func = _v8NodeObject->GetValue(name);

	if (!func->IsFunction())
		func = nullptr;

	return func;
}

CefRefPtr<CefV8Value>
ChromiumDOMElement::getProperty(std::string name)
{
	CefRefPtr<CefV8Value> prop = _v8NodeObject->GetValue(name);

	if (prop->IsFunction())
		prop = nullptr;

	return prop;
}

std::list<AbstractDOMElement::Ptr>
ChromiumDOMElement::v8ElementArrayToList(CefRefPtr<CefV8Value> v8Nodes)
{
	std::list<minko::dom::AbstractDOMElement::Ptr> result;

	if (v8Nodes->IsArray())
	{
		int l = v8Nodes->GetArrayLength();

		for (int i = 0; i < l; ++i)
		{
			CefRefPtr<CefV8Value> childNodeV8 = v8Nodes->GetValue(i);
			result.push_back(getDOMElementFromV8Object(childNodeV8));
		}
	}

	return result;
}


std::string
ChromiumDOMElement::id()
{
	return getProperty("id")->GetStringValue();
}

std::string
ChromiumDOMElement::className()
{
	return getProperty("className")->GetStringValue();
}

std::string
ChromiumDOMElement::tagName()
{
	return getProperty("tagName")->GetStringValue();
}

AbstractDOMElement::Ptr
ChromiumDOMElement::parentNode()
{
	CefRefPtr<CefV8Value> parentNodeV8 = getProperty("parentNode");

	return getDOMElementFromV8Object(parentNodeV8);
}

std::list<AbstractDOMElement::Ptr>
ChromiumDOMElement::childNodes()
{
	return v8ElementArrayToList(getProperty("childNodes"));
}

AbstractDOMElement::Ptr
ChromiumDOMElement::appendChild(AbstractDOMElement::Ptr child)
{
	CefRefPtr<CefV8Value> func = getFunction("appendChild");
	
	CefV8ValueList args;
	args.push_back(_elementToV8Object[child]);

	CefRefPtr<CefV8Value> v8Result = func->ExecuteFunction(_v8NodeObject, args);
	return getDOMElementFromV8Object(v8Result);
}

AbstractDOMElement::Ptr
ChromiumDOMElement::removeChild(AbstractDOMElement::Ptr child)
{
	CefRefPtr<CefV8Value> func = getFunction("removeChild");

	CefV8ValueList args;
	args.push_back(_elementToV8Object[child]);

	CefRefPtr<CefV8Value> v8Result = func->ExecuteFunction(_v8NodeObject, args);
	return getDOMElementFromV8Object(v8Result);
}


AbstractDOMElement::Ptr
ChromiumDOMElement::insertBefore(AbstractDOMElement::Ptr child)
{
	CefRefPtr<CefV8Value> func = getFunction("insertBefore");

	CefV8ValueList args;
	args.push_back(_elementToV8Object[child]);

	CefRefPtr<CefV8Value> v8Result = func->ExecuteFunction(_v8NodeObject, args);
	return getDOMElementFromV8Object(v8Result);
}

AbstractDOMElement::Ptr
ChromiumDOMElement::cloneNode(bool deep)
{
	CefRefPtr<CefV8Value> func = getFunction("cloneNode");

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateBool(deep));

	CefRefPtr<CefV8Value> v8Result = func->ExecuteFunction(_v8NodeObject, args);
	return getDOMElementFromV8Object(v8Result);
}

std::string
ChromiumDOMElement::getAttribute(std::string name)
{
	CefRefPtr<CefV8Value> func = getFunction("getAttribute");

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString(name));

	return func->ExecuteFunction(_v8NodeObject, args)->GetStringValue();
}

void
ChromiumDOMElement::setAttribute(std::string name, std::string value)
{
	CefRefPtr<CefV8Value> func = getFunction("setAttribute");

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString(name));
	args.push_back(CefV8Value::CreateString(value));

	func->ExecuteFunction(_v8NodeObject, args);
}

std::list<AbstractDOMElement::Ptr>
ChromiumDOMElement::getElementsByTagName(std::string tagName)
{
	CefRefPtr<CefV8Value> func = getFunction("setAttribute");

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString(tagName));

	CefRefPtr<CefV8Value> v8Result = func->ExecuteFunction(_v8NodeObject, args);
	return v8ElementArrayToList(v8Result);
}

std::string
ChromiumDOMElement::style(std::string name)
{
	CefRefPtr<CefV8Value> styleProperty = getProperty("style")->GetValue("name");

	if (styleProperty->IsString())
		return styleProperty->GetStringValue();
}

std::string
ChromiumDOMElement::style(std::string name, std::string value)
{
	getProperty("style")->SetValue(name, CefV8Value::CreateString(value), V8_PROPERTY_ATTRIBUTE_NONE);
}

void
ChromiumDOMElement::addEventListener(std::string type)
{
	std::string onClickFunctionName = "on" + type + "Handler";

	CefRefPtr<CefV8Value> onClickFunction = CefV8Value::CreateFunction(onClickFunctionName, _v8Handler.get());

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString(type));
	args.push_back(onClickFunction);

	getFunction("addEventListener")->ExecuteFunction(nullptr, args);
}

Signal<AbstractDOMEvent::Ptr>::Ptr
ChromiumDOMElement::onclick()
{
	if (!_onclickCallbackSet)
		addEventListener("click");

	return _v8Handler->onclick();
}

Signal<AbstractDOMEvent::Ptr>::Ptr
ChromiumDOMElement::onmousedown()
{
	if (!_onmousedownCallbackSet)
		addEventListener("mousedown");

	return _v8Handler->onmousedown();
}

Signal<AbstractDOMEvent::Ptr>::Ptr
ChromiumDOMElement::onmousemove()
{
	if (!_onmousemoveCallbackSet)
		addEventListener("mousemove");

	return _v8Handler->onmousemove();
}

Signal<AbstractDOMEvent::Ptr>::Ptr
ChromiumDOMElement::onmouseup()
{
	if (!_onmouseupCallbackSet)
		addEventListener("mouseup");

	return _v8Handler->onmouseup();
}

Signal<AbstractDOMEvent::Ptr>::Ptr
ChromiumDOMElement::onmouseout()
{
	if (!_onmouseoutCallbackSet)
		addEventListener("mouseout");

	return _v8Handler->onmouseout();
}

Signal<AbstractDOMEvent::Ptr>::Ptr
ChromiumDOMElement::onmouseover()
{
	if (!_onmouseoverCallbackSet)
		addEventListener("mouseover");

	return _v8Handler->onmouseover();
}