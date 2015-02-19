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
    class Canvas;

    namespace input
    {
        class SDLKeyboard :
            public Keyboard
        {
            friend Canvas;

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
            keyIsDown(Keyboard::Key key);

        private:
            enum class KeyType
            {
                KeyCode,
                ScanCode
            };

            // TODO: change map to unordered_map (doesn't compile with Emscripten 1.13 with unordered_map)
            static const std::unordered_map<Key, KeyType> keyTypeMap;

            //static const std::unordered_map<Key, KeyCode> keyToKeyCodeMap;
            //static const std::unordered_map<Key, ScanCode> keyToScanCodeMap;

            bool
            hasKeyDownSignal(Keyboard::Key key)
            {
                return _keyDown.count(static_cast<int>(key)) != 0;
            }

            bool
            hasKeyUpSignal(Keyboard::Key key)
            {
                return _keyUp.count(static_cast<int>(key)) != 0;
            }

            SDLKeyboard();

            KeyCode
            getKeyCodeFromScanCode(ScanCode scanCode);

            ScanCode
            getScanCodeFromKeyCode(KeyCode keyCode);
        };
    }
}