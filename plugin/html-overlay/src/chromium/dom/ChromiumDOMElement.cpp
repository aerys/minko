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

#include "chromium/dom/ChromiumDOMElement.hpp"
#include "include/cef_task.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/base/cef_bind.h"

using namespace chromium;
using namespace chromium::dom;
using namespace minko;
using namespace minko::dom;

std::map<CefRefPtr<CefV8Value>, ChromiumDOMElement::Ptr>
ChromiumDOMElement::_v8NodeToElement;

std::list<std::function<void()>>
ChromiumDOMElement::_functionList;

std::mutex
ChromiumDOMElement::_functionListMutex;

std::map<ChromiumDOMElement::Ptr, CefRefPtr<CefV8Value>>
ChromiumDOMElement::_elementToV8Object;

ChromiumDOMElement::ChromiumDOMElement(CefRefPtr<CefV8Value> v8NodeObject, CefRefPtr<CefV8Context> v8Context) :
    // _onclickCallbackSet(false),
    // _onmousedownCallbackSet(false),
    // _onmousemoveCallbackSet(false),
    // _onmouseupCallbackSet(false),
    // _onmouseoverCallbackSet(false),
    // _onmouseoutCallbackSet(false),
    _onchangeCallbackSet(false),
    _oninputCallbackSet(false),
	_cleared(false),
	_v8Handler()
{
    _blocker = true;
    _v8NodeObject = v8NodeObject;
    _v8Context = v8Context;
}


ChromiumDOMElement::~ChromiumDOMElement()
{
	clear();
}

void
ChromiumDOMElement::clear()
{
	if (_cleared)
		return;

	if (_v8NodeToElement.find(_v8NodeObject) != _v8NodeToElement.end())
		_v8NodeToElement.erase(_v8NodeObject);

	if (_elementToV8Object.size() != 0)
	{
		std::map<ChromiumDOMElement::Ptr, CefRefPtr<CefV8Value>>::iterator i = _elementToV8Object.begin();

		while (i != _elementToV8Object.end())
		{
			if (i->second == _v8NodeObject)
			{
				_elementToV8Object.erase(i->first);
				break;
			}
			i++;
		}
	}

	_v8Handler = nullptr;
	_v8NodeObject = nullptr;
	_v8Context = nullptr;

	_cleared = true;
}

void
ChromiumDOMElement::createWrapper(CefRefPtr<CefV8Value>* v8NodeObject, CefRefPtr<CefV8Context>* v8Context, std::atomic<bool>* blocker, ChromiumDOMElement::Ptr* result)
{
	*result = ChromiumDOMElement::create(*v8NodeObject, *v8Context);
	blocker->store(false);
}

ChromiumDOMElement::Ptr
ChromiumDOMElement::create(CefRefPtr<CefV8Value> v8NodeObject, CefRefPtr<CefV8Context> v8Context)
{
	ChromiumDOMElement::Ptr result;

	if (v8NodeObject == nullptr)
		return nullptr;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		v8Context->Enter();
		if (v8NodeObject->IsUndefined() || v8NodeObject->IsNull())
		{
			result = nullptr;
		}
		else
		{
			ChromiumDOMElement::Ptr element(new ChromiumDOMElement(v8NodeObject, v8Context));
			element->_v8Handler = new ChromiumDOMElementV8Handler(element);
			_v8NodeToElement[v8NodeObject] = element;
			_elementToV8Object[element] = v8NodeObject;
			result = element;
		}
		v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		std::atomic<bool> blocker(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOMElement::createWrapper, &v8NodeObject, &v8Context, &blocker, &result)));
		while (blocker.load());
	}

	return result;
}

ChromiumDOMElement::Ptr
ChromiumDOMElement::getDOMElementFromV8Object(CefRefPtr<CefV8Value> v8Object, CefRefPtr<CefV8Context> v8Context)
{
	for (auto& it : _v8NodeToElement)
	{
		if (v8Object != nullptr && v8Object->IsSame(it.first))
			return it.second;
	}
	
	return create(v8Object, v8Context);
}

void
ChromiumDOMElement::clearAll()
{
	//Keeping a reference to avoid destructors being called when clearing maps
	//Destructors will be called at the end of clearAll
	std::list<Ptr> l;

	if (_elementToV8Object.size() != 0)
	{
		std::map<Ptr, CefRefPtr<CefV8Value>>::iterator i = _elementToV8Object.begin();

		while (i != _elementToV8Object.end())
		{
			l.push_back(i->first);
			i++;
		}
	}

	for (Ptr element : l)
	{
		element->clear();
	}

	_v8NodeToElement.clear();
	_elementToV8Object.clear();
}

