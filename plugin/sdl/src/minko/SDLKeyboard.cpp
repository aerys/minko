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

    { Key::DELETE, KeyType::KeyCode },
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
    { Key::HANGUL, KeyType::ScanCode },
    { Key::EISU, KeyType::ScanCode },
    { Key::JUNJA, KeyType::ScanCode },
    { Key::FINAL, KeyType::ScanCode },
    { Key::HANJA, KeyType::ScanCode },
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

const std::map<SDLKeyboard::Key, SDLKeyboard::KeyCode> SDLKeyboard::keyToKeyCodeMap =
{
    { Key::CANCEL, input::Keyboard::KeyCode::CANCEL },
    { Key::BACK_SPACE, input::Keyboard::KeyCode::BACKSPACE },
    { Key::TAB, input::Keyboard::KeyCode::TAB },
    { Key::CLEAR, input::Keyboard::KeyCode::CLEAR },
    { Key::RETURN, input::Keyboard::KeyCode::RETURN },

    { Key::ESCAPE, input::Keyboard::KeyCode::ESCAPE },

    // Supported on Linux with Gecko 4.0
    { Key::CONVERT, input::Keyboard::KeyCode::FS },
    { Key::NONCONVERT, input::Keyboard::KeyCode::GS },
    { Key::ACCEPT, input::Keyboard::KeyCode::RS },
    { Key::MODECHANGE, input::Keyboard::KeyCode::US },

    { Key::SPACE, input::Keyboard::KeyCode::SPACE },

    { Key::DELETE, input::Keyboard::KeyCode::DELETE },
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
    { Key::LESS_THAN, input::Keyboard::KeyCode::LESS },
    { Key::EQUALS, input::Keyboard::KeyCode::EQUALS },
    { Key::GREATER_THAN, input::Keyboard::KeyCode::GREATER },
    { Key::QUESTION_MARK, input::Keyboard::KeyCode::QUESTION },
    { Key::AT, input::Keyboard::KeyCode::AT },
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

    //{ Key::WIN_OEM_FJ_JISHO, input::Keyboard::KeyCode::WIN_OEM_FJ_JISHO },
    //{ Key::WIN_OEM_FJ_MASSHOU, input::Keyboard::KeyCode::WIN_OEM_FJ_MASSHOU },
    //{ Key::WIN_OEM_FJ_TOUROKU, input::Keyboard::KeyCode::WIN_OEM_FJ_TOUROKU },
    //{ Key::WIN_OEM_FJ_LOYA, input::Keyboard::KeyCode::WIN_OEM_FJ_LOYA },
    //{ Key::WIN_OEM_FJ_ROYA, input::Keyboard::KeyCode::WIN_OEM_FJ_ROYA },

    { Key::CIRCUMFLEX, input::Keyboard::KeyCode::CARET },
    { Key::EXCLAMATION, input::Keyboard::KeyCode::EXCLAIM },
    { Key::DOUBLE_QUOTE, input::Keyboard::KeyCode::QUOTEDBL },
    { Key::HASH, input::Keyboard::KeyCode::HASH },
    { Key::DOLLAR, input::Keyboard::KeyCode::DOLLAR },
    { Key::PERCENT, input::Keyboard::KeyCode::PERCENT },
    { Key::AMPERSAND, input::Keyboard::KeyCode::AMPERSAND },
    { Key::UNDERSCORE, input::Keyboard::KeyCode::UNDERSCORE },
    { Key::OPEN_PAREN, input::Keyboard::KeyCode::LEFTPAREN },
    { Key::CLOSE_PAREN, input::Keyboard::KeyCode::RIGHTPAREN },
    { Key::ASTERISK, input::Keyboard::KeyCode::ASTERISK },
    { Key::PLUS, input::Keyboard::KeyCode::PLUS },
    { Key::PIPE, input::Keyboard::KeyCode::PIPE },
    { Key::HYPHEN_MINUS, input::Keyboard::KeyCode::MINUS },
    { Key::OPEN_CURLY_BRACKET, input::Keyboard::KeyCode::LEFTCURLYBRACKET },
    { Key::CLOSE_CURLY_BRACKET, input::Keyboard::KeyCode::RIGHTCURLYBRACKET },
    { Key::TILDE, input::Keyboard::KeyCode::TILDE },

    { Key::COMMA, input::Keyboard::KeyCode::COMMA },
    { Key::PERIOD, input::Keyboard::KeyCode::PERIOD },
    { Key::SLASH, input::Keyboard::KeyCode::SLASH },
    { Key::BACK_QUOTE, input::Keyboard::KeyCode::BACKQUOTE },
    { Key::OPEN_BRACKET, input::Keyboard::KeyCode::LEFTBRACKET },
    { Key::BACK_SLASH, input::Keyboard::KeyCode::BACKSLASH },
    { Key::CLOSE_BRACKET, input::Keyboard::KeyCode::RIGHTBRACKET },
    { Key::QUOTE, input::Keyboard::KeyCode::QUOTE },

    //{ Key::WIN_ICO_HELP, input::Keyboard::KeyCode::WIN_ICO_HELP },
    //{ Key::WIN_ICO_00, input::Keyboard::KeyCode::WIN_ICO_00 },
    //{ Key::WIN_ICO_CLEAR, input::Keyboard::KeyCode::WIN_ICO_CLEAR },
    //{ Key::WIN_OEM_RESET, input::Keyboard::KeyCode::WIN_OEM_RESET },
    //{ Key::WIN_OEM_JUMP, input::Keyboard::KeyCode::WIN_OEM_JUMP },
    //{ Key::WIN_OEM_PA1, input::Keyboard::KeyCode::WIN_OEM_PA1 },
    //{ Key::WIN_OEM_PA2, input::Keyboard::KeyCode::WIN_OEM_PA2 },
    //{ Key::WIN_OEM_PA3, input::Keyboard::KeyCode::WIN_OEM_PA3 },
    //{ Key::WIN_OEM_WSCTRL, input::Keyboard::KeyCode::WIN_OEM_WSCTRL },
    //{ Key::WIN_OEM_CUSEL, input::Keyboard::KeyCode::WIN_OEM_CUSEL },
    //{ Key::WIN_OEM_ATTN, input::Keyboard::KeyCode::WIN_OEM_ATTN },
    //{ Key::WIN_OEM_FINISH, input::Keyboard::KeyCode::WIN_OEM_FINISH },
    //{ Key::WIN_OEM_COPY, input::Keyboard::KeyCode::WIN_OEM_COPY },
    //{ Key::WIN_OEM_AUTO, input::Keyboard::KeyCode::WIN_OEM_AUTO },
    //{ Key::WIN_OEM_ENLW, input::Keyboard::KeyCode::WIN_OEM_ENLW },
    //{ Key::WIN_OEM_BACKTAB, input::Keyboard::KeyCode::WIN_OEM_BACKTAB },
    //{ Key::ATTN, input::Keyboard::KeyCode::ATTN },
    //{ Key::CRSEL, input::Keyboard::KeyCode::CRSEL },
    //{ Key::EXSEL, input::Keyboard::KeyCode::EXSEL },
    //{ Key::EREOF, input::Keyboard::KeyCode::EREOF },
    //{ Key::PLAY, input::Keyboard::KeyCode::PLAY },
    //{ Key::ZOOM, input::Keyboard::KeyCode::ZOOM },
    //{ Key::PA1, input::Keyboard::KeyCode::PA1 },
    //{ Key::WIN_OEM_CLEAR, input::Keyboard::KeyCode::WIN_OEM_CLEAR },
};

