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
				auto& input_keyboard = state.Class<Keyboard>("Keyboard");
				MINKO_LUAGLUE_BIND_SIGNAL(state, Keyboard::Ptr, uint);
				MINKO_LUAGLUE_BIND_SIGNAL(state, Keyboard::Ptr);
				input_keyboard
					.methodWrapper("keyDown",   &LuaKeyboard::keyboardKeyDownWrapper)
					.methodWrapper("keyUp",     &LuaKeyboard::keyboardKeyUpWrapper)
					.methodWrapper("keyIsDown", &LuaKeyboard::keyboardKeyIsDownWrapper)
					.property("anyKeyDown",	    &Keyboard::keyDown)
					.property("anyKeyUp",       &Keyboard::keyUp);

				for (int key = 0; key < (int) Keyboard::NUM_SCANCODES; ++key)
				{
					auto& keyName = Keyboard::getKeyName(static_cast<Keyboard::ScanCode>(key));

					if (keyName.size())
						input_keyboard.constant(keyName, key);
				}
			}

			static
			Signal<Keyboard::Ptr, uint>::Ptr
			keyboardKeyDownWrapper(Keyboard::Ptr k, uint s)
			{
				return k->keyDown(static_cast<Keyboard::ScanCode>(s));
			}

			static
			Signal<Keyboard::Ptr, uint>::Ptr
			keyboardKeyUpWrapper(Keyboard::Ptr k, uint s)
			{
				return k->keyUp(static_cast<Keyboard::ScanCode>(s));
			}

			static
			bool
			keyboardKeyIsDownWrapper(Keyboard::Ptr k, uint s)
			{
				return k->keyIsDown(static_cast<Keyboard::ScanCode>(s));
			}
		};
	}
}
