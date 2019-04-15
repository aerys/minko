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
#include "include/cef_task.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/base/cef_bind.h"

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
	_receivedMessages(),
	_receivedMessagesMutex(),
	_blocker(true)
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
	_minkoObject = CefV8Value::CreateObject(nullptr, nullptr);
    _minkoObject->SetValue("ready", CefV8Value::CreateBool(false), V8_PROPERTY_ATTRIBUTE_NONE);

	_document = _global->GetValue("document");
	window()->SetValue("Minko", _minkoObject, V8_PROPERTY_ATTRIBUTE_NONE);

    _minkoObject->SetValue("platform", CefV8Value::CreateString("chromium"), V8_PROPERTY_ATTRIBUTE_NONE);

	addLoadEventListener();
	addSendMessageFunction();
    addDispatchMessageFunction();

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
	return ChromiumDOMElement::getDOMElementFromV8Object(_document, _v8Context);
}

AbstractDOMElement::Ptr
ChromiumDOM::body()
{
	return ChromiumDOMElement::getDOMElementFromV8Object(_document->GetValue("body"), _v8Context);
}

void
ChromiumDOM::sendMessageWrapper(const std::string message, ChromiumDOM* target)
{
	target->sendMessage(message);
	target->_blocker.store(false);
}

void
ChromiumDOM::sendMessage(const std::string& message, bool async)
{
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();

		CefRefPtr<CefV8Value> func = _minkoObject->GetValue("dispatchMessage");

		if (func->IsFunction())
		{
			CefV8ValueList args;
			args.push_back(CefV8Value::CreateString(message));
			CefRefPtr<CefV8Value> result = func->ExecuteFunction(_minkoObject, args);
		}
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOM::sendMessageWrapper, message, this)));
	}
}

void
ChromiumDOM::evalWrapper(const std::string* message, ChromiumDOM* target)
{
	target->eval(*message);
	target->_blocker.store(false);
}

void
ChromiumDOM::eval(const std::string& message, bool async)
{
	ChromiumDOMElement::Ptr element;
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();

		const std::string url = "";
		CefRefPtr<CefV8Value> returnValue;
		CefRefPtr<CefV8Exception> exceptionValue;

		_v8Context->Eval(message, url, 42, returnValue, exceptionValue);
		
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		//if (async)
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOM::evalWrapper, &message, this)));

		//if (async)
		while (_blocker.load());
	}
}

void
ChromiumDOM::addSendMessageFunction()
{
	std::string sendMessageFunctionName = "sendMessage";

	// [javascript] Minko.sendMessage('message');
	CefRefPtr<CefV8Value> sendMessageFunction = CefV8Value::CreateFunction(sendMessageFunctionName, _v8Handler.get());
	_minkoObject->SetValue(sendMessageFunctionName, sendMessageFunction, V8_PROPERTY_ATTRIBUTE_NONE);

	CefRefPtr<CefV8Value> alertFunction = CefV8Value::CreateFunction("alert", _v8Handler.get());
	window()->SetValue("alert", alertFunction, V8_PROPERTY_ATTRIBUTE_NONE);
	
	_onmessageSlot = _v8Handler->received()->connect([=](std::string functionName, CefV8ValueList arguments)
	{
		if (functionName == sendMessageFunctionName)
		{
			CefRefPtr<CefV8Value> messageV8Value = arguments[0];
			std::string message = messageV8Value->GetStringValue();

			_receivedMessagesMutex.lock();
			_receivedMessages.push_back(message);
			_receivedMessagesMutex.unlock();
		}
		else if (functionName == "alert")
		{
			CefRefPtr<CefV8Value> messageV8Value = arguments[0];
			std::string message = messageV8Value->GetStringValue();

			std::cout << "alert: " << message << std::endl;
		}
	});
}

void
ChromiumDOM::addDispatchMessageFunction()
{
    std::string code = "";
    code += "Minko.listeners = {};";
    code += "Minko.addEventListener = function(type, callback)";
    code += "{";
    code += "   if (!(Minko.listeners[type]))";
    code += "       Minko.listeners[type] = [];";

    code += "   Minko.listeners[type].push(callback);";
    code += "};";

    code += "Minko.removeEventListener = function(type, callback)";
    code += "{";
    code += "   if (!(Minko.listeners[type]))";
    code += "       Minko.listeners[type] = [];";
    
    code += "   var index = Minko.listeners[type].indexOf(callback);";
    
    code += "   if (index != -1)";
    code += "       Minko.listeners[type].splice(index, 1);";
    code += "};";

    code += "Minko.dispatchEvent = function(event)";
    code += "{";
    code += "   var callbacks = Minko.listeners[event.type];";

    code += "   if (!callbacks)";
    code += "       return;";

    code += "   for(var i = 0; i < callbacks.length; ++i) {";
    code += "       callbacks[i](event);";
	code += "	}";
    code += "};";

    code += "Minko.dispatchMessage = function(message)";
    code += "{";
    code += "	var ev = document.createEvent(\"Event\");";
    code += "	ev.initEvent(\"message\", true, true);";
    code += "	ev.message = message;";

	code += "	Minko.dispatchEvent(ev);";
    code += "   if(Minko.onmessage)";
    code += "	    Minko.onmessage(message);";
    code += "};";

    eval(code);
}

