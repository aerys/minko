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
#include "include/cef_runnable.h"
#include "include/cef_task.h"

using namespace chromium;
using namespace chromium::dom;
using namespace minko;
using namespace minko::dom;

std::map<CefRefPtr<CefV8Value>, ChromiumDOMElement::Ptr>
ChromiumDOMElement::_v8NodeToElement;

std::list<std::function<void()>>
ChromiumDOMElement::_functionList;

std::list<std::function<void()>>
ChromiumDOMElement::_fnHolder;

std::mutex
ChromiumDOMElement::_fnHolderMutex;

std::atomic<int>
ChromiumDOMElement::_waitingFunctions(0);

std::map<ChromiumDOMElement::Ptr, CefRefPtr<CefV8Value>>
ChromiumDOMElement::_elementToV8Object;

ChromiumDOMElement::ChromiumDOMElement(CefRefPtr<CefV8Value> v8NodeObject, CefRefPtr<CefV8Context> v8Context) :
	_onclickCallbackSet(false),
	_onmousedownCallbackSet(false),
	_onmousemoveCallbackSet(false),
	_onmouseupCallbackSet(false),
	_onmouseoverCallbackSet(false),
	_onmouseoutCallbackSet(false),
	_cleared(false),
	_v8Handler(),
	_v8NodeObject(v8NodeObject),
	_v8Context(v8Context),
	_blocker(true)
{
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

ChromiumDOMElement::Ptr
ChromiumDOMElement::create(CefRefPtr<CefV8Value> v8NodeObject, CefRefPtr<CefV8Context> v8Context)
{
	ChromiumDOMElement::Ptr result;
	if (CefCurrentlyOn(TID_RENDERER))
	{
		v8Context->Enter();
		if (v8NodeObject->IsUndefined() || v8NodeObject->IsNull())
			return nullptr;

		ChromiumDOMElement::Ptr element(new ChromiumDOMElement(v8NodeObject, v8Context));
		element->_v8Handler = new ChromiumDOMElementV8Handler(element);
		_v8NodeToElement[v8NodeObject] = element;
		_elementToV8Object[element] = v8NodeObject;
		result = element;
		v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		std::atomic<bool> blocker(true);

		auto fn = [&]()
		{
			result = ChromiumDOMElement::create(v8NodeObject, v8Context);
			blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));
		while (blocker.load());
	}

	return result;
}

ChromiumDOMElement::Ptr
ChromiumDOMElement::getDOMElementFromV8Object(CefRefPtr<CefV8Value> v8Object, CefRefPtr<CefV8Context> v8Context)
{
	for (auto& it : _v8NodeToElement)
	{
		if (v8Object->IsSame(it.first))
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

CefRefPtr<CefV8Value>
ChromiumDOMElement::getFunction(std::string name)
{
	CefRefPtr<CefV8Value> func = _v8NodeObject->GetValue(name);

	if (!func->IsFunction())
		throw;

	return func;
}

CefRefPtr<CefV8Value>
ChromiumDOMElement::getProperty(std::string name)
{
	return _v8NodeObject->GetValue(name);
}

void
ChromiumDOMElement::setProperty(std::string name, CefRefPtr<CefV8Value> value)
{
	_v8NodeObject->SetValue(name, value, V8_PROPERTY_ATTRIBUTE_NONE);
}

std::list<AbstractDOMElement::Ptr>
ChromiumDOMElement::v8ElementArrayToList(CefRefPtr<CefV8Value> v8Nodes, CefRefPtr<CefV8Context> v8Context)
{
	std::list<minko::dom::AbstractDOMElement::Ptr> result;

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


std::string
ChromiumDOMElement::id()
{
	std::string result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = getProperty("id")->GetStringValue();
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = id();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}
	return result;
}

void
ChromiumDOMElement::id(std::string newId)
{
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		setProperty("id", CefV8Value::CreateString(newId));
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			id(newId);
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));
		while (_blocker.load());
	}
}

std::string
ChromiumDOMElement::className()
{
	std::string result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = getProperty("className")->GetStringValue();
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn =[&]()
		{
			result = className();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}
	return result;
}

void
ChromiumDOMElement::className(std::string newClass)
{
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		setProperty("className", CefV8Value::CreateString(newClass));
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			className(newClass);
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));
		while (_blocker.load());
	}
}

std::string
ChromiumDOMElement::tagName()
{
	std::string result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = getProperty("tagName")->GetStringValue();
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = tagName();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}
	return result;
}

AbstractDOMElement::Ptr
ChromiumDOMElement::parentNode()
{
	AbstractDOMElement::Ptr result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		CefRefPtr<CefV8Value> parentNodeV8 = getProperty("parentNode");

		result = getDOMElementFromV8Object(parentNodeV8, _v8Context);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = parentNode();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}
	return result;
}

std::list<AbstractDOMElement::Ptr>
ChromiumDOMElement::childNodes()
{
	std::list<AbstractDOMElement::Ptr> result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = v8ElementArrayToList(getProperty("childNodes"), _v8Context);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = childNodes();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}
	return result;
}

std::string
ChromiumDOMElement::textContent()
{
	std::string result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = getProperty("textContent")->GetStringValue();
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = textContent();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}
	return result;
}

void
ChromiumDOMElement::textContent(std::string content)
{
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		setProperty("textContent", CefV8Value::CreateString(content));
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			textContent(content);
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}
}

