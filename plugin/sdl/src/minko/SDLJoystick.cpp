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

#include "minko/Canvas.hpp"
#include "minko/SDLJoystick.hpp"

#if defined(EMSCRIPTEN)
# include "minko/MinkoWebGL.hpp"
# include "SDL/SDL.h"
# include "emscripten/emscripten.h"
#elif defined(MINKO_ANGLE)
# include "SDL2/SDL.h"
# include "SDL2/SDL_syswm.h"
# include <EGL/egl.h>
# include <GLES2/gl2.h>
# include <GLES2/gl2ext.h>
#else
# include "SDL2/SDL.h"
#endif

using namespace minko;

const std::map<SDLJoystick::Button, SDLJoystick::Button> SDLJoystick::nativeToHtmlMap =
{
    { Button::DPadUp, Button::X },
    { Button::DPadDown, Button::Y },
    { Button::DPadLeft, Button::Home },
    { Button::DPadRight, Button::LT },
    { Button::Start, Button::RB },
    { Button::Select, Button::LB },
    { Button::L3, Button::A },
    { Button::R3, Button::B },
    { Button::LB, Button::Start },
    { Button::RB, Button::Select },
    { Button::A, Button::DPadUp },
    { Button::B, Button::DPadDown },
    { Button::X, Button::DPadLeft },
    { Button::Y, Button::DPadRight },
    { Button::Home, Button::Nothing },
    { Button::LT, Button::L3 },
    { Button::RT, Button::R3 },
};

SDLJoystick::SDLJoystick(std::shared_ptr<Canvas> canvas, int joystickId, SDL_Joystick* joystick) :
    input::Joystick(canvas, joystickId),
    _joystick(joystick)
{
}

bool SDLJoystick::isButtonDown(Button button)
{
#if defined EMSCRIPTEN
    auto realButton = nativeToHtmlMap.find(button);

    // This button is not mapped
    if (realButton == nativeToHtmlMap.end())
        return false;

    return SDL_JoystickGetButton(_joystick, static_cast<int>(realButton->second));
#else
    return SDL_JoystickGetButton(_joystick, static_cast<int>(button));
#endif
}