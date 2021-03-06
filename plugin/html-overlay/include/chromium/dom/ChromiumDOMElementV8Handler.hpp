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

#include "minko/Signal.hpp"
#include "minko/dom/AbstractDOMMouseEvent.hpp"
#include "chromium/dom/ChromiumDOMTouchEvent.hpp"
#include "include/cef_render_process_handler.h"

namespace chromium
{
	namespace dom
	{
		class ChromiumDOMElement;

		class ChromiumDOMElementV8Handler : public CefV8Handler
		{
		public:
			ChromiumDOMElementV8Handler(std::shared_ptr<ChromiumDOMElement>);

			virtual
			bool
			Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);

		private:
			std::shared_ptr<ChromiumDOMElement> _element;
			
			IMPLEMENT_REFCOUNTING(ChromiumDOMElementV8Handler);
		};
	}
}
#endif