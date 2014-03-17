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

#include "minko/input/Keyboard.hpp"

namespace minko
{
    class SDLKeyboard :
        public input::Keyboard
    {
        friend class Canvas;

    private:
        const unsigned char* _keyboardState;

    public:
        static inline
        std::shared_ptr<SDLKeyboard>
        create()
        {
            return std::shared_ptr<SDLKeyboard>(new SDLKeyboard());
        }

    public:
        bool
            keyIsDown(input::Keyboard::ScanCode scanCode);

        bool
            keyIsDown(input::Keyboard::KeyCode keyCode);
    private:
        bool
        hasKeyDownSignal(input::Keyboard::ScanCode scanCode)
        {
            return _keyDown.count(static_cast<int>(scanCode)) != 0;
        }

        bool
        hasKeyUpSignal(input::Keyboard::ScanCode scanCode)
        {
            return _keyUp.count(static_cast<int>(scanCode)) != 0;
        }

        SDLKeyboard();

        KeyCode
            getKeyCodeFromScanCode(ScanCode scanCode);

        ScanCode
            getScanCodeFromKeyCode(KeyCode keyCode);
    };
}