std::string
ChromiumDOMElement::innerHTML()
{
	std::string result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = getProperty("innerHTML")->GetStringValue();
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = innerHTML();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}
	return result;
}

void
ChromiumDOMElement::innerHTML(std::string html)
{
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		setProperty("innerHTML", CefV8Value::CreateString(html));
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			innerHTML(html);
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}
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

		auto fn = [&]()
		{
			appendChild(child);
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}

	return shared_from_this();
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

		auto fn = [&]()
		{
			removeChild(child);
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}
	return shared_from_this();
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

		auto fn = [&]()
		{
			insertBefore(newNode, refNode);
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}

	return shared_from_this();
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

		auto fn = [&]()
		{
			element = cloneNode(deep);
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}
	return element;
}

std::string
ChromiumDOMElement::getAttribute(std::string name)
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

		auto fn = [&]()
		{
			result = getAttribute(name);
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}
	return result;
}

void
ChromiumDOMElement::setAttribute(std::string name, std::string value)
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

		auto fn = [&]()
		{
			setAttribute(name, value);
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));
		while (_blocker.load());
	}
}

std::list<AbstractDOMElement::Ptr>
ChromiumDOMElement::getElementsByTagName(std::string tagName)
{
	std::list<AbstractDOMElement::Ptr> list;

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

		auto fn = [&]()
		{
			list = getElementsByTagName(tagName);
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));
		while (_blocker.load());
	}
	return list;
}

std::string
ChromiumDOMElement::style(std::string name)
{
	std::string result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		CefRefPtr<CefV8Value> styleObject = getProperty("style");
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

		auto fn = [&]()
		{
			result = style(name);
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));
		while (_blocker.load());
	}
	return result;
}

void
ChromiumDOMElement::style(std::string name, std::string value)
{
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		getProperty("style")->SetValue(name, CefV8Value::CreateString(value), V8_PROPERTY_ATTRIBUTE_NONE);
		_v8Context->Exit();

		_fnHolderMutex.lock();

		_waitingFunctions.store(_waitingFunctions.load() - 1);

		if (_fnHolder.size() > 0 && _waitingFunctions.load() == 0)
			_fnHolder.clear();

		_fnHolderMutex.unlock();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);

		_fnHolderMutex.lock();
		_waitingFunctions.store(_waitingFunctions.load() + 1);
		
		_fnHolder.push_back(std::bind(
			(void(ChromiumDOMElement::*)(std::string, std::string))&ChromiumDOMElement::style,
			shared_from_this(),
			name,
			value
		));

		runner->PostTask(NewCefRunnableFunction(&(_fnHolder.back())));
		_fnHolderMutex.unlock();
	}
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

		auto fn = [&]()
		{
			addEventListener(type);
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));
		while (_blocker.load());
	}
}

Signal<AbstractDOMEvent::Ptr>::Ptr
ChromiumDOMElement::onclick()
{
	if (!_onclickCallbackSet)
	{
		_onclick = Signal<minko::dom::AbstractDOMEvent::Ptr>::create();
		addEventListener("click");
		_onclickCallbackSet = true;
	}

	return _onclick;
}

Signal<AbstractDOMEvent::Ptr>::Ptr
ChromiumDOMElement::onmousedown()
{
	if (!_onmousedownCallbackSet)
	{
		_onmousedown = Signal<minko::dom::AbstractDOMEvent::Ptr>::create();
		addEventListener("mousedown");
		_onmousedownCallbackSet = true;
	}

	return _onmousedown;
}

Signal<AbstractDOMEvent::Ptr>::Ptr
ChromiumDOMElement::onmousemove()
{
	if (!_onmousemoveCallbackSet)
	{
		_onmousemove = Signal<minko::dom::AbstractDOMEvent::Ptr>::create();
		addEventListener("mousemove");
		_onmousemoveCallbackSet = true;
	}

	return _onmousemove;
}

Signal<AbstractDOMEvent::Ptr>::Ptr
ChromiumDOMElement::onmouseup()
{
	if (!_onmouseupCallbackSet)
	{
		_onmouseup = Signal<minko::dom::AbstractDOMEvent::Ptr>::create();
		addEventListener("mouseup");
		_onmouseupCallbackSet = true;
	}

	return _onmouseup;
}

Signal<AbstractDOMEvent::Ptr>::Ptr
ChromiumDOMElement::onmouseout()
{
	if (!_onmouseoutCallbackSet)
	{
		_onmouseout = Signal<minko::dom::AbstractDOMEvent::Ptr>::create();
		addEventListener("mouseout");
		_onmouseoutCallbackSet = true;
	}

	return _onmouseout;
}

Signal<AbstractDOMEvent::Ptr>::Ptr
ChromiumDOMElement::onmouseover()
{
	if (!_onmouseoverCallbackSet)
	{
		_onmouseover = Signal<minko::dom::AbstractDOMEvent::Ptr>::create();
		addEventListener("mouseover");
		_onmouseoverCallbackSet = true;
	}

	return _onmouseover;
}

void
ChromiumDOMElement::update()
{
	for (auto func : _functionList)
	{
		func();
	}

	_functionList.clear();
}


void
ChromiumDOMElement::addFunction(std::function<void()> func)
{
	_functionList.push_back(func);
}

#endif