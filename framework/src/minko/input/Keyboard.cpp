/*
Copyright (c) 2013 Aerys

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

const Keyboard::ScanCodeToNameArray Keyboard::_scanCodeToName = initializeKeyNames();

const Keyboard::ScanCodeToNameArray
Keyboard::initializeKeyNames()
{
	ScanCodeToNameArray names;

	names[0] = "UNKNOWN";
	names[4] = "A";
	names[5] = "B";
	names[6] = "C";
	names[7] = "D";
	names[8] = "E";
	names[9] = "F";
	names[10] = "G";
	names[11] = "H";
	names[12] = "I";
	names[13] = "J";
	names[14] = "K";
	names[15] = "L";
	names[16] = "M";
	names[17] = "N";
	names[18] = "O";
	names[19] = "P";
	names[20] = "Q";
	names[21] = "R";
	names[22] = "S";
	names[23] = "T";
	names[24] = "U";
	names[25] = "V";
	names[26] = "W";
	names[27] = "X";
	names[28] = "Y";
	names[29] = "Z";
	names[30] = "_1";
	names[31] = "_2";
	names[32] = "_3";
	names[33] = "_4";
	names[34] = "_5";
	names[35] = "_6";
	names[36] = "_7";
	names[37] = "_8";
	names[38] = "_9";
	names[39] = "_0";
	names[40] = "RETURN";
	names[41] = "ESCAPE";
	names[42] = "BACKSPACE";
	names[43] = "TAB";
	names[44] = "SPACE";
	names[45] = "MINUS";
	names[46] = "EQUALS";
	names[47] = "LEFTBRACKET";
	names[48] = "RIGHTBRACKET";
	names[49] = "BACKSLASH"; 
	names[50] = "NONUSHASH"; 
	names[51] = "SEMICOLON";
	names[52] = "APOSTROPHE";
	names[53] = "GRAVE"; 
	names[54] = "COMMA";
	names[55] = "PERIOD";
	names[56] = "SLASH";
	names[57] = "CAPSLOCK";
	names[58] = "F1";
	names[59] = "F2";
	names[60] = "F3";
	names[61] = "F4";
	names[62] = "F5";
	names[63] = "F6";
	names[64] = "F7";
	names[65] = "F8";
	names[66] = "F9";
	names[67] = "F10";
	names[68] = "F11";
	names[69] = "F12";
	names[70] = "PRINTSCREEN";
	names[71] = "SCROLLLOCK";
	names[72] = "PAUSE";
	names[73] = "INSERT"; 
	names[74] = "HOME";
	names[75] = "PAGEUP";
	names[76] = "DELETE";
	names[77] = "END";
	names[78] = "PAGEDOWN";
	names[79] = "RIGHT";
	names[80] = "LEFT";
	names[81] = "DOWN";
	names[82] = "UP";
	names[83] = "NUMLOCKCLEAR"; 
	names[84] = "KP_DIVIDE";
	names[85] = "KP_MULTIPLY";
	names[86] = "KP_MINUS";
	names[87] = "KP_PLUS";
	names[88] = "KP_ENTER";
	names[89] = "KP_1";
	names[90] = "KP_2";
	names[91] = "KP_3";
	names[92] = "KP_4";
	names[93] = "KP_5";
	names[94] = "KP_6";
	names[95] = "KP_7";
	names[96] = "KP_8";
	names[97] = "KP_9";
	names[98] = "KP_0";
	names[99] = "KP_PERIOD";
	names[100] = "NONUSBACKSLASH"; 
	names[101] = "APPLICATION"; 
	names[102] = "POWER"; 
	names[103] = "KP_EQUALS";
	names[104] = "F13";
	names[105] = "F14";
	names[106] = "F15";
	names[107] = "F16";
	names[108] = "F17";
	names[109] = "F18";
	names[110] = "F19";
	names[111] = "F20";
	names[112] = "F21";
	names[113] = "F22";
	names[114] = "F23";
	names[115] = "F24";
	names[116] = "EXECUTE";
	names[117] = "HELP";
	names[118] = "MENU";
	names[119] = "SELECT";
	names[120] = "STOP";
	names[121] = "AGAIN";
	names[122] = "UNDO";
	names[123] = "CUT";
	names[124] = "COPY";
	names[125] = "PASTE";
	names[126] = "FIND";
	names[127] = "MUTE";
	names[128] = "VOLUMEUP";
	names[129] = "VOLUMEDOWN";
	names[133] = "KP_COMMA";
	names[134] = "KP_EQUALSAS400";
	names[135] = "INTERNATIONAL1"; 
	names[136] = "INTERNATIONAL2";
	names[137] = "INTERNATIONAL3";
	names[138] = "INTERNATIONAL4";
	names[139] = "INTERNATIONAL5";
	names[140] = "INTERNATIONAL6";
	names[141] = "INTERNATIONAL7";
	names[142] = "INTERNATIONAL8";
	names[143] = "INTERNATIONAL9";
	names[144] = "LANG1";
	names[145] = "LANG2";
	names[146] = "LANG3";
	names[147] = "LANG4";
	names[148] = "LANG5";
	names[149] = "LANG6";
	names[150] = "LANG7";
	names[151] = "LANG8";
	names[152] = "LANG9";
	names[153] = "ALTERASE";
	names[154] = "SYSREQ";
	names[155] = "CANCEL";
	names[156] = "CLEAR";
	names[157] = "PRIOR";
	names[158] = "RETURN2";
	names[159] = "SEPARATOR";
	names[160] = "OUT";
	names[161] = "OPER";
	names[162] = "CLEARAGAIN";
	names[163] = "CRSEL";
	names[164] = "EXSEL";
	names[176] = "KP_00";
	names[177] = "KP_000";
	names[178] = "THOUSANDSSEPARATOR";
	names[179] = "DECIMALSEPARATOR";
	names[180] = "CURRENCYUNIT";
	names[181] = "CURRENCYSUBUNIT";
	names[182] = "KP_LEFTPAREN";
	names[183] = "KP_RIGHTPAREN";
	names[184] = "KP_LEFTBRACE";
	names[185] = "KP_RIGHTBRACE";
	names[186] = "KP_TAB";
	names[187] = "KP_BACKSPACE";
	names[188] = "KP_A";
	names[189] = "KP_B";
	names[190] = "KP_C";
	names[191] = "KP_D";
	names[192] = "KP_E";
	names[193] = "KP_F";
	names[194] = "KP_XOR";
	names[195] = "KP_POWER";
	names[196] = "KP_PERCENT";
	names[197] = "KP_LESS";
	names[198] = "KP_GREATER";
	names[199] = "KP_AMPERSAND";
	names[200] = "KP_DBLAMPERSAND";
	names[201] = "KP_VERTICALBAR";
	names[202] = "KP_DBLVERTICALBAR";
	names[203] = "KP_COLON";
	names[204] = "KP_HASH";
	names[205] = "KP_SPACE";
	names[206] = "KP_AT";
	names[207] = "KP_EXCLAM";
	names[208] = "KP_MEMSTORE";
	names[209] = "KP_MEMRECALL";
	names[210] = "KP_MEMCLEAR";
	names[211] = "KP_MEMADD";
	names[212] = "KP_MEMSUBTRACT";
	names[213] = "KP_MEMMULTIPLY";
	names[214] = "KP_MEMDIVIDE";
	names[215] = "KP_PLUSMINUS";
	names[216] = "KP_CLEAR";
	names[217] = "KP_CLEARENTRY";
	names[218] = "KP_BINARY";
	names[219] = "KP_OCTAL";
	names[220] = "KP_DECIMAL";
	names[221] = "KP_HEXADECIMAL";
	names[224] = "LCTRL";
	names[225] = "LSHIFT";
	names[226] = "LALT";
	names[227] = "LGUI";
	names[228] = "RCTRL";
	names[229] = "RSHIFT";
	names[230] = "RALT";
	names[231] = "RGUI";
	names[257] = "MODE";
	names[258] = "AUDIONEXT";
	names[259] = "AUDIOPREV";
	names[260] = "AUDIOSTOP";
	names[261] = "AUDIOPLAY";
	names[262] = "AUDIOMUTE";
	names[263] = "MEDIASELECT";
	names[264] = "WWW";
	names[265] = "MAIL";
	names[266] = "CALCULATOR";
	names[267] = "COMPUTER";
	names[268] = "AC_SEARCH";
	names[269] = "AC_HOME";
	names[270] = "AC_BACK";
	names[271] = "AC_FORWARD";
	names[272] = "AC_STOP";
	names[273] = "AC_REFRESH";
	names[274] = "AC_BOOKMARKS";
	names[275] = "BRIGHTNESSDOWN";
	names[276] = "BRIGHTNESSUP";
	names[277] = "DISPLAYSWITCH"; 
	names[278] = "KBDILLUMTOGGLE";
	names[279] = "KBDILLUMDOWN";
	names[280] = "KBDILLUMUP";
	names[281] = "EJECT";
	names[282] = "SLEEP";
	names[283] = "APP1";
	names[284] = "APP2";

	return names;
}

Keyboard::Keyboard() : 
	_down(Signal<Ptr>::create()),
	_up(Signal<Ptr>::create())
{

}
