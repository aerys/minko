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

#if defined(EMSCRIPTEN)
#pragma once

#include "minko/Common.hpp"
#include "minko/dom/AbstractDOMElement.hpp"
#include "minko/dom/AbstractDOMEvent.hpp"

namespace emscripten
{
    namespace dom
    {
        class EmscriptenDOMEvent : public virtual minko::dom::AbstractDOMEvent
        {
        public:
            typedef std::shared_ptr<EmscriptenDOMEvent> Ptr;

        protected:
            EmscriptenDOMEvent(std::string jsAccessor):
                _jsAccessor(jsAccessor)
            {
            }

        public:
            static
            Ptr
            create(std::string jsAccessor)
            {
                Ptr event(new EmscriptenDOMEvent(jsAccessor));
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

        protected:
            
            int
            getProperty(std::string path);

            std::string _jsAccessor;
        };
    }
}
#endif
