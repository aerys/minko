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

#include "MinkoSDL.hpp"

bool MinkoSDL::_active = false;

minko::Signal<>::Ptr MinkoSDL::_enterFrame = nullptr;

minko::Signal<const Uint8*>::Ptr MinkoSDL::_keyDown = nullptr;

minko::Signal<MinkoSDL::uint, MinkoSDL::uint>::Ptr MinkoSDL::_mouseMove = nullptr;
minko::Signal<MinkoSDL::uint, MinkoSDL::uint>::Ptr MinkoSDL::_mouseLeftButtonDown = nullptr;
minko::Signal<MinkoSDL::uint, MinkoSDL::uint>::Ptr MinkoSDL::_mouseLeftButtonUp = nullptr;
minko::Signal<int, int>::Ptr MinkoSDL::_mouseWheel = nullptr;
minko::Signal<int, int, int>::Ptr MinkoSDL::_joystickMotion = nullptr;
minko::Signal<int>::Ptr MinkoSDL::_joystickButtonDown = nullptr;
minko::Signal<int>::Ptr MinkoSDL::_joystickButtonUp = nullptr;

minko::Signal<MinkoSDL::uint, MinkoSDL::uint>::Ptr MinkoSDL::_resized = nullptr;

minko::render::AbstractContext::Ptr MinkoSDL::_context = nullptr;
float MinkoSDL::_framerate = 0.f;

#ifndef EMSCRIPTEN
SDL_Window* MinkoSDL::_window = 0;
#endif

#ifdef MINKO_ANGLE
MinkoSDL::ESContext* MinkoSDL::_angleContext = 0;
#endif
