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
#include "minko/Signal.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/AbstractCanvas.hpp"
#include "minko/dom/AbstractDOMEngine.hpp"
#include "minko/dom/AbstractDOMElement.hpp"
#include "minko/dom/AbstractDOMEvent.hpp"
#include "ChromiumDOMEngineV8Handler.hpp"
#include "include/cef_render_process_handler.h"

#include <stdio.h>  /* defines FILENAME_MAX */
#if defined(_WIN32)
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

namespace chromium
{
	class ChromiumPimpl;
	namespace dom
	{
		class ChromiumDOMEngine : public minko::dom::AbstractDOMEngine,
			public std::enable_shared_from_this<ChromiumDOMEngine>
		{
		public:
			typedef std::shared_ptr<ChromiumDOMEngine> Ptr;

			ChromiumDOMEngine();
			
			void
			initNewPage(CefRefPtr<CefV8Context>);

			void
			initialize(std::shared_ptr<minko::AbstractCanvas>, std::shared_ptr<minko::component::SceneManager>);
			
			void
			load(std::string uri);

			void
			unload();

			minko::dom::AbstractDOMElement::Ptr
			createElement(std::string);

			minko::dom::AbstractDOMElement::Ptr
			getElementById(std::string);

			std::list<minko::dom::AbstractDOMElement::Ptr>
			getElementsByClassName(std::string);

			std::list<minko::dom::AbstractDOMElement::Ptr>
			getElementsByTagName(std::string);

			minko::dom::AbstractDOMElement::Ptr
			document();

			minko::dom::AbstractDOMElement::Ptr
			body();

			minko::Signal<std::string>::Ptr
			onload()
			{
				return _onload;
			}

			minko::Signal<std::string>::Ptr
			onmessage()
			{
				return _onmessage;
			}

		private:
			void
			start(int argc, char** argv);

			void
			start();

			void
			enterFrame();

			void
			loadHttp(std::string);

			void
			loadLocal(std::string);


			std::string
			getWorkingDirectory()
			{
				if (!GetCurrentDir(_currentPath, sizeof(_currentPath)))
				{
					return "";
				}
				_currentPath[sizeof(_currentPath)-1] = '\0'; /* not really required */

				return std::string(_currentPath);
			}

			CefRefPtr<CefV8Value>
			ChromiumDOMEngine::window();

			void
			addLoadEventListener();

			void
			addSendMessageFunction();

		private:
			CefRefPtr<CefV8Value> _minkoObject;

			std::shared_ptr<minko::Signal<std::string>> _onload;
			std::shared_ptr<minko::Signal<std::string>> _onmessage;

			minko::Signal<std::string, CefV8ValueList>::Slot _onloadSlot;
			minko::Signal<std::string, CefV8ValueList>::Slot _onmessageSlot;

			CefRefPtr<ChromiumDOMEngineV8Handler> _v8Handler;
			CefRefPtr<CefV8Context> _currentV8Context;
			
			minko::AbstractCanvas::Ptr _canvas;
			minko::component::SceneManager::Ptr _sceneManager;

			minko::Signal<minko::AbstractCanvas::Ptr, minko::uint, minko::uint>::Slot _canvasResizedSlot;
			minko::Signal<minko::component::SceneManager::Ptr>::Slot _enterFrameSlot;

			ChromiumPimpl* _impl;
			std::shared_ptr<minko::material::BasicMaterial> _overlayMaterial;

			char _currentPath[FILENAME_MAX];
		};
	}
}