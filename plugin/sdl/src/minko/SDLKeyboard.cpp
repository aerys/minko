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

const std::map<SDLKeyboard::Key, SDLKeyboard::KeyType> SDLKeyboard::keyTypeMap =
{
    //// Key codes
    { Key::BACK_SPACE, KeyType::KeyCode },
    { Key::TAB, KeyType::KeyCode },
    { Key::CLEAR, KeyType::KeyCode },
    { Key::RETURN, KeyType::KeyCode },
    { Key::PAUSE, KeyType::KeyCode },
    { Key::ESCAPE, KeyType::KeyCode },
    { Key::SPACE, KeyType::KeyCode },
    { Key::EXCLAMATION, KeyType::KeyCode },
    { Key::DOUBLE_QUOTE, KeyType::KeyCode },
    { Key::HASH, KeyType::KeyCode },
    { Key::DOLLAR, KeyType::KeyCode },
    { Key::PERCENT, KeyType::KeyCode },
    { Key::AMPERSAND, KeyType::KeyCode },
    { Key::QUOTE, KeyType::KeyCode },
    { Key::OPEN_PAREN, KeyType::KeyCode },
    { Key::CLOSE_PAREN, KeyType::KeyCode },
    { Key::ASTERISK, KeyType::KeyCode },
    { Key::PLUS, KeyType::KeyCode },
    { Key::COMMA, KeyType::KeyCode },
    { Key::HYPHEN_MINUS, KeyType::KeyCode },
    { Key::PERIOD, KeyType::KeyCode },
    { Key::SLASH, KeyType::KeyCode },
    { Key::_0, KeyType::KeyCode },
    { Key::_1, KeyType::KeyCode },
    { Key::_2, KeyType::KeyCode },
    { Key::_3, KeyType::KeyCode },
    { Key::_4, KeyType::KeyCode },
    { Key::_5, KeyType::KeyCode },
    { Key::_6, KeyType::KeyCode },
    { Key::_7, KeyType::KeyCode },
    { Key::_8, KeyType::KeyCode },
    { Key::_9, KeyType::KeyCode },
    { Key::COLON, KeyType::KeyCode },
    { Key::SEMICOLON, KeyType::KeyCode },

    { Key::A, KeyType::KeyCode },
    { Key::B, KeyType::KeyCode },
    { Key::C, KeyType::KeyCode },
    { Key::D, KeyType::KeyCode },
    { Key::E, KeyType::KeyCode },
    { Key::F, KeyType::KeyCode },
    { Key::G, KeyType::KeyCode },
    { Key::H, KeyType::KeyCode },
    { Key::I, KeyType::KeyCode },
    { Key::J, KeyType::KeyCode },
    { Key::K, KeyType::KeyCode },
    { Key::L, KeyType::KeyCode },
    { Key::M, KeyType::KeyCode },
    { Key::N, KeyType::KeyCode },
    { Key::O, KeyType::KeyCode },
    { Key::P, KeyType::KeyCode },
    { Key::Q, KeyType::KeyCode },
    { Key::R, KeyType::KeyCode },
    { Key::S, KeyType::KeyCode },
    { Key::T, KeyType::KeyCode },
    { Key::U, KeyType::KeyCode },
    { Key::V, KeyType::KeyCode },
    { Key::W, KeyType::KeyCode },
    { Key::X, KeyType::KeyCode },
    { Key::Y, KeyType::KeyCode },
    { Key::Z, KeyType::KeyCode },

    // Scan codes
    { Key::F1, KeyType::ScanCode },
    { Key::F2, KeyType::ScanCode },
    { Key::F3, KeyType::ScanCode },
    { Key::F4, KeyType::ScanCode },
    { Key::F5, KeyType::ScanCode },
    { Key::F6, KeyType::ScanCode },
    { Key::F7, KeyType::ScanCode },
    { Key::F8, KeyType::ScanCode },
    { Key::F9, KeyType::ScanCode },
    { Key::F10, KeyType::ScanCode },
    { Key::F11, KeyType::ScanCode },
    { Key::F12, KeyType::ScanCode },
};

