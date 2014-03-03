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

#include "minko/Common.hpp"
#include "include/cef_app.h"
#include "minko/Signal.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/AbstractCanvas.hpp"


namespace minko
{
	class CefPimpl;

	class App : public CefApp,
				public CefBrowserProcessHandler
	{
	public:
		App();

		std::shared_ptr<render::Texture>
		initialize(std::shared_ptr<AbstractCanvas> canvas, std::shared_ptr<render::AbstractContext> context, CefPimpl* impl);

		virtual
		CefRefPtr<CefBrowserProcessHandler> 
		GetBrowserProcessHandler() OVERRIDE
		{ 
			return this; 
		}

		virtual
		void
		OnContextInitialized() OVERRIDE;

		CefRefPtr<CefRenderProcessHandler>
		GetRenderProcessHandler();

	private:
		void
		bindControls();

		Signal<std::shared_ptr<input::Mouse>, int, int>::Slot _mouseMoveSlot;
		Signal<std::shared_ptr<input::Mouse>>::Slot _leftDownSlot;
		Signal<std::shared_ptr<input::Mouse>>::Slot _leftUpSlot;
		Signal<std::shared_ptr<input::Mouse>>::Slot _rightDownSlot;
		Signal<std::shared_ptr<input::Mouse>>::Slot _rightUpSlot;
		Signal<std::shared_ptr<input::Mouse>>::Slot _middleDownSlot;
		Signal<std::shared_ptr<input::Mouse>>::Slot _middleUpSlot;

		std::shared_ptr<AbstractCanvas> _canvas;
		std::shared_ptr<render::AbstractContext> _context;
		CefPimpl* _impl;
		Signal<std::shared_ptr<AbstractCanvas>, uint, uint>::Slot _canvasResizedSlot;

		IMPLEMENT_REFCOUNTING(App);
	};
}