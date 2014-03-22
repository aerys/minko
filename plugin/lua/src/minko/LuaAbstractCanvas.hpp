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

#include "minko/AbstractCanvas.hpp"

#include "minko/LuaWrapper.hpp"

namespace minko
{
	class LuaAbstractCanvas :
		public LuaWrapper
	{
	public:
		static
		void
		bind(LuaGlue& state)
		{
			auto& abstractCanvas = state.Class<AbstractCanvas>("AbstractCanvas")
				.property("width",		    &AbstractCanvas::width)
				.property("height",		    &AbstractCanvas::height)
				.property("mouse", 		    &AbstractCanvas::mouse)
				.property("keyboard",	    &AbstractCanvas::keyboard)
				.property("relativeTime",	&AbstractCanvas::relativeTime)
				.property("frameDuration",	&AbstractCanvas::frameDuration)
                .methodWrapper("joystick",  &LuaAbstractCanvas::joystickWrapper);
		    MINKO_LUAGLUE_BIND_SIGNAL(state, AbstractCanvas::Ptr);
		    MINKO_LUAGLUE_BIND_SIGNAL(state, AbstractCanvas::Ptr, uint, uint);
			MINKO_LUAGLUE_BIND_SIGNAL(state, AbstractCanvas::Ptr, input::Joystick::Ptr);
		    abstractCanvas.property("resized", &AbstractCanvas::resized);
			abstractCanvas.property("joystickAdded", &AbstractCanvas::joystickAdded);
			abstractCanvas.property("joystickRemoved", &AbstractCanvas::joystickRemoved);
			
		}

	private:
		static
		std::shared_ptr<input::Joystick>
		joystickWrapper(AbstractCanvas::Ptr c, uint id)
		{
			return c->joystick(id - 1);
		}
	};
}
