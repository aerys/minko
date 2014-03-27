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
#include "minko/Signal.hpp"
#include "minko/dom/AbstractDOM.hpp"
#include "minko/dom/AbstractDOMElement.hpp"
#include "ChromiumDOMV8Handler.hpp"
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
		class ChromiumDOMEngine;

		class ChromiumDOM : public minko::dom::AbstractDOM,
			public std::enable_shared_from_this<ChromiumDOM>
		{
		public:
			typedef std::shared_ptr<ChromiumDOM> Ptr;


		private:
			ChromiumDOM();

		public:
			~ChromiumDOM();

			static
			Ptr
			create(std::shared_ptr<ChromiumDOMEngine>);

			void
			clear();

			void
			init(CefRefPtr<CefV8Context>, CefRefPtr<CefFrame>);

			void
			sendMessage(std::string);

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
			
			CefRefPtr<CefV8Value>
			window();

			void
			addLoadEventListener();

			void
			addSendMessageFunction();
			
			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr
			onload()
			{
				return _onload;
			}

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr
			onmessage()
			{
				return _onmessage;
			}

			bool
			initialized()
			{
				return _init;
			}

			bool
				isMain();

			std::string
			fileName()
			{
				std::string url = fullUrl();
				int i = url.find_last_of('/');

				std::string fileName = url.substr(i + 1);
				return fileName;
			}

			std::string
			fullUrl();

			bool
			update();

		private:

			std::atomic<bool>	_blocker;

			CefRefPtr<CefV8Context> _v8Context;
			CefRefPtr<CefFrame> _frame;
			CefRefPtr<CefV8Value> _minkoObject;
			CefRefPtr<CefV8Value> _global;
			CefRefPtr<CefV8Value> _document;
			CefRefPtr<ChromiumDOMV8Handler> _v8Handler;

			std::shared_ptr<ChromiumDOMEngine> _engine;

			std::shared_ptr<minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>> _onload;
			std::shared_ptr<minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>> _onmessage;

			minko::Signal<std::string, CefV8ValueList>::Slot _onloadSlot;
			minko::Signal<std::string, CefV8ValueList>::Slot _onmessageSlot;

			bool _cleared;
			bool _init;
			bool _executeOnLoad;

			std::list<std::string> _receivedMessages;
		};
	}
}
#endif