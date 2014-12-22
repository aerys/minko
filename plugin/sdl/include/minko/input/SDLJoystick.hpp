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

#include "minko/input/Joystick.hpp"

struct _SDL_Joystick;

namespace minko
{
    class Canvas;

    namespace input
    {
        class SDLJoystick :
            public Joystick
        {
            friend Canvas;

        private:
            typedef struct _SDL_Joystick SDL_Joystick;

            SDL_Joystick*    _joystick;

            static const std::map<Button, std::string> ButtonNames;
            static const std::map<Button, Button> NativeToHtmlMap;
            static const std::map<Button, Button> HtmlToNativeMap;

        public:
            static inline
            std::shared_ptr<SDLJoystick>
            create(std::shared_ptr<Canvas> canvas, int joystickId, SDL_Joystick* joystick)
            {
                return std::shared_ptr<SDLJoystick>(new SDLJoystick(canvas, joystickId, joystick));
            }

            inline
            SDL_Joystick* const
            joystick()
            {
                return _joystick;
            }

            bool
            isButtonDown(Button button);

            static
            Button
            button(int platformSpecificButtonId);

            static
            std::string
            buttonName(Button button);

        private:
            SDLJoystick(std::shared_ptr<Canvas> canvas, int joystickId, SDL_Joystick* joystick);

            static
            int
            buttonId(Button button);
        };
    }
}