std::vector<AbstractDOMElement::Ptr>
ChromiumDOMElement::v8ElementArrayToList(CefRefPtr<CefV8Value> v8Nodes, CefRefPtr<CefV8Context> v8Context)
{
	std::vector<minko::dom::AbstractDOMElement::Ptr> result;

	CefRefPtr<CefV8Value> childNodeV8;

	if (v8Nodes->IsArray() || v8Nodes->IsObject())
	{
		int l = 0;

		if (v8Nodes->IsArray())
			l = v8Nodes->GetArrayLength();
		else
			l = v8Nodes->GetValue("length")->GetIntValue();

		for (int i = 0; i < l; ++i)
		{
			childNodeV8 = v8Nodes->GetValue(i);
			result.push_back(getDOMElementFromV8Object(childNodeV8, v8Context));
		}
	}

	return result;
}

void
ChromiumDOMElement::appendChildWrapper(AbstractDOMElement::Ptr* child, ChromiumDOMElement* target)
{
	target->appendChild(*child);
	target->_blocker.store(false);
}

AbstractDOMElement::Ptr
ChromiumDOMElement::appendChild(AbstractDOMElement::Ptr child)
{
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		CefRefPtr<CefV8Value> func = getFunction("appendChild");

		if (func == nullptr)
			return nullptr;

		CefV8ValueList args;
		args.push_back(_elementToV8Object[std::dynamic_pointer_cast<ChromiumDOMElement>(child)]);

		func->ExecuteFunction(_v8NodeObject, args);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOMElement::appendChildWrapper, &child, this)));
		while (_blocker.load());
	}

	return shared_from_this();
}

void
ChromiumDOMElement::removeChildWrapper(AbstractDOMElement::Ptr* child, ChromiumDOMElement* target)
{
	target->removeChild(*child);
	target->_blocker.store(false);
}

AbstractDOMElement::Ptr
ChromiumDOMElement::removeChild(AbstractDOMElement::Ptr child)
{
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		CefRefPtr<CefV8Value> func = getFunction("removeChild");

		if (func == nullptr)
			return nullptr;

		CefV8ValueList args;
		args.push_back(_elementToV8Object[std::dynamic_pointer_cast<ChromiumDOMElement>(child)]);

		func->ExecuteFunction(_v8NodeObject, args);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOMElement::removeChildWrapper, &child, this)));
		while (_blocker.load());
	}
	return shared_from_this();
}

void
ChromiumDOMElement::insertBeforeWrapper(AbstractDOMElement::Ptr* newNode, AbstractDOMElement::Ptr* refNode, ChromiumDOMElement* target)
{
	target->insertBefore(*newNode, *refNode);
	target->_blocker.store(false);
}

AbstractDOMElement::Ptr
ChromiumDOMElement::insertBefore(AbstractDOMElement::Ptr newNode, AbstractDOMElement::Ptr refNode)
{
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		CefRefPtr<CefV8Value> func = getFunction("insertBefore");

		if (func == nullptr)
			return nullptr;

		CefV8ValueList args;
		args.push_back(_elementToV8Object[std::dynamic_pointer_cast<ChromiumDOMElement>(newNode)]);
		args.push_back(_elementToV8Object[std::dynamic_pointer_cast<ChromiumDOMElement>(refNode)]);

		func->ExecuteFunction(_v8NodeObject, args);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOMElement::insertBeforeWrapper, &newNode, &refNode, this)));
		while (_blocker.load());
	}

	return shared_from_this();
}

void
ChromiumDOMElement::cloneNodeWrapper(AbstractDOMElement::Ptr* element, bool* deep, ChromiumDOMElement* target)
{
	*element = target->cloneNode(*deep);
	target->_blocker.store(false);
}

AbstractDOMElement::Ptr
ChromiumDOMElement::cloneNode(bool deep)
{
	AbstractDOMElement::Ptr element;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		CefRefPtr<CefV8Value> func = getFunction("cloneNode");

		if (func == nullptr)
			return nullptr;

		CefV8ValueList args;
		args.push_back(CefV8Value::CreateBool(deep));

		CefRefPtr<CefV8Value> v8Result = func->ExecuteFunction(_v8NodeObject, args);
		element = getDOMElementFromV8Object(v8Result, _v8Context);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOMElement::cloneNodeWrapper, &element, &deep, this)));
		while (_blocker.load());
	}
	return element;
}

