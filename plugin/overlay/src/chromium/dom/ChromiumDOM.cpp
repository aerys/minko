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
#include "minko/Minko.hpp"

#include "chromium/dom/ChromiumDOM.hpp"
#include "chromium/dom/ChromiumDOMEngine.hpp"
#include "chromium/dom/ChromiumDOMElement.hpp"
#include "chromium/dom/ChromiumDOMV8Handler.hpp"
#include "include/cef_runnable.h"
#include "include/cef_task.h"

using namespace chromium;
using namespace chromium::dom;
using namespace minko;
using namespace minko::dom;


void
getElementByIdHelper(bool* blocker, ChromiumDOM::Ptr dom, ChromiumDOMElement::Ptr* element, std::string id);

ChromiumDOM::ChromiumDOM() :
	_v8Handler(new ChromiumDOMV8Handler()),
	_onload(minko::Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(minko::Signal<AbstractDOM::Ptr, std::string>::create()),
	_cleared(false),
	_init(false),
	_executeOnLoad(false),
	_receivedMessages()
{
}

ChromiumDOM::~ChromiumDOM()
{
	clear();
}

void
ChromiumDOM::clear()
{
	if (_cleared)
		return;

	_engine = nullptr;
	_global = nullptr;
	_v8Handler = nullptr;
	_v8Context = nullptr;
	_frame = nullptr;
	_minkoObject = nullptr;
	_document = nullptr;

	_cleared = true;
}

ChromiumDOM::Ptr
ChromiumDOM::create(ChromiumDOMEngine::Ptr engine)
{
	Ptr dom(new ChromiumDOM());
	dom->_engine = engine;
	engine->registerDom(dom);

	return dom;
}

void
ChromiumDOM::init(CefRefPtr<CefV8Context> context, CefRefPtr<CefFrame> frame)
{
	_v8Context = context;
	_frame = frame;

	_v8Context->Enter();

	_global = _v8Context->GetGlobal();
	_minkoObject = CefV8Value::CreateObject(nullptr);
	_document = _global->GetValue("document");
	window()->SetValue("Minko", _minkoObject, V8_PROPERTY_ATTRIBUTE_NONE);

	addLoadEventListener();
	addSendMessageFunction();

	_v8Context->Exit();
	_init = true;
}


CefRefPtr<CefV8Value>
ChromiumDOM::window()
{
	return _global;
}


AbstractDOMElement::Ptr
ChromiumDOM::document()
{
	return ChromiumDOMElement::getDOMElementFromV8Object(_document);
}

AbstractDOMElement::Ptr
ChromiumDOM::body()
{
	return ChromiumDOMElement::getDOMElementFromV8Object(_document->GetValue("body"));
}

void
ChromiumDOM::sendMessage(std::string message)
{
	ChromiumDOMElement::Ptr element;
	if (CefCurrentlyOn(TID_RENDERER))
	{
		CefRefPtr<CefV8Value> func = _minkoObject->GetValue("onmessage");

		if (func->IsFunction())
		{
			CefV8ValueList args;
			args.push_back(CefV8Value::CreateString(message));

			CefRefPtr<CefV8Value> result = func->ExecuteFunctionWithContext(_v8Context, _minkoObject, args);
		}
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		bool blocker = true;

		runner->PostTask(NewCefRunnableFunction(&[&]()
		{
			sendMessage(message);
			blocker = false;
		}));

		while (blocker);
	}
}
void
ChromiumDOM::addSendMessageFunction()
{
	std::string sendMessageFunctionName = "sendMessage";

	// [javascript] Minko.sendMessage('message');
	CefRefPtr<CefV8Value> sendMessageFunction = CefV8Value::CreateFunction(sendMessageFunctionName, _v8Handler.get());
	_minkoObject->SetValue(sendMessageFunctionName, sendMessageFunction, V8_PROPERTY_ATTRIBUTE_NONE);
	
	_onmessageSlot = _v8Handler->received()->connect([=](std::string functionName, CefV8ValueList arguments)
	{
		if (functionName == sendMessageFunctionName)
		{
			CefRefPtr<CefV8Value> messageV8Value = arguments[0];
			std::string message = messageV8Value->GetStringValue();

			_receivedMessages.push_back(message);
		}
	});
}

void
ChromiumDOM::addLoadEventListener()
{
	std::string loadFunctionName = "onLoadHandler";
	CefRefPtr<CefV8Value> onloadFunction = CefV8Value::CreateFunction(loadFunctionName, _v8Handler.get());

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString("load"));
	args.push_back(onloadFunction);

	window()->GetValue("addEventListener")->ExecuteFunctionWithContext(_v8Context, window(), args);

	_onloadSlot = _v8Handler->received()->connect([=](std::string functionName, CefV8ValueList)
	{
		if (functionName == loadFunctionName)
		{
			std::string url = _frame->GetURL();

			_executeOnLoad = true;
		}
	});
}