void
ChromiumDOM::addLoadEventListener()
{
	std::string loadFunctionName = "onLoadHandler";
	CefRefPtr<CefV8Value> onloadFunction = CefV8Value::CreateFunction(loadFunctionName, _v8Handler.get());

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString("load"));
	args.push_back(onloadFunction);

	window()->GetValue("addEventListener")->ExecuteFunction(window(), args);

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
	ChromiumDOMElement::update();

	_receivedMessagesMutex.lock();

	std::list<std::string> messages;

	for (std::string message : _receivedMessages)
		messages.push_back(message);

	_receivedMessages.clear();
	_receivedMessagesMutex.unlock();

	for (std::string message : messages)
	{
		_onmessage->execute(shared_from_this(), message);
		_engine->onmessage()->execute(shared_from_this(), message);
	}

	if (_executeOnLoad)
	{
		_onload->execute(shared_from_this(), fullUrl());
		_engine->onload()->execute(shared_from_this(), fullUrl());
		_executeOnLoad = false;

        std::string code = "";

        code += "Minko.ready = true;";
        code += "var ev = document.createEvent('Event');\n";
        code += "ev.initEvent('minkoReady', true, true);\n";
        code += "window.dispatchEvent(ev);";

        eval(code);

		return true;
	}
	
	return false;
}

void
ChromiumDOM::createElementWrapper(const std::string* tag, ChromiumDOMElement::Ptr* element, ChromiumDOM* target)
{
	*element = std::dynamic_pointer_cast<ChromiumDOMElement>(target->createElement(*tag));
	target->_blocker.store(false);
}

AbstractDOMElement::Ptr
ChromiumDOM::createElement(const std::string& tag)
{
	ChromiumDOMElement::Ptr element;
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		CefRefPtr<CefV8Value> func = _document->GetValue("createElement");

		CefV8ValueList args;
		args.push_back(CefV8Value::CreateString(tag));

		CefRefPtr<CefV8Value> result = func->ExecuteFunction(_document, args);

		element = ChromiumDOMElement::create(result, _v8Context);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOM::createElementWrapper, &tag, &element, this)));

		while (_blocker.load());
	}
	return element;
}

void
ChromiumDOM::getElementByIdWrapper(const std::string* id, ChromiumDOMElement::Ptr* element, ChromiumDOM* target)
{
	*element = std::dynamic_pointer_cast<ChromiumDOMElement>(target->getElementById(*id));
	target->_blocker.store(false);
}

AbstractDOMElement::Ptr
ChromiumDOM::getElementById(const std::string& id)
{
	ChromiumDOMElement::Ptr element;
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		CefRefPtr<CefV8Value> func = _document->GetValue("getElementById");

		CefV8ValueList args;
		args.push_back(CefV8Value::CreateString(id));

		CefRefPtr<CefV8Value> result = func->ExecuteFunction(_document, args);

		element = ChromiumDOMElement::getDOMElementFromV8Object(result, _v8Context);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOM::getElementByIdWrapper, &id, &element, this)));

		while (_blocker.load());
	}
	return element;
}

void
ChromiumDOM::getElementsByClassNameWrapper(const std::string* className, std::vector<AbstractDOMElement::Ptr>* list, ChromiumDOM* target)
{
	*list = target->getElementsByClassName(*className);
	target->_blocker.store(false);
}

std::vector<AbstractDOMElement::Ptr>
ChromiumDOM::getElementsByClassName(const std::string& className)
{
	std::vector<AbstractDOMElement::Ptr> list;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		CefRefPtr<CefV8Value> func = _document->GetValue("getElementsByClassName");

		CefV8ValueList args;
		args.push_back(CefV8Value::CreateString(className));

		CefRefPtr<CefV8Value> result = func->ExecuteFunction(_document, args);
		list = ChromiumDOMElement::v8ElementArrayToList(result, _v8Context);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOM::getElementsByClassNameWrapper, &className, &list, this)));

		while (_blocker.load());
	}
	return list;
}

void
ChromiumDOM::getElementsByTagNameWrapper(const std::string* tagName, std::vector<AbstractDOMElement::Ptr>* list, ChromiumDOM* target)
{
	*list = target->getElementsByTagName(*tagName);
	target->_blocker.store(false);
}

std::vector<AbstractDOMElement::Ptr>
ChromiumDOM::getElementsByTagName(const std::string& tagName)
{
	std::vector<AbstractDOMElement::Ptr> list;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		CefRefPtr<CefV8Value> func = _document->GetValue("getElementsByTagName");

		CefV8ValueList args;
		args.push_back(CefV8Value::CreateString(tagName));

		CefRefPtr<CefV8Value> result = func->ExecuteFunction(_document, args);

		list = ChromiumDOMElement::v8ElementArrayToList(result, _v8Context);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOM::getElementsByTagNameWrapper, &tagName, &list, this)));

		while (_blocker.load());
	}
	return list;
}

void
ChromiumDOM::fullUrlWrapper(std::string* result, std::atomic<bool>* blocker, ChromiumDOM* target)
{
	*result = target->fullUrl();
	target->_blocker.store(false);
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
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&fullUrlWrapper, &result, &_blocker, this)));

		while (_blocker.load());
	}
	return result;
}

void
ChromiumDOM::isMainWrapper(bool* result, std::atomic<bool>* blocker, ChromiumDOM* target)
{
	*result = target->isMain();
	target->_blocker.store(false);
}

bool
ChromiumDOM::isMain()
{
	bool result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		result = _frame->IsMain();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		runner->PostTask(CefCreateClosureTask(base::Bind(&ChromiumDOM::isMainWrapper, &result, &_blocker, this)));

		while (_blocker.load());
	}
	return result;
}
#endif