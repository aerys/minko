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
#include "minko/Signal.hpp"
#include "minko/AbstractCanvas.hpp"

namespace minko
{
    namespace input
    {
        class Joystick
        {
        public:
            typedef std::shared_ptr<Joystick> Ptr;

            enum class Button
            {
                Nothing = -1,
                DPadUp = 0,
                DPadDown = 1,
                DPadLeft = 2,
                DPadRight = 3,
                Start = 4,
                Select = 5,
                L3 = 6,
                R3 = 7,
                LB = 8,
                RB = 9,
                A = 10,
                B = 11,
                X = 12,
                Y = 13,
                Home = 14,
                LT = 15, // Not a button (axis)
                RT = 16, // Not a button (axis)
            };

        private:
            std::shared_ptr<AbstractCanvas> _canvas;

            Signal<Ptr, int, int, int>::Ptr       _joystickAxisMotion;
            Signal<Ptr, int, int, int>::Ptr       _joystickHatMotion;
            Signal<Ptr, int, int>::Ptr            _joystickButtonDown;
            Signal<Ptr, int, int>::Ptr            _joystickButtonUp;

            int                                   _joystickId;

        public:
            inline
            int
            joystickId()
            {
                return _joystickId;
            }

            inline
            Signal<Ptr, int, int, int>::Ptr
            joystickAxisMotion() const
            {
                return _joystickAxisMotion;
            }

            inline
            Signal<Ptr, int, int, int>::Ptr
            joystickHatMotion() const
            {
                return _joystickHatMotion;
            }


            inline
            Signal<Ptr, int, int>::Ptr
            joystickButtonDown() const
            {
                return _joystickButtonDown;
            }

            inline
            Signal<Ptr, int, int>::Ptr
            joystickButtonUp() const
            {
                return _joystickButtonUp;
            }

        protected:
            Joystick(std::shared_ptr<AbstractCanvas> canvas, int joystickId);
        };
    }
}
