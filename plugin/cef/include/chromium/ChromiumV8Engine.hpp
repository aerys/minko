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

#include "chromium/ChromiumV8Handler.hpp"
#include "include/cef_render_process_handler.h"

namespace chromium
{
	class ChromiumV8Engine
	{
	public:
		ChromiumV8Engine();

		void
		initNewPage(CefRefPtr<CefV8Context>);

	private:
		void
		addLoadEventListener();

		void
		addSendMessageFunction();

		CefRefPtr<CefV8Value>
		window();

		CefRefPtr<CefV8Value>
		document();

		std::shared_ptr<minko::Signal<std::string>> _onLoad;
		std::shared_ptr<minko::Signal<std::string>> _onMessage;

		minko::Signal<std::string, CefV8ValueList>::Slot _onLoadSlot;
		minko::Signal<std::string, CefV8ValueList>::Slot _onMessageSlot;

		CefRefPtr<CefV8Value> _minkoObject;
		CefRefPtr<ChromiumV8Handler> _v8Handler;
		CefRefPtr<CefV8Context> _currentV8Context;
	};
}