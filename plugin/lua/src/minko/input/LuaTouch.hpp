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

#include "minko/input/Touch.hpp"

#include "minko/LuaWrapper.hpp"

namespace minko
{
    namespace input
    {
        class LuaTouch :
            public LuaWrapper
        {
        public:
            static
            void
            bind(LuaGlue& state)
            {
                auto& inputTouch = state.Class<input::Touch>("Touch");


                MINKO_LUAGLUE_BIND_SIGNAL(state, input::Touch::Ptr);
                MINKO_LUAGLUE_BIND_SIGNAL(state, input::Touch::Ptr, float, float);

                inputTouch
                    .property("touches",            &input::Touch::touchMove)
                    .property("numTouches",         &input::Touch::numTouches)
                    .property("touchDown",          &input::Touch::touchDown)
                    .property("touchUp",            &input::Touch::touchUp)
                    .property("touchMove",          &input::Touch::touchMove)
                    .property("swipeLeft",          &input::Touch::swipeLeft)
                    .property("swipeRight",         &input::Touch::swipeRight)
                    .property("swipeUp",            &input::Touch::swipeUp)
                    .property("swipeDown",          &input::Touch::swipeDown)
                    .property("pinchZoom",          &input::Touch::pinchZoom)
                    .method("touch",                &input::Touch::touch)
                ;
            }
        };
    }
}