void
ChromiumDOMElement::getAttributeWrapper(const std::string* name, std::string* result, ChromiumDOMElement* target)
{
	*result = target->getAttribute(*name);
	target->_blocker.store(false);
}

std::string
ChromiumDOMElement::getAttribute(const std::string& name)
{
	std::string result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		CefRefPtr<CefV8Value> func = getFunction("getAttribute");

		if (func == nullptr)
			return nullptr;

		CefV8ValueList args;
		args.push_back(CefV8Value::CreateString(name));

		result = func->ExecuteFunction(_v8NodeObject, args)->GetStringValue();
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOMElement::getAttributeWrapper, &name, &result, this)));
		while (_blocker.load());
	}
	return result;
}

void
ChromiumDOMElement::setAttributeWrapper(const std::string* name, const std::string* value, ChromiumDOMElement* target)
{
	target->setAttribute(*name, *value);
	target->_blocker.store(false);
}

void
ChromiumDOMElement::setAttribute(const std::string& name, const std::string& value)
{
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		CefRefPtr<CefV8Value> func = getFunction("setAttribute");

		if (func == nullptr)
			return;

		CefV8ValueList args;
		args.push_back(CefV8Value::CreateString(name));
		args.push_back(CefV8Value::CreateString(value));

		func->ExecuteFunction(_v8NodeObject, args);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOMElement::setAttributeWrapper, &name, &value, this)));
		while (_blocker.load());
	}
}

void
ChromiumDOMElement::getElementsByTagNameWrapper(const std::string* tagName, std::vector<AbstractDOMElement::Ptr>* list, ChromiumDOMElement* target)
{
	*list = target->getElementsByTagName(*tagName);
	target->_blocker.store(false);
}

std::vector<AbstractDOMElement::Ptr>
ChromiumDOMElement::getElementsByTagName(const std::string& tagName)
{
	std::vector<AbstractDOMElement::Ptr> list;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		CefRefPtr<CefV8Value> func = getFunction("setAttribute");

		if (func != nullptr)
		{
			CefV8ValueList args;
			args.push_back(CefV8Value::CreateString(tagName));

			CefRefPtr<CefV8Value> v8Result = func->ExecuteFunction(_v8NodeObject, args);
			list = v8ElementArrayToList(v8Result, _v8Context);
		}
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOMElement::getElementsByTagNameWrapper, &tagName, &list, this)));
		while (_blocker.load());
	}
	return list;
}

void
ChromiumDOMElement::styleResultWrapper(const std::string* name, std::string* result, ChromiumDOMElement* target)
{
	*result = target->style(*name);
	target->_blocker.store(false);
}

std::string
ChromiumDOMElement::style(const std::string& name)
{
	std::string result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
        CefRefPtr<CefV8Value> styleObject = getV8Property<CefRefPtr<CefV8Value>>("style");
		CefRefPtr<CefV8Value> styleProperty = styleObject->GetValue(name);

		if (styleProperty->IsString())
			result = styleProperty->GetStringValue();
		else
			result = "";
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOMElement::styleResultWrapper, &name, &result, this)));
		while (_blocker.load());
	}
	return result;
}

void
ChromiumDOMElement::styleValueWrapper(const std::string* name, const std::string* value, ChromiumDOMElement* target)
{
	target->style(*name, *value);
	target->_blocker.store(false);
}

void
ChromiumDOMElement::style(const std::string& name, const std::string& value)
{
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
        getV8Property<CefRefPtr<CefV8Value>>("style")->SetValue(name, CefV8Value::CreateString(value), V8_PROPERTY_ATTRIBUTE_NONE);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOMElement::styleValueWrapper, &name, &value, this)));
		while (_blocker.load());
	}
}

void
ChromiumDOMElement::addEventListenerWrapper(std::string* type, ChromiumDOMElement* target)
{
	// target->addEventListener(*type);
	// target->_blocker.store(false);
}

void
ChromiumDOMElement::addEventListener(std::string type)
{
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		static int i = 0;

		std::string onClickFunctionName = "on" + type + "Handler" + std::to_string(i++);

		CefRefPtr<CefV8Value> onClickFunction = CefV8Value::CreateFunction(onClickFunctionName, _v8Handler.get());

		CefV8ValueList args;
		args.push_back(CefV8Value::CreateString(type));
		args.push_back(onClickFunction);

		getFunction("addEventListener")->ExecuteFunction(_v8NodeObject, args);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOMElement::addEventListenerWrapper, &type, this)));
		while (_blocker.load());
	}
}