const std::map<SDLKeyboard::Key, SDLKeyboard::KeyCode> SDLKeyboard::keyToKeyCodeMap =
{
    { Key::TAB, input::Keyboard::KeyCode::TAB },
    { Key::CLEAR, input::Keyboard::KeyCode::CLEAR },
    { Key::RETURN, input::Keyboard::KeyCode::RETURN },
    { Key::PAUSE, input::Keyboard::KeyCode::PAUSE },
    { Key::ESCAPE, input::Keyboard::KeyCode::ESCAPE },
    { Key::SPACE, input::Keyboard::KeyCode::SPACE },
    { Key::EXCLAMATION, input::Keyboard::KeyCode::EXCLAIM },
    { Key::DOUBLE_QUOTE, input::Keyboard::KeyCode::QUOTEDBL },
    { Key::HASH, input::Keyboard::KeyCode::HASH },
    { Key::DOLLAR, input::Keyboard::KeyCode::DOLLAR },
    { Key::PERCENT, input::Keyboard::KeyCode::PERCENT },
    { Key::AMPERSAND, input::Keyboard::KeyCode::AMPERSAND },
    { Key::QUOTE, input::Keyboard::KeyCode::QUOTE },
    { Key::OPEN_PAREN, input::Keyboard::KeyCode::LEFTPAREN },
    { Key::CLOSE_PAREN, input::Keyboard::KeyCode::RIGHTPAREN },
    { Key::ASTERISK, input::Keyboard::KeyCode::ASTERISK },
    { Key::PLUS, input::Keyboard::KeyCode::PLUS },
    { Key::COMMA, input::Keyboard::KeyCode::COMMA },
    { Key::HYPHEN_MINUS, input::Keyboard::KeyCode::MINUS },
    { Key::PERIOD, input::Keyboard::KeyCode::PERIOD },
    { Key::SLASH, input::Keyboard::KeyCode::SLASH },
    { Key::_0, input::Keyboard::KeyCode::_0 },
    { Key::_1, input::Keyboard::KeyCode::_1 },
    { Key::_2, input::Keyboard::KeyCode::_2 },
    { Key::_3, input::Keyboard::KeyCode::_3 },
    { Key::_4, input::Keyboard::KeyCode::_4 },
    { Key::_5, input::Keyboard::KeyCode::_5 },
    { Key::_6, input::Keyboard::KeyCode::_6 },
    { Key::_7, input::Keyboard::KeyCode::_7 },
    { Key::_8, input::Keyboard::KeyCode::_8 },
    { Key::_9, input::Keyboard::KeyCode::_9 },
    { Key::COLON, input::Keyboard::KeyCode::COLON },
    { Key::SEMICOLON, input::Keyboard::KeyCode::SEMICOLON },

    { Key::A, input::Keyboard::KeyCode::A },
    { Key::B, input::Keyboard::KeyCode::B },
    { Key::C, input::Keyboard::KeyCode::C },
    { Key::D, input::Keyboard::KeyCode::D },
    { Key::E, input::Keyboard::KeyCode::E },
    { Key::F, input::Keyboard::KeyCode::F },
    { Key::G, input::Keyboard::KeyCode::G },
    { Key::H, input::Keyboard::KeyCode::H },
    { Key::I, input::Keyboard::KeyCode::I },
    { Key::J, input::Keyboard::KeyCode::J },
    { Key::K, input::Keyboard::KeyCode::K },
    { Key::L, input::Keyboard::KeyCode::L },
    { Key::M, input::Keyboard::KeyCode::M },
    { Key::N, input::Keyboard::KeyCode::N },
    { Key::O, input::Keyboard::KeyCode::O },
    { Key::P, input::Keyboard::KeyCode::P },
    { Key::Q, input::Keyboard::KeyCode::Q },
    { Key::R, input::Keyboard::KeyCode::R },
    { Key::S, input::Keyboard::KeyCode::S },
    { Key::T, input::Keyboard::KeyCode::T },
    { Key::U, input::Keyboard::KeyCode::U },
    { Key::V, input::Keyboard::KeyCode::V },
    { Key::W, input::Keyboard::KeyCode::W },
    { Key::X, input::Keyboard::KeyCode::X },
    { Key::Y, input::Keyboard::KeyCode::Y },
    { Key::Z, input::Keyboard::KeyCode::Z },
};

