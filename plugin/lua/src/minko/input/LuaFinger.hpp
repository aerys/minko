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

#include "minko/input/Finger.hpp"

#include "minko/LuaWrapper.hpp"

namespace minko
{
	namespace input
	{
		class LuaFinger :
			public LuaWrapper
		{
		public:
			static
			void
			bind(LuaGlue& state)
			{
			    auto& inputFinger = state.Class<input::Finger>("Finger");
			        

			    MINKO_LUAGLUE_BIND_SIGNAL(state, input::Finger::Ptr);
			    MINKO_LUAGLUE_BIND_SIGNAL(state, input::Finger::Ptr, float, float);

			    inputFinger
                    .property("x",                  &Finger::x)
                    .property("y",                  &input::Finger::y)
                    .property("dx",                 &input::Finger::dx)
                    .property("dy",                 &input::Finger::dy)
			        .property("fingerDown",         &Finger::fingerDown)
                    .property("fingerUp",           &input::Finger::fingerUp)
			        .property("fingerMotion",       &input::Finger::fingerMotion)
			        .property("swipeLeft",          &input::Finger::swipeLeft)
                    .property("swipeRight",         &input::Finger::swipeRight)
                    .property("swipeUp",            &input::Finger::swipeUp)
                    .property("swipeDown",          &input::Finger::swipeDown)
                ;
			}
		};
	}
}
