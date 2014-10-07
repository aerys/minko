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

#if MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS
#pragma warning(push)
#pragma warning(disable:4250)
#endif

#include "minko/Common.hpp"
#include "minko/dom/AbstractDOMMouseEvent.hpp"
#include "minko/dom/AbstractDOMElement.hpp"
#include "chromium/dom/ChromiumDOMEvent.hpp"
#include "chromium/dom/ChromiumDOMObject.hpp"
#include "include/cef_render_process_handler.h"

namespace chromium
{
	namespace dom
	{
        class ChromiumDOMMouseEvent :
            public ChromiumDOMEvent,
            public virtual minko::dom::AbstractDOMMouseEvent
		{
		public:
			typedef std::shared_ptr<ChromiumDOMMouseEvent> Ptr;

			~ChromiumDOMMouseEvent();

        private:
			ChromiumDOMMouseEvent(CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>);
			
		public:
			static
			Ptr
			create(CefRefPtr<CefV8Value>, CefRefPtr<CefV8Context>);

            inline
			int
            clientX()
            {
                return getProperty<int>("clientX");
            }

            inline
			int
			clientY()
            {
                return getProperty<int>("clientY");
            }

            inline
			int
			pageX()
            {
                return getProperty<int>("pageX");
            }

            inline
            int
            pageY()
            {
                return getProperty<int>("pageY");
            }

            inline
			int
            layerX()
            {
                return getProperty<int>("layerX");
            }

            inline
			int
			layerY()
            {
                return getProperty<int>("layerY");
            }

            inline
			int
			screenX()
            {
                return getProperty<int>("screenX");
            }

            inline
			int
            screenY()
            {
                return getProperty<int>("screenY");
            }
		};
	}
}

#if MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS
#pragma warning(pop)
#endif

#endif