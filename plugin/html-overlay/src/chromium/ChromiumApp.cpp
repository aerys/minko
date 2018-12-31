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

using namespace minko;
using namespace chromium;

ChromiumApp::~ChromiumApp()
{
}

std::shared_ptr<render::Texture>
ChromiumApp::initialize(std::shared_ptr<AbstractCanvas> canvas, std::shared_ptr<render::AbstractContext> context, ChromiumPimpl* impl)
{
    _secure = true;
	_canvas = canvas;
	_context = context;
	_impl = impl;
	_impl->renderHandler = new ChromiumRenderHandler(_canvas, _context);
    _impl->renderProcessHandler = new ChromiumRenderProcessHandler(_impl);

	return _impl->renderHandler->renderTexture;
}

void
ChromiumApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
	command_line->AppendSwitch("off-screen-rendering-enabled");
	command_line->AppendSwitch("single-process");
	command_line->AppendSwitch("no-sandbox");
#ifdef DEBUG
	command_line->AppendSwitchWithValue("remote-debugging-port", "8080");
#endif
	if (!_secure) {
		command_line->AppendSwitch("disable-web-security");
		command_line->AppendSwitch("ignore-certificate-errors");
	}
}

void
ChromiumApp::OnContextInitialized()
{
	CefWindowInfo window_info;
	CefBrowserSettings browserSettings;

	browserSettings.file_access_from_file_urls = STATE_ENABLED;
	browserSettings.universal_access_from_file_urls = STATE_ENABLED;
	browserSettings.webgl = STATE_DISABLED;

	CefRefPtr<ChromiumClient> browserClient = new ChromiumClient(_impl);

	// in linux set a gtk widget, in windows a hwnd. If not available set to null (not nullptr) - may cause some render errors, in context-menu and plugins.
	window_info.SetAsWindowless(0);
	// if [parent] is not provided then the main screen monitor will be used
  	// and some functionality that requires a parent window may not function correctly.
	// transparency is set by default in the latest CEF3 versions.

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

        if (_impl->renderHandler->currentCursor())
        {
#if defined(_MSC_VER) 
            SetCursor(_impl->renderHandler->currentCursor());
#endif
        }
	}, std::numeric_limits<float>::max());

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
	}, std::numeric_limits<float>::max());

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
	}, std::numeric_limits<float>::max());

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
	}, std::numeric_limits<float>::max());

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
	}, std::numeric_limits<float>::max());

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
	}, std::numeric_limits<float>::max());

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
    }, std::numeric_limits<float>::max());

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
					keyEvent.native_key_code = key;
					keyEvent.unmodified_character = key;
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
    }, std::numeric_limits<float>::max());

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
					keyEvent.native_key_code = key;
					keyEvent.unmodified_character = key;
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
    }, std::numeric_limits<float>::max());

    _textInputSlot = _canvas->keyboard()->textInput()->connect([&](input::Keyboard::Ptr keyboard, char16_t c)
    {
        if (!_enableInput)
            return;

        CefKeyEvent keyEvent;

        keyEvent.type = KEYEVENT_CHAR;
        keyEvent.windows_key_code = c;
		keyEvent.native_key_code = c;
		keyEvent.unmodified_character = c;
        keyEvent.character = c;

        _impl->browser->GetHost()->SendKeyEvent(keyEvent);
        _impl->browser->GetHost()->SendFocusEvent(true);
    }, std::numeric_limits<float>::max());
}

CefRefPtr<CefRenderProcessHandler>
ChromiumApp::GetRenderProcessHandler()
{
	return _impl->renderProcessHandler.get();
}
#endif
