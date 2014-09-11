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
#include "chromium/ChromiumApp.hpp"
#include "minko/input/Mouse.hpp"
#include "minko/input/Keyboard.hpp"
#include "chromium/ChromiumPimpl.hpp"
#include "chromium/ChromiumRenderProcessHandler.hpp"
#include "chromium/ChromiumRenderHandler.hpp"
#include "chromium/ChromiumClient.hpp"
#include "include/cef_command_line.h"

using namespace minko;
using namespace chromium;

ChromiumApp::~ChromiumApp()
{
}

std::shared_ptr<render::Texture>
ChromiumApp::initialize(std::shared_ptr<AbstractCanvas> canvas, std::shared_ptr<render::AbstractContext> context, ChromiumPimpl* impl)
{
	_canvas = canvas;
	_context = context;
	_impl = impl;
	_impl->renderHandler = new ChromiumRenderHandler(_canvas, _context);
	_impl->renderProcessHandler = new ChromiumRenderProcessHandler(_impl);

	return _impl->renderHandler->renderTexture;
}

void
ChromiumApp::OnContextInitialized()
{
	CefWindowInfo window_info;
	CefBrowserSettings browserSettings;

	browserSettings.web_security = STATE_DISABLED;
	browserSettings.file_access_from_file_urls = STATE_ENABLED;
	browserSettings.universal_access_from_file_urls = STATE_ENABLED;
	browserSettings.webgl = STATE_DISABLED;

	CefRefPtr<ChromiumClient> browserClient = new ChromiumClient(_impl);

	// in linux set a gtk widget, in windows a hwnd. If not available set nullptr - may cause some render errors, in context-menu and plugins.
	window_info.SetAsOffScreen(nullptr);
	window_info.SetTransparentPainting(true);

	_impl->browser = CefBrowserHost::CreateBrowserSync(window_info, browserClient.get(), "", browserSettings, nullptr);

    _impl->browser->GetHost()->SetMouseCursorChangeDisabled(false);
    _impl->browser->GetHost()->SetFocus(true);

	bindControls();
}