const std::map<SDLKeyboard::Key, SDLKeyboard::ScanCode> SDLKeyboard::keyToScanCodeMap =
{
    { Key::F1, input::Keyboard::ScanCode::F1 },
    { Key::F2, input::Keyboard::ScanCode::F2 },
    { Key::F3, input::Keyboard::ScanCode::F3 },
    { Key::F4, input::Keyboard::ScanCode::F4 },
    { Key::F5, input::Keyboard::ScanCode::F5 },
    { Key::F6, input::Keyboard::ScanCode::F6 },
    { Key::F7, input::Keyboard::ScanCode::F7 },
    { Key::F8, input::Keyboard::ScanCode::F8 },
    { Key::F9, input::Keyboard::ScanCode::F9 },
    { Key::F10, input::Keyboard::ScanCode::F10 },
    { Key::F11, input::Keyboard::ScanCode::F11 },
    { Key::F12, input::Keyboard::ScanCode::F12 },
};

SDLKeyboard::SDLKeyboard()
{
    _keyboardState = SDL_GetKeyboardState(NULL);

    std::cout << SDL_SCANCODE_TO_KEYCODE(4) << std::endl;
}

bool SDLKeyboard::keyIsDown(input::Keyboard::ScanCode scanCode)
{
#if defined(EMSCRIPTEN)
    return _keyboardState[static_cast<int>(getKeyCodeFromScanCode(scanCode))] != 0;
#else
    return _keyboardState[static_cast<int>(scanCode)] != 0;
#endif
    
    //return _keyboardState[static_cast<int>(scanCode)] != 0;
}

bool SDLKeyboard::keyIsDown(input::Keyboard::KeyCode keyCode)
{
/*
#if defined(EMSCRIPTEN)
    // Note: bug in emscripten, GetKeyStates is indexed by key codes.
    return _keyboardState[static_cast<int>(keyCode)] != 0;
#else
    return _keyboardState[static_cast<int>(getScanCodeFromKeyCode(keyCode))] != 0;
#endif
*/
    return _keyboardState[static_cast<int>(keyCode)] != 0;
}

bool SDLKeyboard::keyIsDown(input::Keyboard::Key key)
{
    auto keyType = keyTypeMap.find(key);

    // This key is not properly mapped (we don't know if it comes from a key code or a scan code)
    if (keyType == keyTypeMap.end())
        return false;

    // This key only has a key code
    if (keyType->second == KeyType::KeyCode)
    {
        auto keyCode = keyToKeyCodeMap.find(key);

        // This key hasn't any key -> key code mapping
        if (keyCode == keyToKeyCodeMap.end())
            return false;

#if defined(EMSCRIPTEN)
        // Note: bug in emscripten, GetKeyStates is indexed by key codes.
        return _keyboardState[static_cast<int>(keyCode->second)] != 0;
#else
        return _keyboardState[static_cast<int>(
            getScanCodeFromKeyCode(static_cast<input::Keyboard::KeyCode>(keyCode->second)))] != 0;
#endif
    }
    // This key only has a scan code
    else if (keyType->second == KeyType::ScanCode)
    {
        auto scanCode = keyToScanCodeMap.find(key);

        // This key hasn't any key -> key code mapping
        if (scanCode == keyToScanCodeMap.end())
            return false;

#if defined(EMSCRIPTEN)
        return _keyboardState[static_cast<int>(
            getKeyCodeFromScanCode(static_cast<input::Keyboard::ScanCode>(scanCode->second)))] != 0;
#else
        return _keyboardState[static_cast<int>(scanCode->second)] != 0;
#endif
    }
    // This key has a scan code and a key code!
    else
    {

    }

    return false;

}

input::Keyboard::KeyCode SDLKeyboard::getKeyCodeFromScanCode(input::Keyboard::ScanCode scanCode)
{
    return static_cast<input::Keyboard::KeyCode>(SDL_SCANCODE_TO_KEYCODE(static_cast<int>(scanCode)));
}

input::Keyboard::ScanCode SDLKeyboard::getScanCodeFromKeyCode(input::Keyboard::KeyCode keyCode)
{
    return static_cast<input::Keyboard::ScanCode>(SDL_GetScancodeFromKey(static_cast<int>(keyCode)));
}