bool
ChromiumDOM::update()
{
	for (std::string message : _receivedMessages)
	{
		_onload->execute(shared_from_this(), message);
		_engine->onmessage()->execute(shared_from_this(), message);
	}
	_receivedMessages.clear();

	if (_executeOnLoad)
	{
		_onload->execute(shared_from_this(), fullUrl());
		_engine->onload()->execute(shared_from_this(), fullUrl());
		_executeOnLoad = false;
		return true;
	}

	return false;
}

AbstractDOMElement::Ptr
ChromiumDOM::createElement(std::string tag)
{
	ChromiumDOMElement::Ptr element;
	if (CefCurrentlyOn(TID_RENDERER))
	{
		CefRefPtr<CefV8Value> func = _document->GetValue("createElement");

		CefV8ValueList args;
		args.push_back(CefV8Value::CreateString(tag));

		CefRefPtr<CefV8Value> result = func->ExecuteFunctionWithContext(_v8Context, _document, args);

		element = ChromiumDOMElement::create(result);
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		bool blocker = true;

		runner->PostTask(NewCefRunnableFunction(&[&]()
		{
			element = std::dynamic_pointer_cast<ChromiumDOMElement>(createElement(tag));
			blocker = false;
		}));

		while (blocker);
	}
	return element;
}

AbstractDOMElement::Ptr
ChromiumDOM::getElementById(std::string id)
{
	ChromiumDOMElement::Ptr element;
	if (CefCurrentlyOn(TID_RENDERER))
	{
		CefRefPtr<CefV8Value> func = _document->GetValue("getElementById");

		CefV8ValueList args;
		args.push_back(CefV8Value::CreateString(id));

		CefRefPtr<CefV8Value> result = func->ExecuteFunctionWithContext(_v8Context, _document, args);

		element = ChromiumDOMElement::getDOMElementFromV8Object(result);
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		bool blocker = true;
		
		runner->PostTask(NewCefRunnableFunction(&[&]()
		{
			element = std::dynamic_pointer_cast<ChromiumDOMElement>(getElementById(id));
			blocker = false;
		}));

		while (blocker);
	}
	return element;
}

std::list<AbstractDOMElement::Ptr>
ChromiumDOM::getElementsByClassName(std::string className)
{
	std::list<AbstractDOMElement::Ptr> list;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		CefRefPtr<CefV8Value> func = _document->GetValue("getElementsByClassName");

		CefV8ValueList args;
		args.push_back(CefV8Value::CreateString(className));

		CefRefPtr<CefV8Value> result = func->ExecuteFunctionWithContext(_v8Context, _document, args);
		list = ChromiumDOMElement::v8ElementArrayToList(result);
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		bool blocker = true;

		runner->PostTask(NewCefRunnableFunction(&[&]()
		{
			list = getElementsByClassName(className);
			blocker = false;
		}));

		while (blocker);
	}
	return list;
}

std::list<AbstractDOMElement::Ptr>
ChromiumDOM::getElementsByTagName(std::string tagName)
{
	std::list<AbstractDOMElement::Ptr> list;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		CefRefPtr<CefV8Value> func = _document->GetValue("getElementsByTagName");

		CefV8ValueList args;
		args.push_back(CefV8Value::CreateString(tagName));

		CefRefPtr<CefV8Value> result = func->ExecuteFunctionWithContext(_v8Context, _document, args);

		list = ChromiumDOMElement::v8ElementArrayToList(result);
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		bool blocker = true;

		runner->PostTask(NewCefRunnableFunction(&[&]()
		{
			list = getElementsByTagName(tagName);
			blocker = false;
		}));

		while (blocker);
	}
	return list;
}

std::string
ChromiumDOM::fullUrl()
{
	std::string result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		result = _frame->GetURL();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		bool blocker = true;

		runner->PostTask(NewCefRunnableFunction(&[&]()
		{
			result = fullUrl();
			blocker = false;
		}));

		while (blocker);
	}
	return result;
}
#endif