// Signal<AbstractDOMMouseEvent::Ptr>::Ptr
// ChromiumDOMElement::onclick()
// {
// 	if (!_onclickCallbackSet)
// 	{
// 		_onclick = Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::create();
// 		addEventListener("click");
// 		_onclickCallbackSet = true;
// 	}

// 	return _onclick;
// }

// Signal<AbstractDOMMouseEvent::Ptr>::Ptr
// ChromiumDOMElement::onmousedown()
// {
// 	if (!_onmousedownCallbackSet)
// 	{
// 		_onmousedown = Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::create();
// 		addEventListener("mousedown");
// 		_onmousedownCallbackSet = true;
// 	}

// 	return _onmousedown;
// }

// Signal<AbstractDOMMouseEvent::Ptr>::Ptr
// ChromiumDOMElement::onmousemove()
// {
// 	if (!_onmousemoveCallbackSet)
// 	{
// 		_onmousemove = Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::create();
// 		addEventListener("mousemove");
// 		_onmousemoveCallbackSet = true;
// 	}

// 	return _onmousemove;
// }

// Signal<AbstractDOMMouseEvent::Ptr>::Ptr
// ChromiumDOMElement::onmouseup()
// {
// 	if (!_onmouseupCallbackSet)
// 	{
// 		_onmouseup = Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::create();
// 		addEventListener("mouseup");
// 		_onmouseupCallbackSet = true;
// 	}

// 	return _onmouseup;
// }

// Signal<AbstractDOMMouseEvent::Ptr>::Ptr
// ChromiumDOMElement::onmouseout()
// {
// 	if (!_onmouseoutCallbackSet)
// 	{
// 		_onmouseout = Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::create();
// 		addEventListener("mouseout");
// 		_onmouseoutCallbackSet = true;
// 	}

// 	return _onmouseout;
// }

// Signal<AbstractDOMMouseEvent::Ptr>::Ptr
// ChromiumDOMElement::onmouseover()
// {
//     if (!_onmouseoverCallbackSet)
//     {
//         _onmouseover = Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::create();
//         addEventListener("mouseover");
//         _onmouseoverCallbackSet = true;
//     }

//     return _onmouseover;
// }

Signal<AbstractDOMEvent::Ptr>::Ptr
ChromiumDOMElement::onchange()
{
    if (!_onchangeCallbackSet)
    {
        _onchange = Signal<minko::dom::AbstractDOMEvent::Ptr>::create();
        addEventListener("change");
        _onchangeCallbackSet = true;
    }

    return _onchange;
}

Signal<AbstractDOMEvent::Ptr>::Ptr
ChromiumDOMElement::oninput()
{
    if (!_oninputCallbackSet)
    {
        _oninput = Signal<minko::dom::AbstractDOMEvent::Ptr>::create();
        addEventListener("input");
        _oninputCallbackSet = true;
    }

    return _oninput;
}

Signal<AbstractDOMTouchEvent::Ptr>::Ptr
ChromiumDOMElement::ontouchstart()
{
    if (!_ontouchstartCallbackSet)
    {
        _ontouchstart = Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::create();
        addEventListener("touchstart");
        _ontouchstartCallbackSet = true;
    }

    return _ontouchstart;
}

Signal<AbstractDOMTouchEvent::Ptr>::Ptr
ChromiumDOMElement::ontouchend()
{
    if (!_ontouchendCallbackSet)
    {
        _ontouchend = Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::create();
        addEventListener("touchend");
        _ontouchendCallbackSet = true;
    }

    return _ontouchend;
}

Signal<AbstractDOMTouchEvent::Ptr>::Ptr
ChromiumDOMElement::ontouchmove()
{
    if (!_ontouchstartCallbackSet)
    {
        _ontouchmove = Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::create();
        addEventListener("touchmove");
        _ontouchmoveCallbackSet = true;
    }

    return _ontouchmove;
}

void
ChromiumDOMElement::update()
{
	_functionListMutex.lock();

	std::list<std::function<void()>> functionList;

	for (auto func : _functionList)
		functionList.push_back(func);

	_functionList.clear();
	_functionListMutex.unlock();

	for (auto func : functionList)
	{
		func();
	}
}


void
ChromiumDOMElement::addFunction(std::function<void()> func)
{
	_functionListMutex.lock();
	_functionList.push_back(func);
	_functionListMutex.unlock();
}

#endif