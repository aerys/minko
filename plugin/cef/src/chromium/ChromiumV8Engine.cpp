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

#include "chromium/ChromiumV8Engine.hpp"

using namespace chromium;

ChromiumV8Engine::ChromiumV8Engine() :
	_v8Handler(new ChromiumV8Handler()),
	_onLoad(minko::Signal<std::string>::create()),
	_onMessage(minko::Signal<std::string>::create())
{
}

void
ChromiumV8Engine::initNewPage(CefRefPtr<CefV8Context> context)
{
	_currentV8Context = context;

	_currentV8Context->Enter();

	_minkoObject = CefV8Value::CreateObject(nullptr);
	window()->SetValue("Minko", _minkoObject, V8_PROPERTY_ATTRIBUTE_NONE);

	addLoadEventListener();
	addSendMessageFunction();

	_currentV8Context->Exit();
}


CefRefPtr<CefV8Value>
ChromiumV8Engine::window()
{
	return _currentV8Context->GetGlobal();
}

CefRefPtr<CefV8Value>
ChromiumV8Engine::document()
{
	return _currentV8Context->GetGlobal()->GetValue("document");
}

void
ChromiumV8Engine::addSendMessageFunction()
{
	std::string sendMessageFunctionName = "sendMessage";

	// [javascript] Minko.sendMessage('message');
	CefRefPtr<CefV8Value> sendMessageFunction = CefV8Value::CreateFunction(sendMessageFunctionName, _v8Handler.get());
	_minkoObject->SetValue(sendMessageFunctionName, sendMessageFunction, V8_PROPERTY_ATTRIBUTE_NONE);

	_onMessageSlot = _v8Handler->received()->connect([=](std::string functionName, CefV8ValueList arguments)
	{
		if (functionName == sendMessageFunctionName)
		{
			CefRefPtr<CefV8Value> messageV8Value = arguments[0];
			std::string message = messageV8Value->GetStringValue();

			_onMessage->execute(message);
		}
	});
}

void
ChromiumV8Engine::addLoadEventListener()
{
	std::string loadFunctionName = "onLoadHandler";
	CefRefPtr<CefV8Value> onloadFunction = CefV8Value::CreateFunction(loadFunctionName, _v8Handler.get());

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString("load"));
	args.push_back(onloadFunction);

	window()->GetValue("addEventListener")->ExecuteFunction(nullptr, args);

	_onLoadSlot = _v8Handler->received()->connect([=](std::string functionName, CefV8ValueList)
	{
		//fixme: pass page name, relative to working directory
		if (functionName == loadFunctionName)
			_onLoad->execute("");
	});
}