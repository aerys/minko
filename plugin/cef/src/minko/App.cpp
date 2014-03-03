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

#include "minko/App.hpp"
#include "minko/input/Mouse.hpp"
#include "minko/CefPimpl.hpp"
#include "minko/RenderProcessHandler.hpp"
#include "minko/RenderHandler.hpp"
#include "minko/BrowserClient.hpp"
#include "include/cef_command_line.h"

using namespace minko;

App::App()
{
}

std::shared_ptr<render::Texture>
App::initialize(std::shared_ptr<AbstractCanvas> canvas, std::shared_ptr<render::AbstractContext> context, CefPimpl* impl)
{
	_canvas = canvas;
	_context = context;
	_impl = impl;
	_impl->cefRenderHandler = new RenderHandler(_canvas, _context);
	_impl->cefRenderProcessHandler = new RenderProcessHandler(_impl);

	return _impl->cefRenderHandler->renderTexture;
}

void
App::OnContextInitialized()
{
	CefWindowInfo window_info;
	CefBrowserSettings browserSettings;

	browserSettings.web_security = STATE_DISABLED;
	browserSettings.file_access_from_file_urls = STATE_ENABLED;

	CefRefPtr<BrowserClient> browserClient = new BrowserClient(_impl->cefRenderHandler);

	// in linux set a gtk widget, in windows a hwnd. If not available set nullptr - may cause some render errors, in context-menu and plugins.
	window_info.SetAsOffScreen(nullptr);
	window_info.SetTransparentPainting(true);

	_impl->cefBrowser = CefBrowserHost::CreateBrowserSync(window_info, browserClient.get(), "", browserSettings, nullptr);

	_impl->cefBrowser->GetHost()->SetMouseCursorChangeDisabled(false);

	bindControls();
}


void
App::bindControls()
{
	_mouseMoveSlot = _canvas->mouse()->move()->connect([&](input::Mouse::Ptr m, int dx, int dy)
	{
		if (dx == 0 && dy == 0)
			return;

		if (m->x() == 0 || m->y() == 0)
			return;

		CefMouseEvent mouseEvent;
		{
			mouseEvent.x = m->x();
			mouseEvent.y = m->y();
		}

		_impl->cefBrowser->GetHost()->SendMouseMoveEvent(mouseEvent, false);
	});

	_leftDownSlot = _canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr m)
	{
		CefMouseEvent mouseEvent;
		{
			mouseEvent.x = m->x();
			mouseEvent.y = m->y();
		}

		_impl->cefBrowser->GetHost()->SendMouseClickEvent(mouseEvent, CefBrowserHost::MouseButtonType::MBT_LEFT, false, 1);
	});

	_leftUpSlot = _canvas->mouse()->leftButtonUp()->connect([&](input::Mouse::Ptr m)
	{
		CefMouseEvent mouseEvent;
		{
			mouseEvent.x = m->x();
			mouseEvent.y = m->y();
		}

		_impl->cefBrowser->GetHost()->SendMouseClickEvent(mouseEvent, CefBrowserHost::MouseButtonType::MBT_LEFT, true, 1);
	});

	_rightDownSlot = _canvas->mouse()->rightButtonDown()->connect([&](input::Mouse::Ptr m)
	{
		CefMouseEvent mouseEvent;
		{
			mouseEvent.x = m->x();
			mouseEvent.y = m->y();
		}

		_impl->cefBrowser->GetHost()->SendMouseClickEvent(mouseEvent, CefBrowserHost::MouseButtonType::MBT_RIGHT, false, 1);
	});

	_rightUpSlot = _canvas->mouse()->rightButtonUp()->connect([&](input::Mouse::Ptr m)
	{
		CefMouseEvent mouseEvent;
		{
			mouseEvent.x = m->x();
			mouseEvent.y = m->y();
		}

		_impl->cefBrowser->GetHost()->SendMouseClickEvent(mouseEvent, CefBrowserHost::MouseButtonType::MBT_RIGHT, true, 1);
	});

	_middleDownSlot = _canvas->mouse()->middleButtonDown()->connect([&](input::Mouse::Ptr m)
	{
		CefMouseEvent mouseEvent;
		{
			mouseEvent.x = m->x();
			mouseEvent.y = m->y();
		}

		_impl->cefBrowser->GetHost()->SendMouseClickEvent(mouseEvent, CefBrowserHost::MouseButtonType::MBT_MIDDLE, false, 1);
	});

	_middleUpSlot = _canvas->mouse()->middleButtonUp()->connect([&](input::Mouse::Ptr m)
	{
		CefMouseEvent mouseEvent;
		{
			mouseEvent.x = m->x();
			mouseEvent.y = m->y();
		}

		_impl->cefBrowser->GetHost()->SendMouseClickEvent(mouseEvent, CefBrowserHost::MouseButtonType::MBT_MIDDLE, true, 1);
	});
}

CefRefPtr<CefRenderProcessHandler>
App::GetRenderProcessHandler()
{
	return _impl->cefRenderProcessHandler;
}