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

namespace ioswebview
{
	namespace dom
	{
        class IOSWebViewDOMEngine;
        
		class IOSWebViewDOMTouchEvent : public minko::dom::AbstractDOMTouchEvent
		{
		public:
			typedef std::shared_ptr<IOSWebViewDOMTouchEvent> Ptr;

		private:
			IOSWebViewDOMTouchEvent(std::string jsAccessor, int fingerId, int index):
				_jsAccessor(jsAccessor),
                _fingerId(fingerId),
                _index(index)
			{
			}

		public:

			static
			Ptr
			create(std::string jsAccessor, int fingerId, int index, std::shared_ptr<IOSWebViewDOMEngine> engine)
			{
				Ptr event(new IOSWebViewDOMTouchEvent(jsAccessor, fingerId, index));
                event->_engine = engine;
                
				return event;
			}

			void
			preventDefault();

			void
			stopPropagation();
            
			std::string
			type();

			minko::dom::AbstractDOMElement::Ptr
			target();
            
			int
			clientX();

			int
			clientY();

			int
			pageX();

			int
			pageY();


			int
			layerX();

			int
			layerY();


			int
			screenX();

			int
			screenY();
            
            int
            fingerId();
            
            std::string&
            jsAccessor();

		private:
			std::string _jsAccessor;
            std::shared_ptr<IOSWebViewDOMEngine> _engine;
            // The JS touch event property called "identifier" similar to SDL fingerId (a unique id)
            int _fingerId;
            // The index in the current changedTouches array
            int _index;
		};
	}
}