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

#include "minko/input/Keyboard.hpp"

#include "minko/LuaWrapper.hpp"

namespace minko
{
	namespace input
	{
		class LuaKeyboard :
			public LuaWrapper
		{
		public:
			static
			void
			bind(LuaGlue& state)
			{
				 auto& input_keyboard = state.Class<input::Keyboard>("Keyboard");
			    for (int key = 0; key < (int)input::Keyboard::NUM_KEYS; ++key)
			    {
			        auto& keyName = input::Keyboard::getKeyName(static_cast<input::Keyboard::ScanCode>(key));

			        if (keyName.size())
			            input_keyboard.constant(keyName, key);
			    }
			    MINKO_LUAGLUE_BIND_SIGNAL(state, input::Keyboard::Ptr, uint);
			    MINKO_LUAGLUE_BIND_SIGNAL(state, input::Keyboard::Ptr);
			    input_keyboard
			        .method("keyDown",          &LuaKeyboard::keyboardKeyDownWrapper)
			        .method("keyUp",            &LuaKeyboard::keyboardKeyUpWrapper)
			        .method("keyIsDown",        &input::Keyboard::keyIsDown)
			        .property("anyKeyDown",     &input::Keyboard::keyDown)
			        .property("anyKeyUp",       &input::Keyboard::keyUp);
			}

			static
			Signal<input::Keyboard::Ptr, uint>::Ptr
			keyboardKeyDownWrapper(input::Keyboard::Ptr k, uint s)
			{
				return k->keyDown(static_cast<input::Keyboard::ScanCode>(s));
			}

			static
			Signal<input::Keyboard::Ptr, uint>::Ptr
			keyboardKeyUpWrapper(input::Keyboard::Ptr k, uint s)
			{
				return k->keyUp(static_cast<input::Keyboard::ScanCode>(s));
			}
		};
	}
}
