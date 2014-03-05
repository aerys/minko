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

#include "minko/RenderProcessHandler.hpp"
#include "minko/CefPimpl.hpp"
#include "include/cef_render_process_handler.h"

class V8Handler : public CefV8Handler
{
public:
	V8Handler() {}

	virtual bool Execute(const CefString& name,
		CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception) OVERRIDE
	{
		if (name == "sendMessage")
		{
			CefRefPtr<CefV8Value> message = arguments[0];

			std::string value = message->GetStringValue();

			//_messageReceived->execute(value);

			std::cout << value << "\n";
		}
		else if (name == "onLoadCallback")
		{

		}
		else if (name == "sendEventCallback")
		{
			CefRefPtr<CefV8Value> callbackId = arguments[0];
			CefRefPtr<CefV8Value> elementId = arguments[1];
			CefRefPtr<CefV8Value> eventType = arguments[2];

			std::string callbackIdValue = callbackId->GetStringValue();
			std::string elementIdValue = elementId->GetStringValue();
			std::string eventTypeValue = eventType->GetStringValue();

		}

		// Function does not exist.
		return false;
	}

private:

	IMPLEMENT_REFCOUNTING(V8Handler);
};


using namespace minko;

RenderProcessHandler::RenderProcessHandler(CefPimpl* impl) :
	_impl(impl)
{
	_loadHandler = new LoadHandler();
}

void
RenderProcessHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	context->Enter();
	CefRefPtr<CefV8Value> object = context->GetGlobal();
		
	CefRefPtr<CefV8Value> minkoObject = CefV8Value::CreateObject(nullptr);

	CefRefPtr<CefV8Handler> handler = new V8Handler();
	CefRefPtr<CefV8Value> sendMessageFunction = CefV8Value::CreateFunction("sendMessage", handler);
	CefRefPtr<CefV8Value> sendEventCallback = CefV8Value::CreateFunction("sendEventCallback", handler);

	minkoObject->SetValue("sendMessage", sendMessageFunction, V8_PROPERTY_ATTRIBUTE_NONE);
	minkoObject->SetValue("sendEventCallback", sendEventCallback, V8_PROPERTY_ATTRIBUTE_NONE);
	minkoObject->SetValue("foo", CefV8Value::CreateString("bar"), V8_PROPERTY_ATTRIBUTE_NONE);

	object->SetValue("Minko", minkoObject, V8_PROPERTY_ATTRIBUTE_NONE);

	CefRefPtr<CefV8Value> retValue = ExecuteFunction("tototiti");
	if (retValue->IsInt())
	{
		int tititoto = retValue->GetIntValue();
		tititoto++;
	}
	context->Exit();
}

CefRefPtr<CefV8Value>
RenderProcessHandler::ExecuteFunction(std::string functionName, CefRefPtr<CefV8Value> object)
{
	CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();

	if (context != nullptr)
	{
		if (object == nullptr)
			object = context->GetGlobal();

		CefV8ValueList args;

		CefRefPtr<CefV8Value> function = object->GetValue(functionName);

		if (function->IsFunction())
			return function->ExecuteFunctionWithContext(context, object, args);
	}
	return CefV8Value::CreateUndefined();
}



CefRefPtr<CefLoadHandler>
RenderProcessHandler::GetLoadHandler()
{
	return _loadHandler;
}