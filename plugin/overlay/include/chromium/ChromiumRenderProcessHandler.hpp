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
#pragma once

#include "minko/Common.hpp"
#include "include/cef_render_process_handler.h"
#include "minko/Signal.hpp"

namespace chromium
{
	class ChromiumPimpl;

	class ChromiumRenderProcessHandler : public CefRenderProcessHandler
	{
	public:
		ChromiumRenderProcessHandler(ChromiumPimpl* impl);
		~ChromiumRenderProcessHandler();
	public:
		virtual
		void
		OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;

		static
		CefRefPtr<CefV8Value>
		ExecuteFunction(std::string, CefRefPtr<CefV8Value> = nullptr);

	private:
		ChromiumPimpl*  _impl;

		IMPLEMENT_REFCOUNTING(ChromiumRenderProcessHandler);
	};
}
#endif