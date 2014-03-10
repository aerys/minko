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

#include "minko/Minko.hpp"

#include "chromium/dom/ChromiumDOM.hpp"
#include "chromium/dom/ChromiumDOMEngine.hpp"
#include "chromium/dom/ChromiumDOMElement.hpp"
#include "chromium/dom/ChromiumDOMV8Handler.hpp"

using namespace chromium;
using namespace chromium::dom;
using namespace minko;
using namespace minko::dom;

ChromiumDOM::ChromiumDOM() :
	_v8Handler(new ChromiumDOMV8Handler()),
	_onload(minko::Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(minko::Signal<AbstractDOM::Ptr, std::string>::create()),
	_cleared(false),
	_init(false)
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
	_v8Handler = nullptr;
	_v8Context = nullptr;
	_frame = nullptr;
	_minkoObject = nullptr;

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

	_minkoObject = CefV8Value::CreateObject(nullptr);
	window()->SetValue("Minko", _minkoObject, V8_PROPERTY_ATTRIBUTE_NONE);

	addLoadEventListener();
	addSendMessageFunction();

	_v8Context->Exit();
	_init = true;
}


CefRefPtr<CefV8Value>
ChromiumDOM::window()
{
	return _v8Context->GetGlobal();
}


AbstractDOMElement::Ptr
ChromiumDOM::document()
{
	return ChromiumDOMElement::getDOMElementFromV8Object(_v8Context->GetGlobal()->GetValue("document"));
}

AbstractDOMElement::Ptr
ChromiumDOM::body()
{
	return ChromiumDOMElement::getDOMElementFromV8Object(_v8Context->GetGlobal()->GetValue("document")->GetValue("body"));
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

			_onmessage->execute(shared_from_this(), message);
			_engine->onmessage()->execute(shared_from_this(), message);
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
		//fixme: pass page name, relative to working directory
		if (functionName == loadFunctionName)
		{
			std::string url = _frame->GetURL();
			_onload->execute(shared_from_this(), url);
			_engine->onload()->execute(shared_from_this(), url);
		}
	});
}

AbstractDOMElement::Ptr
ChromiumDOM::createElement(std::string element)
{
	CefRefPtr<CefV8Value> document = window()->GetValue("document");
	CefRefPtr<CefV8Value> func = document->GetValue("createElement");

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString(element));

	CefRefPtr<CefV8Value> result = func->ExecuteFunctionWithContext(_v8Context, document, args);

	return ChromiumDOMElement::create(result);
}

AbstractDOMElement::Ptr
ChromiumDOM::getElementById(std::string id)
{
	CefRefPtr<CefV8Value> document = window()->GetValue("document");
	CefRefPtr<CefV8Value> func = document->GetValue("getElementById");

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString(id));

	CefRefPtr<CefV8Value> result = func->ExecuteFunctionWithContext(_v8Context, document, args);

	return ChromiumDOMElement::getDOMElementFromV8Object(result);
}

std::list<AbstractDOMElement::Ptr>
ChromiumDOM::getElementsByClassName(std::string className)
{
	CefRefPtr<CefV8Value> document = window()->GetValue("document");
	CefRefPtr<CefV8Value> func = document->GetValue("getElementsByClassName");

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString(className));

	CefRefPtr<CefV8Value> result = func->ExecuteFunctionWithContext(_v8Context, document, args);

	return ChromiumDOMElement::v8ElementArrayToList(result);
}

std::list<AbstractDOMElement::Ptr>
ChromiumDOM::getElementsByTagName(std::string tagName)
{
	CefRefPtr<CefV8Value> document = window()->GetValue("document");
	CefRefPtr<CefV8Value> func = document->GetValue("getElementsByTagName");

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString(tagName));

	CefRefPtr<CefV8Value> result = func->ExecuteFunctionWithContext(_v8Context, document, args);

	return ChromiumDOMElement::v8ElementArrayToList(result);
}

