/*
Copyright (c) 2014 Aerys

Permission is hereby granted; free of charge; to any person obtaining a copy of this software and
associated documentation files (the "Software"); to deal in the Software without restriction;
including without limitation the rights to use; copy; modify; merge; publish; distribute;
sublicense; and/or sell copies of the Software; and to permit persons to whom the Software is
furnished to do so; subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS"; WITHOUT WARRANTY OF ANY KIND; EXPRESS OR IMPLIED; INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY; FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM;
DAMAGES OR OTHER LIABILITY; WHETHER IN AN ACTION OF CONTRACT; TORT OR OTHERWISE; ARISING FROM;
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/input/Keyboard.hpp"

using namespace minko;
using namespace minko::input;

const Keyboard::KeyToNameArray Keyboard::_keyToName = initializeKeyNames();

const Keyboard::KeyToNameArray
Keyboard::initializeKeyNames()
{
    KeyToNameArray names;

    names[3] = "CANCEL";
    names[6] = "HELP";
    names[8] = "BACK_SPACE";
    names[9] = "TAB";
    names[12] = "CLEAR";
    names[13] = "RETURN";
    names[14] = "ENTER";
    names[16] = "SHIFT";
    names[17] = "CONTROL";
    names[18] = "ALT";
    names[19] = "PAUSE";
    names[20] = "CAPS_LOCK";
    names[21] = "KANA";
    //names[21] = "HANGUL";
    names[22] = "EISU";
    names[23] = "JUNJA";
    names[24] = "FINAL";
    //names[25] = "HANJA";
    names[25] = "KANJI";
    names[27] = "ESCAPE";
    names[28] = "CONVERT";
    names[29] = "NONCONVERT";
    names[30] = "ACCEPT";
    names[31] = "MODECHANGE";
    names[32] = "SPACE";
    names[33] = "PAGE_UP";
    names[34] = "PAGE_DOWN";
    names[35] = "END";
    names[36] = "HOME";
    names[37] = "LEFT";
    names[38] = "UP";
    names[39] = "RIGHT";
    names[40] = "DOWN";
    names[41] = "SELECT";
    names[42] = "PRINT";
    names[43] = "EXECUTE";
    names[44] = "PRINTSCREEN";
    names[45] = "INSERT";
    names[46] = "DELETE";
    names[48] = "_0";
    names[49] = "_1";
    names[50] = "_2";
    names[51] = "_3";
    names[52] = "_4";
    names[53] = "_5";
    names[54] = "_6";
    names[55] = "_7";
    names[56] = "_8";
    names[57] = "_9";
    names[58] = "COLON";
    names[59] = "SEMICOLON";
    names[60] = "LESS_THAN";
    names[61] = "EQUALS";
    names[62] = "GREATER_THAN";
    names[63] = "QUESTION_MARK";
    names[64] = "AT";
    names[65] = "A";
    names[66] = "B";
    names[67] = "C";
    names[68] = "D";
    names[69] = "E";
    names[70] = "F";
    names[71] = "G";
    names[72] = "H";
    names[73] = "I";
    names[74] = "J";
    names[75] = "K";
    names[76] = "L";
    names[77] = "M";
    names[78] = "N";
    names[79] = "O";
    names[80] = "P";
    names[81] = "Q";
    names[82] = "R";
    names[83] = "S";
    names[84] = "T";
    names[85] = "U";
    names[86] = "V";
    names[87] = "W";
    names[88] = "X";
    names[89] = "Y";
    names[90] = "Z";
    names[91] = "WIN";
    names[93] = "CONTEXT_MENU";
    names[95] = "SLEEP";
    names[96] = "NUMPAD0";
    names[97] = "NUMPAD1";
    names[98] = "NUMPAD2";
    names[99] = "NUMPAD3";
    names[100] = "NUMPAD4";
    names[101] = "NUMPAD5";
    names[102] = "NUMPAD6";
    names[103] = "NUMPAD7";
    names[104] = "NUMPAD8";
    names[105] = "NUMPAD9";
    names[106] = "MULTIPLY";
    names[107] = "ADD";
    names[108] = "SEPARATOR";
    names[109] = "SUBTRACT";
    names[110] = "DECIMAL";
    names[111] = "DIVIDE";
    names[112] = "F1";
    names[113] = "F2";
    names[114] = "F3";
    names[115] = "F4";
    names[116] = "F5";
    names[117] = "F6";
    names[118] = "F7";
    names[119] = "F8";
    names[120] = "F9";
    names[121] = "F10";
    names[122] = "F11";
    names[123] = "F12";
    names[124] = "F13";
    names[125] = "F14";
    names[126] = "F15";
    names[127] = "F16";
    names[128] = "F17";
    names[129] = "F18";
    names[130] = "F19";
    names[131] = "F20";
    names[132] = "F21";
    names[133] = "F22";
    names[134] = "F23";
    names[135] = "F24";
    names[144] = "NUM_LOCK";
    names[145] = "SCROLL_LOCK";
    names[146] = "WIN_OEM_FJ_JISHO";
    names[147] = "WIN_OEM_FJ_MASSHOU";
    names[148] = "WIN_OEM_FJ_TOUROKU";
    names[149] = "WIN_OEM_FJ_LOYA";
    names[150] = "WIN_OEM_FJ_ROYA";
    names[160] = "CIRCUMFLEX";
    names[161] = "EXCLAMATION";
    names[162] = "DOUBLE_QUOTE";
    names[163] = "HASH";
    names[164] = "DOLLAR";
    names[165] = "PERCENT";
    names[166] = "AMPERSAND";
    names[167] = "UNDERSCORE";
    names[168] = "OPEN_PAREN";
    names[169] = "CLOSE_PAREN";
    names[170] = "ASTERISK";
    names[171] = "PLUS";
    names[172] = "PIPE";
    names[173] = "HYPHEN_MINUS";
    names[174] = "OPEN_CURLY_BRACKET";
    names[175] = "CLOSE_CURLY_BRACKET";
    names[176] = "TILDE";
    names[181] = "VOLUME_MUTE";
    names[182] = "VOLUME_DOWN";
    names[183] = "VOLUME_UP";
    names[188] = "COMMA";
    names[190] = "PERIOD";
    names[191] = "SLASH";
    names[192] = "BACK_QUOTE";
    names[219] = "OPEN_BRACKET";
    names[220] = "BACK_SLASH";
    names[221] = "CLOSE_BRACKET";
    names[222] = "QUOTE";
    names[224] = "META";
    names[225] = "ALTGR";
    names[227] = "WIN_ICO_HELP";
    names[228] = "WIN_ICO_00";
    names[230] = "WIN_ICO_CLEAR";
    names[233] = "WIN_OEM_RESET";
    names[234] = "WIN_OEM_JUMP";
    names[235] = "WIN_OEM_PA1";
    names[236] = "WIN_OEM_PA2";
    names[237] = "WIN_OEM_PA3";
    names[238] = "WIN_OEM_WSCTRL";
    names[239] = "WIN_OEM_CUSEL";
    names[240] = "WIN_OEM_ATTN";
    names[241] = "WIN_OEM_FINISH";
    names[242] = "WIN_OEM_COPY";
    names[243] = "WIN_OEM_AUTO";
    names[244] = "WIN_OEM_ENLW";
    names[245] = "WIN_OEM_BACKTAB";
    names[246] = "ATTN";
    names[247] = "CRSEL";
    names[248] = "EXSEL";
    names[249] = "EREOF";
    names[250] = "PLAY";
    names[251] = "ZOOM";
    names[253] = "PA1";
    names[254] = "WIN_OEM_CLEAR";

    // Additional keys (specific to Minko and for native support of some keys)
    names[300] = "CONTROL_RIGHT";
    names[301] = "SHIFT_RIGHT";

    return names;
}

Keyboard::Keyboard() :
    _down(Signal<Ptr>::create()),
    _up(Signal<Ptr>::create()),
    _textInput(Signal<Ptr, char16_t>::create())
{
}

const std::unordered_map<Keyboard::Key, Keyboard::KeyCode> KeyMap::keyToKeyCodeMap =
{
    { Keyboard::Key::CANCEL, input::Keyboard::KeyCode::CANCEL },
    { Keyboard::Key::BACK_SPACE, input::Keyboard::KeyCode::BACKSPACE },
    { Keyboard::Key::TAB, input::Keyboard::KeyCode::TAB },
    { Keyboard::Key::CLEAR, input::Keyboard::KeyCode::CLEAR },
    { Keyboard::Key::RETURN, input::Keyboard::KeyCode::RETURN },

    { Keyboard::Key::ESCAPE, input::Keyboard::KeyCode::ESCAPE },

    // Supported on Linux with Gecko 4.0
    { Keyboard::Key::CONVERT, input::Keyboard::KeyCode::FS },
    { Keyboard::Key::NONCONVERT, input::Keyboard::KeyCode::GS },
    { Keyboard::Key::ACCEPT, input::Keyboard::KeyCode::RS },
    { Keyboard::Key::MODECHANGE, input::Keyboard::KeyCode::US },

    { Keyboard::Key::SPACE, input::Keyboard::KeyCode::SPACE },

    { Keyboard::Key::DEL, input::Keyboard::KeyCode::DEL },
    { Keyboard::Key::_0, input::Keyboard::KeyCode::_0 },
    { Keyboard::Key::_1, input::Keyboard::KeyCode::_1 },
    { Keyboard::Key::_2, input::Keyboard::KeyCode::_2 },
    { Keyboard::Key::_3, input::Keyboard::KeyCode::_3 },
    { Keyboard::Key::_4, input::Keyboard::KeyCode::_4 },
    { Keyboard::Key::_5, input::Keyboard::KeyCode::_5 },
    { Keyboard::Key::_6, input::Keyboard::KeyCode::_6 },
    { Keyboard::Key::_7, input::Keyboard::KeyCode::_7 },
    { Keyboard::Key::_8, input::Keyboard::KeyCode::_8 },
    { Keyboard::Key::_9, input::Keyboard::KeyCode::_9 },
    { Keyboard::Key::COLON, input::Keyboard::KeyCode::COLON },
    { Keyboard::Key::SEMICOLON, input::Keyboard::KeyCode::SEMICOLON },
    { Keyboard::Key::LESS_THAN, input::Keyboard::KeyCode::LESS },
    { Keyboard::Key::EQUALS, input::Keyboard::KeyCode::EQUALS },
    { Keyboard::Key::GREATER_THAN, input::Keyboard::KeyCode::GREATER },
    { Keyboard::Key::QUESTION_MARK, input::Keyboard::KeyCode::QUESTION },
    { Keyboard::Key::AT, input::Keyboard::KeyCode::AT },
    { Keyboard::Key::A, input::Keyboard::KeyCode::A },
    { Keyboard::Key::B, input::Keyboard::KeyCode::B },
    { Keyboard::Key::C, input::Keyboard::KeyCode::C },
    { Keyboard::Key::D, input::Keyboard::KeyCode::D },
    { Keyboard::Key::E, input::Keyboard::KeyCode::E },
    { Keyboard::Key::F, input::Keyboard::KeyCode::F },
    { Keyboard::Key::G, input::Keyboard::KeyCode::G },
    { Keyboard::Key::H, input::Keyboard::KeyCode::H },
    { Keyboard::Key::I, input::Keyboard::KeyCode::I },
    { Keyboard::Key::J, input::Keyboard::KeyCode::J },
    { Keyboard::Key::K, input::Keyboard::KeyCode::K },
    { Keyboard::Key::L, input::Keyboard::KeyCode::L },
    { Keyboard::Key::M, input::Keyboard::KeyCode::M },
    { Keyboard::Key::N, input::Keyboard::KeyCode::N },
    { Keyboard::Key::O, input::Keyboard::KeyCode::O },
    { Keyboard::Key::P, input::Keyboard::KeyCode::P },
    { Keyboard::Key::Q, input::Keyboard::KeyCode::Q },
    { Keyboard::Key::R, input::Keyboard::KeyCode::R },
    { Keyboard::Key::S, input::Keyboard::KeyCode::S },
    { Keyboard::Key::T, input::Keyboard::KeyCode::T },
    { Keyboard::Key::U, input::Keyboard::KeyCode::U },
    { Keyboard::Key::V, input::Keyboard::KeyCode::V },
    { Keyboard::Key::W, input::Keyboard::KeyCode::W },
    { Keyboard::Key::X, input::Keyboard::KeyCode::X },
    { Keyboard::Key::Y, input::Keyboard::KeyCode::Y },
    { Keyboard::Key::Z, input::Keyboard::KeyCode::Z },

    //{ Key::WIN_OEM_FJ_JISHO, input::Keyboard::KeyCode::WIN_OEM_FJ_JISHO },
    //{ Key::WIN_OEM_FJ_MASSHOU, input::Keyboard::KeyCode::WIN_OEM_FJ_MASSHOU },
    //{ Key::WIN_OEM_FJ_TOUROKU, input::Keyboard::KeyCode::WIN_OEM_FJ_TOUROKU },
    //{ Key::WIN_OEM_FJ_LOYA, input::Keyboard::KeyCode::WIN_OEM_FJ_LOYA },
    //{ Key::WIN_OEM_FJ_ROYA, input::Keyboard::KeyCode::WIN_OEM_FJ_ROYA },

    { Keyboard::Key::CIRCUMFLEX, input::Keyboard::KeyCode::CARET },
    { Keyboard::Key::EXCLAMATION, input::Keyboard::KeyCode::EXCLAIM },
    { Keyboard::Key::DOUBLE_QUOTE, input::Keyboard::KeyCode::QUOTEDBL },
    { Keyboard::Key::HASH, input::Keyboard::KeyCode::HASH },
    { Keyboard::Key::DOLLAR, input::Keyboard::KeyCode::DOLLAR },
    { Keyboard::Key::PERCENT, input::Keyboard::KeyCode::PERCENT },
    { Keyboard::Key::AMPERSAND, input::Keyboard::KeyCode::AMPERSAND },
    { Keyboard::Key::UNDERSCORE, input::Keyboard::KeyCode::UNDERSCORE },
    { Keyboard::Key::OPEN_PAREN, input::Keyboard::KeyCode::LEFTPAREN },
    { Keyboard::Key::CLOSE_PAREN, input::Keyboard::KeyCode::RIGHTPAREN },
    { Keyboard::Key::ASTERISK, input::Keyboard::KeyCode::ASTERISK },
    { Keyboard::Key::PLUS, input::Keyboard::KeyCode::PLUS },
    { Keyboard::Key::PIPE, input::Keyboard::KeyCode::PIPE },
    { Keyboard::Key::HYPHEN_MINUS, input::Keyboard::KeyCode::MINUS },
    { Keyboard::Key::OPEN_CURLY_BRACKET, input::Keyboard::KeyCode::LEFTCURLYBRACKET },
    { Keyboard::Key::CLOSE_CURLY_BRACKET, input::Keyboard::KeyCode::RIGHTCURLYBRACKET },
    { Keyboard::Key::TILDE, input::Keyboard::KeyCode::TILDE },

    { Keyboard::Key::COMMA, input::Keyboard::KeyCode::COMMA },
    { Keyboard::Key::PERIOD, input::Keyboard::KeyCode::PERIOD },
    { Keyboard::Key::SLASH, input::Keyboard::KeyCode::SLASH },
    { Keyboard::Key::BACK_QUOTE, input::Keyboard::KeyCode::BACKQUOTE },
    { Keyboard::Key::OPEN_BRACKET, input::Keyboard::KeyCode::LEFTBRACKET },
    { Keyboard::Key::BACK_SLASH, input::Keyboard::KeyCode::BACKSLASH },
    { Keyboard::Key::CLOSE_BRACKET, input::Keyboard::KeyCode::RIGHTBRACKET },
    { Keyboard::Key::QUOTE, input::Keyboard::KeyCode::QUOTE },

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

const std::unordered_map<Keyboard::Key, Keyboard::ScanCode> KeyMap::keyToScanCodeMap =
{
    { Keyboard::Key::HELP, input::Keyboard::ScanCode::HELP },

    { Keyboard::Key::SCROLL_LOCK, input::Keyboard::ScanCode::SCROLLLOCK },

    { Keyboard::Key::KANA, input::Keyboard::ScanCode::LANG1 },
    //{ Key::HANGUL, input::Keyboard::ScanCode::LANG1 },
    { Keyboard::Key::EISU, input::Keyboard::ScanCode::LANG1 },
    { Keyboard::Key::JUNJA, input::Keyboard::ScanCode::LANG1 },
    { Keyboard::Key::FINAL, input::Keyboard::ScanCode::LANG1 },
    //{ Key::HANJA, input::Keyboard::ScanCode::LANG1},
    { Keyboard::Key::KANJI, input::Keyboard::ScanCode::LANG1 },

    { Keyboard::Key::SHIFT, input::Keyboard::ScanCode::LSHIFT },
    { Keyboard::Key::CONTROL, input::Keyboard::ScanCode::LCTRL },
    { Keyboard::Key::ALT, input::Keyboard::ScanCode::LALT },
    { Keyboard::Key::PAUSE, input::Keyboard::ScanCode::PAUSE },
    { Keyboard::Key::CAPS_LOCK, input::Keyboard::ScanCode::CAPSLOCK },

    { Keyboard::Key::PAGE_UP, input::Keyboard::ScanCode::PAGEUP },
    { Keyboard::Key::PAGE_DOWN, input::Keyboard::ScanCode::PAGEDOWN },
    { Keyboard::Key::END, input::Keyboard::ScanCode::END },
    { Keyboard::Key::HOME, input::Keyboard::ScanCode::HOME },
    { Keyboard::Key::LEFT, input::Keyboard::ScanCode::LEFT },
    { Keyboard::Key::UP, input::Keyboard::ScanCode::UP },
    { Keyboard::Key::RIGHT, input::Keyboard::ScanCode::RIGHT },
    { Keyboard::Key::DOWN, input::Keyboard::ScanCode::DOWN },
    { Keyboard::Key::SELECT, input::Keyboard::ScanCode::SELECT },
    //{ Key::PRINT, input::Keyboard::ScanCode::PRINT },
    { Keyboard::Key::EXECUTE, input::Keyboard::ScanCode::EXECUTE },
    { Keyboard::Key::PRINTSCREEN, input::Keyboard::ScanCode::PRINTSCREEN },
    { Keyboard::Key::INSERT, input::Keyboard::ScanCode::INSERT },

    { Keyboard::Key::META, input::Keyboard::ScanCode::RGUI },
    { Keyboard::Key::ALTGR, input::Keyboard::ScanCode::RALT },
    { Keyboard::Key::WIN, input::Keyboard::ScanCode::LGUI },
    { Keyboard::Key::CONTEXT_MENU, input::Keyboard::ScanCode::APPLICATION },
    { Keyboard::Key::SLEEP, input::Keyboard::ScanCode::SLEEP },
    { Keyboard::Key::NUMPAD0, input::Keyboard::ScanCode::KP_0 },
    { Keyboard::Key::NUMPAD1, input::Keyboard::ScanCode::KP_1 },
    { Keyboard::Key::NUMPAD2, input::Keyboard::ScanCode::KP_2 },
    { Keyboard::Key::NUMPAD3, input::Keyboard::ScanCode::KP_3 },
    { Keyboard::Key::NUMPAD4, input::Keyboard::ScanCode::KP_4 },
    { Keyboard::Key::NUMPAD5, input::Keyboard::ScanCode::KP_5 },
    { Keyboard::Key::NUMPAD6, input::Keyboard::ScanCode::KP_6 },
    { Keyboard::Key::NUMPAD7, input::Keyboard::ScanCode::KP_7 },
    { Keyboard::Key::NUMPAD8, input::Keyboard::ScanCode::KP_8 },
    { Keyboard::Key::NUMPAD9, input::Keyboard::ScanCode::KP_9 },
    { Keyboard::Key::MULTIPLY, input::Keyboard::ScanCode::KP_MULTIPLY },
    { Keyboard::Key::ADD, input::Keyboard::ScanCode::KP_PLUS },
    { Keyboard::Key::SEPARATOR, input::Keyboard::ScanCode::SEPARATOR },
    { Keyboard::Key::SUBTRACT, input::Keyboard::ScanCode::KP_MINUS },
    { Keyboard::Key::DECIMAL, input::Keyboard::ScanCode::KP_DECIMAL },
    { Keyboard::Key::DIVIDE, input::Keyboard::ScanCode::KP_DIVIDE },

    { Keyboard::Key::F1, input::Keyboard::ScanCode::F1 },
    { Keyboard::Key::F2, input::Keyboard::ScanCode::F2 },
    { Keyboard::Key::F3, input::Keyboard::ScanCode::F3 },
    { Keyboard::Key::F4, input::Keyboard::ScanCode::F4 },
    { Keyboard::Key::F5, input::Keyboard::ScanCode::F5 },
    { Keyboard::Key::F6, input::Keyboard::ScanCode::F6 },
    { Keyboard::Key::F7, input::Keyboard::ScanCode::F7 },
    { Keyboard::Key::F8, input::Keyboard::ScanCode::F8 },
    { Keyboard::Key::F9, input::Keyboard::ScanCode::F9 },
    { Keyboard::Key::F10, input::Keyboard::ScanCode::F10 },
    { Keyboard::Key::F11, input::Keyboard::ScanCode::F11 },
    { Keyboard::Key::F12, input::Keyboard::ScanCode::F12 },
    { Keyboard::Key::F13, input::Keyboard::ScanCode::F13 },
    { Keyboard::Key::F14, input::Keyboard::ScanCode::F14 },
    { Keyboard::Key::F15, input::Keyboard::ScanCode::F15 },
    { Keyboard::Key::F16, input::Keyboard::ScanCode::F16 },
    { Keyboard::Key::F17, input::Keyboard::ScanCode::F17 },
    { Keyboard::Key::F18, input::Keyboard::ScanCode::F18 },
    { Keyboard::Key::F19, input::Keyboard::ScanCode::F19 },
    { Keyboard::Key::F20, input::Keyboard::ScanCode::F20 },
    { Keyboard::Key::F21, input::Keyboard::ScanCode::F21 },
    { Keyboard::Key::F22, input::Keyboard::ScanCode::F22 },
    { Keyboard::Key::F23, input::Keyboard::ScanCode::F23 },
    { Keyboard::Key::F24, input::Keyboard::ScanCode::F24 },
    { Keyboard::Key::NUM_LOCK, input::Keyboard::ScanCode::NUMLOCKCLEAR },

    { Keyboard::Key::VOLUME_MUTE, input::Keyboard::ScanCode::MUTE },
    { Keyboard::Key::VOLUME_DOWN, input::Keyboard::ScanCode::VOLUMEDOWN },
    { Keyboard::Key::VOLUME_UP, input::Keyboard::ScanCode::VOLUMEUP },

    // Additional keys
    { Keyboard::Key::CONTROL_RIGHT, input::Keyboard::ScanCode::RCTRL },
    { Keyboard::Key::SHIFT_RIGHT, input::Keyboard::ScanCode::RSHIFT },
};
