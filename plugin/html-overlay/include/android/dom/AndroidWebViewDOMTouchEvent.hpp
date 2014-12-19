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
#include "minko/dom/AbstractDOMElement.hpp"
#include "minko/dom/AbstractDOMTouchEvent.hpp"
#include "android/dom/AndroidWebViewDOMMouseEvent.hpp"

namespace android
{
	namespace dom
	{
        class AndroidWebViewDOMEngine;

		class AndroidWebViewDOMTouchEvent : 
			public virtual minko::dom::AbstractDOMTouchEvent,
			public AndroidWebViewDOMMouseEvent
		{
		public:
			typedef std::shared_ptr<AndroidWebViewDOMTouchEvent> Ptr;

		private:
			AndroidWebViewDOMTouchEvent(const std::string& type, minko::dom::AbstractDOMElement::Ptr target) :
				AndroidWebViewDOMMouseEvent(type, target),
				_identifier(-1)
			{
			}

		public:

			static
			Ptr
			create(const std::string& type, minko::dom::AbstractDOMElement::Ptr target)
			{
				Ptr event(new AndroidWebViewDOMTouchEvent(type, target));

				return event;
			}

            int
            identifier();

            void
            identifier(int value);

		private:
            int _identifier;
		};
	}
}