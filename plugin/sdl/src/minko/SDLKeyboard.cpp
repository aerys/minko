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

#include "minko/SDLKeyboard.hpp"

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

#if defined(__APPLE__)
# include <TargetConditionals.h>
# if TARGET_OS_IPHONE
#  include "SDL2/SDL_opengles.h"
# endif
#endif

using namespace minko;

const std::map<input::Keyboard::ScanCode, input::Keyboard::KeyCode> ScanCodeKeyCodeMap =
{
    { input::Keyboard::ScanCode::UNKNOWN, input::Keyboard::KeyCode::UNKNOWN },    { input::Keyboard::ScanCode::A, input::Keyboard::KeyCode::q },    { input::Keyboard::ScanCode::B, input::Keyboard::KeyCode::b },    { input::Keyboard::ScanCode::C, input::Keyboard::KeyCode::c },    { input::Keyboard::ScanCode::D, input::Keyboard::KeyCode::d },    { input::Keyboard::ScanCode::E, input::Keyboard::KeyCode::e },    { input::Keyboard::ScanCode::F, input::Keyboard::KeyCode::f },    { input::Keyboard::ScanCode::G, input::Keyboard::KeyCode::g },    { input::Keyboard::ScanCode::H, input::Keyboard::KeyCode::h },    { input::Keyboard::ScanCode::I, input::Keyboard::KeyCode::i },    { input::Keyboard::ScanCode::J, input::Keyboard::KeyCode::j },    { input::Keyboard::ScanCode::K, input::Keyboard::KeyCode::k },    { input::Keyboard::ScanCode::L, input::Keyboard::KeyCode::l },    { input::Keyboard::ScanCode::M, input::Keyboard::KeyCode::COMMA },    { input::Keyboard::ScanCode::N, input::Keyboard::KeyCode::n },    { input::Keyboard::ScanCode::O, input::Keyboard::KeyCode::o },    { input::Keyboard::ScanCode::P, input::Keyboard::KeyCode::p },    { input::Keyboard::ScanCode::Q, input::Keyboard::KeyCode::a },    { input::Keyboard::ScanCode::R, input::Keyboard::KeyCode::r },    { input::Keyboard::ScanCode::S, input::Keyboard::KeyCode::s },    { input::Keyboard::ScanCode::T, input::Keyboard::KeyCode::t },    { input::Keyboard::ScanCode::U, input::Keyboard::KeyCode::u },    { input::Keyboard::ScanCode::V, input::Keyboard::KeyCode::v },    { input::Keyboard::ScanCode::W, input::Keyboard::KeyCode::z },    { input::Keyboard::ScanCode::X, input::Keyboard::KeyCode::x },    { input::Keyboard::ScanCode::Y, input::Keyboard::KeyCode::y },    { input::Keyboard::ScanCode::Z, input::Keyboard::KeyCode::w }
};

SDLKeyboard::SDLKeyboard()
{
    _keyboardState = SDL_GetKeyboardState(NULL);

    /*
    int keyCode1 = static_cast<int>(SDL_GetScancodeFromKey(static_cast<int>(input::Keyboard::KeyCode::a)));
    int keyCode2 = static_cast<int>(KeyCodeScanCodeMap.find(input::Keyboard::KeyCode::a)->second);

    std::cout << "A: \n" << "Key code: " << keyCode1
        << "\nKey code: " << keyCode2
        << std::endl;
    */

    //std::cout << static_cast<int>(SDL_SCANCODE_TO_KEYCODE(static_cast<int>(input::Keyboard::ScanCode::B))) << std::endl;
}

bool SDLKeyboard::keyIsDown(input::Keyboard::ScanCode scanCode)
{
#if defined(EMSCRIPTEN)
    return _keyboardState[static_cast<int>(getKeyCodeFromScanCode(scanCode))] != 0;
#else
    return _keyboardState[static_cast<int>(scanCode)] != 0;
#endif
}

bool SDLKeyboard::keyIsDown(input::Keyboard::KeyCode keyCode)
{
#if defined(EMSCRIPTEN)
    // Note: bug in emscripten, GetKeyStates is indexed by key codes.
    return _keyboardState[static_cast<int>(keyCode)] != 0;
#else
    return _keyboardState[static_cast<int>(getScanCodeFromKeyCode(keyCode))] != 0;
#endif
}

input::Keyboard::KeyCode SDLKeyboard::getKeyCodeFromScanCode(input::Keyboard::ScanCode scanCode)
{
    auto iterator = ScanCodeKeyCodeMap.find(scanCode);

    if (iterator != ScanCodeKeyCodeMap.end())
        return iterator->second;
    else
        return static_cast<input::Keyboard::KeyCode>(SDL_SCANCODE_TO_KEYCODE(static_cast<int>(scanCode)));

}

input::Keyboard::ScanCode SDLKeyboard::getScanCodeFromKeyCode(input::Keyboard::KeyCode keyCode)
{
    return static_cast<input::Keyboard::ScanCode>(SDL_GetScancodeFromKey(static_cast<int>(keyCode)));
}
