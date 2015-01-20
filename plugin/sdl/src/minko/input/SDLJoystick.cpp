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

#include "minko/input/SDLJoystick.hpp"

#include "minko/Canvas.hpp"

#include "SDL.h"

using namespace minko;
using namespace minko::input;

const std::map<SDLJoystick::Button, SDLJoystick::Button> SDLJoystick::NativeToHtmlMap =
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

const std::map<SDLJoystick::Button, SDLJoystick::Button> SDLJoystick::HtmlToNativeMap =
{
    { Button::X, Button::DPadUp },
    { Button::Y, Button::DPadDown },
    { Button::Home, Button::DPadLeft },
    { Button::LT, Button::DPadRight },
    { Button::RB, Button::Start },
    { Button::LB, Button::Select },
    { Button::A, Button::L3 },
    { Button::B, Button::R3 },
    { Button::Start, Button::LB },
    { Button::Select, Button::RB },
    { Button::DPadUp, Button::A },
    { Button::DPadDown, Button::B },
    { Button::DPadLeft, Button::X },
    { Button::DPadRight, Button::Y },
    { Button::Nothing, Button::Home },
    { Button::L3, Button::LT },
    { Button::R3, Button::RT },
};


const std::map<SDLJoystick::Button, std::string> SDLJoystick::ButtonNames =
{
    { Button::DPadUp, "DPadUp" },
    { Button::DPadDown, "DPadDown" },
    { Button::DPadLeft, "DPadLeft" },
    { Button::DPadRight, "DPadRight" },
    { Button::Start, "Start" },
    { Button::Select, "Select" },
    { Button::L3, "L3" },
    { Button::R3, "R3" },
    { Button::LB, "LB" },
    { Button::RB, "RB" },
    { Button::A, "A" },
    { Button::B, "B" },
    { Button::X, "X" },
    { Button::Y, "Y" },
    { Button::Home, "Home" },
    { Button::LT, "LT" },
    { Button::RT, "RT" },
};

SDLJoystick::SDLJoystick(std::shared_ptr<Canvas> canvas, int joystickId, SDL_Joystick* joystick) :
    input::Joystick(canvas, joystickId),
    _joystick(joystick)
{
}

SDLJoystick::Button
SDLJoystick::button(int platformSpecificButtonId)
{
    Button button = static_cast<SDLJoystick::Button>(platformSpecificButtonId);

#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
    // Conversion from HTML5 buttons.
    auto buttonIterator = HtmlToNativeMap.find(button);
    button = buttonIterator == HtmlToNativeMap.end() ? Button::Nothing : buttonIterator->second;
#endif

    return button;
}

int
SDLJoystick::buttonId(Button button)
{
    return static_cast<int>(button);
}

bool
SDLJoystick::isButtonDown(Button button)
{
#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
    // Conversion to HTML5 buttons.
    auto buttonIterator = NativeToHtmlMap.find(button);
    button = buttonIterator == NativeToHtmlMap.end() ? Button::Nothing : buttonIterator->second;
#endif

    return SDL_JoystickGetButton(_joystick, buttonId(button)) != 0;
}

std::string
SDLJoystick::buttonName(Button button)
{
    auto buttonName = ButtonNames.find(button);

    // This button is not mapped
    if (buttonName == ButtonNames.end())
        return "None";

    return buttonName->second;
}
