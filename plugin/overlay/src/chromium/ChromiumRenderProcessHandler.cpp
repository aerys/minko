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
#include "chromium/ChromiumRenderProcessHandler.hpp"
#include "chromium/ChromiumPimpl.hpp"
#include "chromium/dom/ChromiumDOM.hpp"
#include "include/cef_render_process_handler.h"

using namespace minko;
using namespace chromium;
using namespace chromium::dom;

ChromiumRenderProcessHandler::ChromiumRenderProcessHandler(ChromiumPimpl* impl) :
	_impl(impl)
{
}

ChromiumRenderProcessHandler::~ChromiumRenderProcessHandler()
{
}

void
ChromiumRenderProcessHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	if (frame->IsMain())
	{
		if (_impl->mainDOM != nullptr && _impl->mainDOM->initialized())
			_impl->mainDOM = ChromiumDOM::create(_impl->domEngine);

		_impl->mainDOM->init(context, frame);
	}
	else
	{
		ChromiumDOM::Ptr newDom = ChromiumDOM::create(_impl->domEngine);
		newDom->init(context, frame);
	}
}
#endif