void
ChromiumApp::bindControls()
{
	_mouseMoveSlot = _canvas->mouse()->move()->connect([&](input::Mouse::Ptr m, int dx, int dy)
	{
		if (!_enableInput)
			return;

		if (dx == 0 && dy == 0)
			return;

		if (m->x() == 0 || m->y() == 0)
			return;

		CefMouseEvent mouseEvent;
		{
			mouseEvent.x = m->x();
			mouseEvent.y = m->y();
		}

		_impl->browser->GetHost()->SendMouseMoveEvent(mouseEvent, false);
	});

	_leftDownSlot = _canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr m)
	{
		if (!_enableInput)
			return;
		CefMouseEvent mouseEvent;
		{
			mouseEvent.x = m->x();
			mouseEvent.y = m->y();
		}

        _impl->browser->GetHost()->SendMouseClickEvent(mouseEvent, CefBrowserHost::MouseButtonType::MBT_LEFT, false, 1);
        _impl->browser->GetHost()->SendFocusEvent(true);
	});

	_leftUpSlot = _canvas->mouse()->leftButtonUp()->connect([&](input::Mouse::Ptr m)
	{
		if (!_enableInput)
			return;
		CefMouseEvent mouseEvent;
		{
			mouseEvent.x = m->x();
			mouseEvent.y = m->y();
		}

		_impl->browser->GetHost()->SendMouseClickEvent(mouseEvent, CefBrowserHost::MouseButtonType::MBT_LEFT, true, 1);
	});

	_rightDownSlot = _canvas->mouse()->rightButtonDown()->connect([&](input::Mouse::Ptr m)
	{
		if (!_enableInput)
			return;
		CefMouseEvent mouseEvent;
		{
			mouseEvent.x = m->x();
			mouseEvent.y = m->y();
		}

        _impl->browser->GetHost()->SendMouseClickEvent(mouseEvent, CefBrowserHost::MouseButtonType::MBT_RIGHT, false, 1);
        _impl->browser->GetHost()->SendFocusEvent(true);
	});

	_rightUpSlot = _canvas->mouse()->rightButtonUp()->connect([&](input::Mouse::Ptr m)
	{
		if (!_enableInput)
			return;
		CefMouseEvent mouseEvent;
		{
			mouseEvent.x = m->x();
			mouseEvent.y = m->y();
		}

        _impl->browser->GetHost()->SendMouseClickEvent(mouseEvent, CefBrowserHost::MouseButtonType::MBT_RIGHT, true, 1);
	});

	_middleDownSlot = _canvas->mouse()->middleButtonDown()->connect([&](input::Mouse::Ptr m)
	{
		if (!_enableInput)
			return;
		CefMouseEvent mouseEvent;
		{
			mouseEvent.x = m->x();
			mouseEvent.y = m->y();
		}

        _impl->browser->GetHost()->SendMouseClickEvent(mouseEvent, CefBrowserHost::MouseButtonType::MBT_MIDDLE, false, 1);
        _impl->browser->GetHost()->SendFocusEvent(true);
	});

	_middleUpSlot = _canvas->mouse()->middleButtonUp()->connect([&](input::Mouse::Ptr m)
	{
		if (!_enableInput)
			return;
		CefMouseEvent mouseEvent;
		{
			mouseEvent.x = m->x();
			mouseEvent.y = m->y();
		}

		_impl->browser->GetHost()->SendMouseClickEvent(mouseEvent, CefBrowserHost::MouseButtonType::MBT_MIDDLE, true, 1);
    });

    _keyDownSlot = _canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr keyboard)
    {
        if (!_enableInput)
            return;

        for (uint key = 0; key < input::Keyboard::NUM_KEYS; ++key)
        {
            if (_canvas->keyboard()->keyIsDown(static_cast<input::Keyboard::Key>(key)))
            {
                if (_keyIsDown.find(key) == _keyIsDown.end() || !_keyIsDown[key])
                {
                    _keyIsDown[key] = true;

                    CefKeyEvent keyEvent;

                    keyEvent.windows_key_code = key;
                    keyEvent.type = KEYEVENT_KEYDOWN;
                    keyEvent.modifiers = 0;

                    if (_canvas->keyboard()->keyIsDown(input::Keyboard::SHIFT) || _canvas->keyboard()->keyIsDown(input::Keyboard::SHIFT_RIGHT))
                        keyEvent.modifiers |= EVENTFLAG_SHIFT_DOWN;

                    if (_canvas->keyboard()->keyIsDown(input::Keyboard::ALT))
                        keyEvent.modifiers |= EVENTFLAG_ALT_DOWN;

                    if (_canvas->keyboard()->keyIsDown(input::Keyboard::CONTROL) || _canvas->keyboard()->keyIsDown(input::Keyboard::CONTROL_RIGHT))
                        keyEvent.modifiers |= EVENTFLAG_CONTROL_DOWN;

                    if (_canvas->keyboard()->keyIsDown(input::Keyboard::WIN))
                        keyEvent.modifiers |= EVENTFLAG_COMMAND_DOWN;

                    _impl->browser->GetHost()->SendKeyEvent(keyEvent);
                }
            }
        }
    });

    _keyDownSlot = _canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr keyboard)
    {
        if (!_enableInput)
            return;

        for (uint key = 0; key < input::Keyboard::NUM_KEYS; ++key)
        {
            if (_canvas->keyboard()->keyIsDown(static_cast<input::Keyboard::Key>(key)))
            {
                if (_keyIsDown.find(key) == _keyIsDown.end() || !_keyIsDown[key])
                {
                    _keyIsDown[key] = true;

                    CefKeyEvent keyEvent;
                    
                    keyEvent.windows_key_code = key;
                    keyEvent.type = KEYEVENT_KEYDOWN;
                    keyEvent.modifiers = 0;

                    if (_canvas->keyboard()->keyIsDown(input::Keyboard::SHIFT) || _canvas->keyboard()->keyIsDown(input::Keyboard::SHIFT_RIGHT))
                        keyEvent.modifiers |= EVENTFLAG_SHIFT_DOWN;

                    if (_canvas->keyboard()->keyIsDown(input::Keyboard::ALT))
                        keyEvent.modifiers |= EVENTFLAG_ALT_DOWN;

                    if (_canvas->keyboard()->keyIsDown(input::Keyboard::CONTROL) || _canvas->keyboard()->keyIsDown(input::Keyboard::CONTROL_RIGHT))
                        keyEvent.modifiers |= EVENTFLAG_CONTROL_DOWN;

                    if (_canvas->keyboard()->keyIsDown(input::Keyboard::WIN))
                        keyEvent.modifiers |= EVENTFLAG_COMMAND_DOWN;

                    _impl->browser->GetHost()->SendKeyEvent(keyEvent);

                    if (key == input::Keyboard::Key::RETURN || 
                        key == input::Keyboard::Key::TAB ||
                        key == input::Keyboard::Key::BACK_SPACE ||
                        key == input::Keyboard::Key::DEL)
                    {
                        keyEvent.type = KEYEVENT_CHAR;
                        _impl->browser->GetHost()->SendKeyEvent(keyEvent);
                    }
                }
            }
        }
    });

    _keyUpSlot = _canvas->keyboard()->keyUp()->connect([&](input::Keyboard::Ptr keyboard)
    {
        if (!_enableInput)
            return;

        for (uint key = 0; key < input::Keyboard::NUM_KEYS; ++key)
        {
            if (!_canvas->keyboard()->keyIsDown(static_cast<input::Keyboard::Key>(key)))
            {
                if (_keyIsDown.find(key) != _keyIsDown.end() && _keyIsDown[key])
                {
                    _keyIsDown[key] = false;

                    CefKeyEvent keyEvent;

                    keyEvent.windows_key_code = key;
                    keyEvent.type = KEYEVENT_KEYUP;
                    keyEvent.modifiers = 0;

                    if (_canvas->keyboard()->keyIsDown(input::Keyboard::SHIFT) || _canvas->keyboard()->keyIsDown(input::Keyboard::SHIFT_RIGHT))
                        keyEvent.modifiers |= EVENTFLAG_SHIFT_DOWN;

                    if (_canvas->keyboard()->keyIsDown(input::Keyboard::ALT))
                        keyEvent.modifiers |= EVENTFLAG_ALT_DOWN;

                    if (_canvas->keyboard()->keyIsDown(input::Keyboard::CONTROL) || _canvas->keyboard()->keyIsDown(input::Keyboard::CONTROL_RIGHT))
                        keyEvent.modifiers |= EVENTFLAG_CONTROL_DOWN;

                    if (_canvas->keyboard()->keyIsDown(input::Keyboard::WIN))
                        keyEvent.modifiers |= EVENTFLAG_COMMAND_DOWN;

                    _impl->browser->GetHost()->SendKeyEvent(keyEvent);
                }
            }
        }
    });

    _textInputSlot = _canvas->keyboard()->textInput()->connect([&](input::Keyboard::Ptr keyboard, char c)
    {
        if (!_enableInput)
            return;

        CefKeyEvent keyEvent;

        keyEvent.type = KEYEVENT_CHAR;
        keyEvent.windows_key_code = c;
        keyEvent.character = c;

        _impl->browser->GetHost()->SendKeyEvent(keyEvent);
        _impl->browser->GetHost()->SendFocusEvent(true);
    });
}

CefRefPtr<CefRenderProcessHandler>
ChromiumApp::GetRenderProcessHandler()
{
	return _impl->renderProcessHandler.get();
}
#endif