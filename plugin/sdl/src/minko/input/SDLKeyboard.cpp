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

#include "minko/input/SDLKeyboard.hpp"

#include "SDL.h"

using namespace minko;
using namespace minko::input;

const std::unordered_map<SDLKeyboard::Key, SDLKeyboard::KeyType> SDLKeyboard::keyTypeMap =
{
    // Key codes

    { Key::CANCEL, KeyType::KeyCode },
    { Key::BACK_SPACE, KeyType::KeyCode },
    { Key::TAB, KeyType::KeyCode },
    { Key::CLEAR, KeyType::KeyCode },
    { Key::RETURN, KeyType::KeyCode },
    // { Key::ENTER, KeyType::KeyCode }, // Obsolete (reserved but not used)

    { Key::ESCAPE, KeyType::KeyCode },
    { Key::CONVERT, KeyType::KeyCode },
    { Key::NONCONVERT, KeyType::KeyCode },
    { Key::ACCEPT, KeyType::KeyCode },
    { Key::MODECHANGE, KeyType::KeyCode },
    { Key::SPACE, KeyType::KeyCode },

    { Key::DEL, KeyType::KeyCode },
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
    { Key::LESS_THAN, KeyType::KeyCode },
    { Key::EQUALS, KeyType::KeyCode },
    { Key::GREATER_THAN, KeyType::KeyCode },
    { Key::QUESTION_MARK, KeyType::KeyCode },
    { Key::AT, KeyType::KeyCode },
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

    //{ Key::WIN_OEM_FJ_JISHO, KeyType::KeyCode },
    //{ Key::WIN_OEM_FJ_MASSHOU, KeyType::KeyCode },
    //{ Key::WIN_OEM_FJ_TOUROKU, KeyType::KeyCode },
    //{ Key::WIN_OEM_FJ_LOYA, KeyType::KeyCode },
    //{ Key::WIN_OEM_FJ_ROYA, KeyType::KeyCode },

    { Key::CIRCUMFLEX, KeyType::KeyCode },
    { Key::EXCLAMATION, KeyType::KeyCode },
    { Key::DOUBLE_QUOTE, KeyType::KeyCode },
    { Key::HASH, KeyType::KeyCode },
    { Key::DOLLAR, KeyType::KeyCode },
    { Key::PERCENT, KeyType::KeyCode },
    { Key::AMPERSAND, KeyType::KeyCode },
    { Key::UNDERSCORE, KeyType::KeyCode },
    { Key::OPEN_PAREN, KeyType::KeyCode },
    { Key::CLOSE_PAREN, KeyType::KeyCode },
    { Key::ASTERISK, KeyType::KeyCode },
    { Key::PLUS, KeyType::KeyCode },
    { Key::PIPE, KeyType::KeyCode },
    { Key::HYPHEN_MINUS, KeyType::KeyCode },
    { Key::OPEN_CURLY_BRACKET, KeyType::KeyCode },
    { Key::CLOSE_CURLY_BRACKET, KeyType::KeyCode },
    { Key::TILDE, KeyType::KeyCode },
    
    { Key::COMMA, KeyType::KeyCode },
    { Key::PERIOD, KeyType::KeyCode },
    { Key::SLASH, KeyType::KeyCode },
    { Key::BACK_QUOTE, KeyType::KeyCode },
    { Key::OPEN_BRACKET, KeyType::KeyCode },
    { Key::BACK_SLASH, KeyType::KeyCode },
    { Key::CLOSE_BRACKET, KeyType::KeyCode },
    { Key::QUOTE, KeyType::KeyCode },

    { Key::WIN_ICO_HELP, KeyType::KeyCode },
    { Key::WIN_ICO_00, KeyType::KeyCode },
    { Key::WIN_ICO_CLEAR, KeyType::KeyCode },
    { Key::WIN_OEM_RESET, KeyType::KeyCode },
    { Key::WIN_OEM_JUMP, KeyType::KeyCode },
    { Key::WIN_OEM_PA1, KeyType::KeyCode },
    { Key::WIN_OEM_PA2, KeyType::KeyCode },
    { Key::WIN_OEM_PA3, KeyType::KeyCode },
    { Key::WIN_OEM_WSCTRL, KeyType::KeyCode },
    { Key::WIN_OEM_CUSEL, KeyType::KeyCode },
    { Key::WIN_OEM_ATTN, KeyType::KeyCode },
    { Key::WIN_OEM_FINISH, KeyType::KeyCode },
    { Key::WIN_OEM_COPY, KeyType::KeyCode },
    { Key::WIN_OEM_AUTO, KeyType::KeyCode },
    { Key::WIN_OEM_ENLW, KeyType::KeyCode },
    { Key::WIN_OEM_BACKTAB, KeyType::KeyCode },
    { Key::ATTN, KeyType::KeyCode },
    { Key::CRSEL, KeyType::KeyCode },
    { Key::EXSEL, KeyType::KeyCode },
    { Key::EREOF, KeyType::KeyCode },
    { Key::PLAY, KeyType::KeyCode },
    { Key::ZOOM, KeyType::KeyCode },
    { Key::PA1, KeyType::KeyCode },
    { Key::WIN_OEM_CLEAR, KeyType::KeyCode },

    // Scan codes

    { Key::HELP, KeyType::ScanCode },

    { Key::SCROLL_LOCK, KeyType::ScanCode },

    { Key::KANA, KeyType::ScanCode },
    //{ Key::HANGUL, KeyType::ScanCode },
    { Key::EISU, KeyType::ScanCode },
    { Key::JUNJA, KeyType::ScanCode },
    { Key::FINAL, KeyType::ScanCode },
    //{ Key::HANJA, KeyType::ScanCode },
    { Key::KANJI, KeyType::ScanCode },

    { Key::SHIFT, KeyType::ScanCode },
    { Key::CONTROL, KeyType::ScanCode },
    { Key::ALT, KeyType::ScanCode },
    { Key::PAUSE, KeyType::ScanCode },
    { Key::CAPS_LOCK, KeyType::ScanCode },

    { Key::PAGE_UP, KeyType::ScanCode },
    { Key::PAGE_DOWN, KeyType::ScanCode },
    { Key::END, KeyType::ScanCode },
    { Key::HOME, KeyType::ScanCode },
    { Key::LEFT, KeyType::ScanCode },
    { Key::UP, KeyType::ScanCode },
    { Key::RIGHT, KeyType::ScanCode },
    { Key::DOWN, KeyType::ScanCode },
    { Key::SELECT, KeyType::ScanCode },
    { Key::PRINT, KeyType::ScanCode },
    { Key::EXECUTE, KeyType::ScanCode },
    { Key::PRINTSCREEN, KeyType::ScanCode },
    { Key::INSERT, KeyType::ScanCode },

    { Key::META, KeyType::ScanCode },
    { Key::ALTGR, KeyType::ScanCode },

    { Key::WIN, KeyType::ScanCode },
    { Key::CONTEXT_MENU, KeyType::ScanCode },
    { Key::SLEEP, KeyType::ScanCode },
    { Key::NUMPAD0, KeyType::ScanCode },
    { Key::NUMPAD1, KeyType::ScanCode },
    { Key::NUMPAD2, KeyType::ScanCode },
    { Key::NUMPAD3, KeyType::ScanCode },
    { Key::NUMPAD4, KeyType::ScanCode },
    { Key::NUMPAD5, KeyType::ScanCode },
    { Key::NUMPAD6, KeyType::ScanCode },
    { Key::NUMPAD7, KeyType::ScanCode },
    { Key::NUMPAD8, KeyType::ScanCode },
    { Key::NUMPAD9, KeyType::ScanCode },
    { Key::MULTIPLY, KeyType::ScanCode },
    { Key::ADD, KeyType::ScanCode },
    { Key::SEPARATOR, KeyType::ScanCode },
    { Key::SUBTRACT, KeyType::ScanCode },
    { Key::DECIMAL, KeyType::ScanCode },
    { Key::DIVIDE, KeyType::ScanCode },

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
    { Key::F13, KeyType::ScanCode },
    { Key::F14, KeyType::ScanCode },
    { Key::F15, KeyType::ScanCode },
    { Key::F16, KeyType::ScanCode },
    { Key::F17, KeyType::ScanCode },
    { Key::F18, KeyType::ScanCode },
    { Key::F19, KeyType::ScanCode },
    { Key::F20, KeyType::ScanCode },
    { Key::F21, KeyType::ScanCode },
    { Key::F22, KeyType::ScanCode },
    { Key::F23, KeyType::ScanCode },
    { Key::F24, KeyType::ScanCode },
    { Key::NUM_LOCK, KeyType::ScanCode },

    { Key::VOLUME_MUTE, KeyType::ScanCode },
    { Key::VOLUME_DOWN, KeyType::ScanCode },
    { Key::VOLUME_UP, KeyType::ScanCode },

    // Additional keys (specific to Minko and for native support of some keys)
    { Key::CONTROL_RIGHT, KeyType::ScanCode },
    { Key::SHIFT_RIGHT, KeyType::ScanCode },
};

SDLKeyboard::SDLKeyboard()
{
    _keyboardState = SDL_GetKeyboardState(NULL);
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
        auto keyCode = input::KeyMap::keyToKeyCodeMap.find(key);

        // This key hasn't any key -> key code mapping
        if (keyCode == input::KeyMap::keyToKeyCodeMap.end())
            return false;

#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
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
        auto scanCode = input::KeyMap::keyToScanCodeMap.find(key);

        // This key hasn't any key -> key code mapping
        if (scanCode == input::KeyMap::keyToScanCodeMap.end())
            return false;

#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
        return _keyboardState[static_cast<int>(
            getKeyCodeFromScanCode(static_cast<input::Keyboard::ScanCode>(scanCode->second)))] != 0;
#else
        return _keyboardState[static_cast<int>(scanCode->second)] != 0;
#endif
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