const std::map<SDLKeyboard::Key, SDLKeyboard::ScanCode> SDLKeyboard::keyToScanCodeMap =
{
    { Key::HELP, input::Keyboard::ScanCode::HELP },

    { Key::SCROLL_LOCK, input::Keyboard::ScanCode::SCROLLLOCK },

    { Key::KANA, input::Keyboard::ScanCode::LANG3 },
    { Key::HANGUL, input::Keyboard::ScanCode::LANG1 },
    { Key::EISU, input::Keyboard::ScanCode::LANG2 },
    { Key::JUNJA, input::Keyboard::ScanCode::LANG4 },
    { Key::FINAL, input::Keyboard::ScanCode::LANG5 },
    { Key::HANJA, input::Keyboard::ScanCode::LANG6 },
    { Key::KANJI, input::Keyboard::ScanCode::LANG7 },

    { Key::SHIFT, input::Keyboard::ScanCode::LSHIFT },
    { Key::CONTROL, input::Keyboard::ScanCode::LCTRL },
    { Key::ALT, input::Keyboard::ScanCode::LALT },
    { Key::PAUSE, input::Keyboard::ScanCode::PAUSE },
    { Key::CAPS_LOCK, input::Keyboard::ScanCode::CAPSLOCK },

    { Key::PAGE_UP, input::Keyboard::ScanCode::PAGEUP },
    { Key::PAGE_DOWN, input::Keyboard::ScanCode::PAGEDOWN },
    { Key::END, input::Keyboard::ScanCode::END },
    { Key::HOME, input::Keyboard::ScanCode::HOME },
    { Key::LEFT, input::Keyboard::ScanCode::LEFT },
    { Key::UP, input::Keyboard::ScanCode::UP },
    { Key::RIGHT, input::Keyboard::ScanCode::RIGHT },
    { Key::DOWN, input::Keyboard::ScanCode::DOWN },
    { Key::SELECT, input::Keyboard::ScanCode::SELECT },
    //{ Key::PRINT, input::Keyboard::ScanCode::PRINT },
    { Key::EXECUTE, input::Keyboard::ScanCode::EXECUTE },
    { Key::PRINTSCREEN, input::Keyboard::ScanCode::PRINTSCREEN },
    { Key::INSERT, input::Keyboard::ScanCode::INSERT },

    { Key::META, input::Keyboard::ScanCode::RGUI },
    { Key::ALTGR, input::Keyboard::ScanCode::RALT },
    { Key::WIN, input::Keyboard::ScanCode::LGUI },
    { Key::CONTEXT_MENU, input::Keyboard::ScanCode::APPLICATION },
    { Key::SLEEP, input::Keyboard::ScanCode::SLEEP },
    { Key::NUMPAD0, input::Keyboard::ScanCode::KP_0 },
    { Key::NUMPAD1, input::Keyboard::ScanCode::KP_1 },
    { Key::NUMPAD2, input::Keyboard::ScanCode::KP_2 },
    { Key::NUMPAD3, input::Keyboard::ScanCode::KP_3 },
    { Key::NUMPAD4, input::Keyboard::ScanCode::KP_4 },
    { Key::NUMPAD5, input::Keyboard::ScanCode::KP_5 },
    { Key::NUMPAD6, input::Keyboard::ScanCode::KP_6 },
    { Key::NUMPAD7, input::Keyboard::ScanCode::KP_7 },
    { Key::NUMPAD8, input::Keyboard::ScanCode::KP_8 },
    { Key::NUMPAD9, input::Keyboard::ScanCode::KP_9 },
    { Key::MULTIPLY, input::Keyboard::ScanCode::KP_MULTIPLY },
    { Key::ADD, input::Keyboard::ScanCode::KP_PLUS },
    { Key::SEPARATOR, input::Keyboard::ScanCode::SEPARATOR },
    { Key::SUBTRACT, input::Keyboard::ScanCode::KP_MINUS },
    { Key::DECIMAL, input::Keyboard::ScanCode::KP_DECIMAL },
    { Key::DIVIDE, input::Keyboard::ScanCode::KP_DIVIDE },

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
    { Key::F13, input::Keyboard::ScanCode::F13 },
    { Key::F14, input::Keyboard::ScanCode::F14 },
    { Key::F15, input::Keyboard::ScanCode::F15 },
    { Key::F16, input::Keyboard::ScanCode::F16 },
    { Key::F17, input::Keyboard::ScanCode::F17 },
    { Key::F18, input::Keyboard::ScanCode::F18 },
    { Key::F19, input::Keyboard::ScanCode::F19 },
    { Key::F20, input::Keyboard::ScanCode::F20 },
    { Key::F21, input::Keyboard::ScanCode::F21 },
    { Key::F22, input::Keyboard::ScanCode::F22 },
    { Key::F23, input::Keyboard::ScanCode::F23 },
    { Key::F24, input::Keyboard::ScanCode::F24 },
    { Key::NUM_LOCK, input::Keyboard::ScanCode::NUMLOCKCLEAR },

    { Key::VOLUME_MUTE, input::Keyboard::ScanCode::MUTE },
    { Key::VOLUME_DOWN, input::Keyboard::ScanCode::VOLUMEDOWN },
    { Key::VOLUME_UP, input::Keyboard::ScanCode::VOLUMEUP },

    // Additional keys
    { Key::CONTROL_RIGHT, input::Keyboard::ScanCode::RCTRL },
    { Key::SHIFT_RIGHT, input::Keyboard::ScanCode::RSHIFT },
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
