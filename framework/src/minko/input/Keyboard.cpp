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
    _up(Signal<Ptr>::create())
{
}
