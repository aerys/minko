/*
Copyright (c) 2014 Aerys

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
#include "apple/dom/AppleWebViewDOMMouseEvent.hpp"
#include "minko/dom/AbstractDOMTouchEvent.hpp"

namespace apple
{
    namespace dom
    {
        class AppleWebViewDOMEngine;

        class AppleWebViewDOMTouchEvent :
            public virtual minko::dom::AbstractDOMTouchEvent,
            public apple::dom::AppleWebViewDOMMouseEvent

        {
        public:
            typedef std::shared_ptr<AppleWebViewDOMTouchEvent> Ptr;

        protected:
            AppleWebViewDOMTouchEvent(const std::string& jsAccessor, int changedTouchesIndex):
                AppleWebViewDOMMouseEvent(jsAccessor),
                _changedTouchesIndex(changedTouchesIndex)
            {
            }

        public:
            static
            Ptr
            create(const std::string& jsAccessor, int changedTouchesIndex, std::shared_ptr<AppleWebViewDOMEngine> engine)
            {
                Ptr event(new AppleWebViewDOMTouchEvent(jsAccessor, changedTouchesIndex));
                event->_engine = engine;

                return event;
            }

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
            identifier();
        
        private:
            int _changedTouchesIndex;
        };
    }
}
