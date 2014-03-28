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

#include "chromium/dom/ChromiumDOMEvent.hpp"
#include "minko/dom/AbstractDOMEvent.hpp"
#include "chromium/dom/ChromiumDOMElement.hpp"
#include "include/cef_runnable.h"
#include "include/cef_task.h"

using namespace chromium;
using namespace chromium::dom;
using namespace minko;
using namespace minko::dom;

std::list<ChromiumDOMEvent::Ptr>
ChromiumDOMEvent::_events;

ChromiumDOMEvent::ChromiumDOMEvent(CefRefPtr<CefV8Value> v8NodeObject, CefRefPtr<CefV8Context> v8Context):
	_cleared(false),
	_v8Context(v8Context),
	_blocker(false)
{
	if (!v8NodeObject->IsObject())
		throw;

	_v8NodeObject = v8NodeObject;
}

ChromiumDOMEvent::~ChromiumDOMEvent()
{
	clear();
}

void
ChromiumDOMEvent::clear()
{
	if (_cleared)
		return;

	_v8NodeObject = nullptr;
	_cleared = true;
}


void
ChromiumDOMEvent::clearAll()
{
	for (Ptr event : _events)
	{
		event->clear();
	}

	_events.clear();
}

ChromiumDOMEvent::Ptr
ChromiumDOMEvent::create(CefRefPtr<CefV8Value> v8NodeObject, CefRefPtr<CefV8Context> v8Context)
{
	ChromiumDOMEvent::Ptr event(new ChromiumDOMEvent(v8NodeObject, v8Context));
	_events.push_back(event);
	return event;
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
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		CefV8ValueList args;
		getFunction("preventDefault")->ExecuteFunction(_v8NodeObject, args);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			preventDefault();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}
}

void
ChromiumDOMEvent::stopPropagation()
{
	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		CefV8ValueList args;
		getFunction("stopPropagation")->ExecuteFunction(_v8NodeObject, args);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			stopPropagation();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}
}


std::string
ChromiumDOMEvent::type()
{
	std::string result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = getProperty("type")->GetStringValue();
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = type();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}

	return result;
}

AbstractDOMElement::Ptr
ChromiumDOMEvent::target()
{
	AbstractDOMElement::Ptr result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = ChromiumDOMElement::getDOMElementFromV8Object(getProperty("target"), _v8Context);
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = target();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}

	return result;
}


int
ChromiumDOMEvent::clientX()
{
	int result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = getProperty("clientX")->GetIntValue();
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = clientX();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}

	return result;
}

int
ChromiumDOMEvent::clientY()
{
	int result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = getProperty("clientY")->GetIntValue();
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = clientY();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}

	return result;
}


int
ChromiumDOMEvent::pageX()
{
	int result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = getProperty("pageX")->GetIntValue();
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = pageX();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}

	return result;
}

int
ChromiumDOMEvent::pageY()
{
	int result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = getProperty("pageY")->GetIntValue();
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = pageY();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}

	return result;
}


int
ChromiumDOMEvent::layerX()
{
	int result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = getProperty("layerX")->GetIntValue();
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = layerX();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}

	return result;
}

int
ChromiumDOMEvent::layerY()
{
	int result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = getProperty("layerY")->GetIntValue();
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = layerY();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}

	return result;
}


int
ChromiumDOMEvent::screenX()
{
	int result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = getProperty("screenX")->GetIntValue();
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = screenX();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}

	return result;
}

int
ChromiumDOMEvent::screenY()
{
	int result;

	if (CefCurrentlyOn(TID_RENDERER))
	{
		_v8Context->Enter();
		result = getProperty("screenY")->GetIntValue();
		_v8Context->Exit();
	}
	else
	{
		CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
		_blocker.store(true);

		auto fn = [&]()
		{
			result = screenY();
			_blocker.store(false);
		};

		runner->PostTask(NewCefRunnableFunction(&fn));

		while (_blocker.load());
	}

	return result;
}
#endif