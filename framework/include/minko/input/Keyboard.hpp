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

#include "minko/Common.hpp"

#include "minko/Signal.hpp"
#include "minko/AbstractCanvas.hpp"

namespace minko
{
    namespace input
    {
        class Keyboard
        {
        public:
            typedef std::shared_ptr<Keyboard>   Ptr;

            typedef enum
            {
                // IDs are the same than the official DOM codes:
                // https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent

                CANCEL = 3, // Cancel key.
                HELP = 6, // Help key.
                BACK_SPACE = 8, // Backspace key.
                TAB = 9, // Tab key.
                CLEAR = 12, // "5" key on Numpad when NumLock is unlocked. Or on Mac, clear key which is positioned at NumLock key.
                RETURN = 13, // Return/enter key on the main keyboard.
                ENTER = 14, // Reserved, but not used.  </code><code>Obsolete since Gecko 30 (Dropped, see bug&nbsp;969247.)
                SHIFT = 16, // Shift key.
                CONTROL = 17, // Control key.
                ALT = 18, // Alt (Option on Mac) key.
                PAUSE = 19, // Pause key.
                CAPS_LOCK = 20, // Caps lock.
                KANA = 21, // Linux support for this keycode was added in Gecko 4.0.
                //HANGUL = 21, // Linux support for this keycode was added in Gecko 4.0.
                EISU = 22, // "英数" key on Japanese Mac keyboard.
                JUNJA = 23, // Linux support for this keycode was added in Gecko 4.0.
                FINAL = 24, // Linux support for this keycode was added in Gecko 4.0.
                //HANJA = 25, // Linux support for this keycode was added in Gecko 4.0.
                KANJI = 25, // Linux support for this keycode was added in Gecko 4.0.
                ESCAPE = 27, // Escape key.
                CONVERT = 28, // Linux support for this keycode was added in Gecko 4.0.
                NONCONVERT = 29, // Linux support for this keycode was added in Gecko 4.0.
                ACCEPT = 30, // Linux support for this keycode was added in Gecko 4.0.
                MODECHANGE = 31, // Linux support for this keycode was added in Gecko 4.0.
                SPACE = 32, // Space bar.
                PAGE_UP = 33, // Page Up key.
                PAGE_DOWN = 34, // Page Down key.
                END = 35, // End key.
                HOME = 36, // Home key.
                LEFT = 37, // Left arrow.
                UP = 38, // Up arrow.
                RIGHT = 39, // Right arrow.
                DOWN = 40, // Down arrow.
                SELECT = 41, // Linux support for this keycode was added in Gecko 4.0.
                PRINT = 42, // Linux support for this keycode was added in Gecko 4.0.
                EXECUTE = 43, // Linux support for this keycode was added in Gecko 4.0.
                PRINTSCREEN = 44, // Print Screen key.
                INSERT = 45, // Ins(ert) key.
                DEL = 46, // Del(ete) key.
                _0 = 48, // "0" key in standard key location.
                _1 = 49, // "1" key in standard key location.
                _2 = 50, // "2" key in standard key location.
                _3 = 51, // "3" key in standard key location.
                _4 = 52, // "4" key in standard key location.
                _5 = 53, // "5" key in standard key location.
                _6 = 54, // "6" key in standard key location.
                _7 = 55, // "7" key in standard key location.
                _8 = 56, // "8" key in standard key location.
                _9 = 57, // "9" key in standard key location.
                COLON = 58, // Colon (":") key.
                SEMICOLON = 59, // Semicolon (";") key.
                LESS_THAN = 60, // Less-than ("&lt;") key.
                EQUALS = 61, // Equals ("=") key.
                GREATER_THAN = 62, // Greater-than ("&gt;") key.
                QUESTION_MARK = 63, // Question mark ("?") key.
                AT = 64, // Atmark ("@") key.
                A = 65, // "A" key.
                B = 66, // "B" key.
                C = 67, // "C" key.
                D = 68, // "D" key.
                E = 69, // "E" key.
                F = 70, // "F" key.
                G = 71, // "G" key.
                H = 72, // "H" key.
                I = 73, // "I" key.
                J = 74, // "J" key.
                K = 75, // "K" key.
                L = 76, // "L" key.
                M = 77, // "M" key.
                N = 78, // "N" key.
                O = 79, // "O" key.
                P = 80, // "P" key.
                Q = 81, // "Q" key.
                R = 82, // "R" key.
                S = 83, // "S" key.
                T = 84, // "T" key.
                U = 85, // "U" key.
                V = 86, // "V" key.
                W = 87, // "W" key.
                X = 88, // "X" key.
                Y = 89, // "Y" key.
                Z = 90, // "Z" key.
                WIN = 91, // Windows logo key on Windows. Or Super or Hyper key on Linux.
                CONTEXT_MENU = 93, // Opening context menu key.
                SLEEP = 95, // Linux support for this keycode was added in Gecko 4.0.
                NUMPAD0 = 96, // "0" on the numeric keypad.
                NUMPAD1 = 97, // "1" on the numeric keypad.
                NUMPAD2 = 98, // "2" on the numeric keypad.
                NUMPAD3 = 99, // "3" on the numeric keypad.
                NUMPAD4 = 100, // "4" on the numeric keypad.
                NUMPAD5 = 101, // "5" on the numeric keypad.
                NUMPAD6 = 102, // "6" on the numeric keypad.
                NUMPAD7 = 103, // "7" on the numeric keypad.
                NUMPAD8 = 104, // "8" on the numeric keypad.
                NUMPAD9 = 105, // "9" on the numeric keypad.
                MULTIPLY = 106, // "*" on the numeric keypad.
                ADD = 107, // "+" on the numeric keypad.
                SEPARATOR = 108, // &nbsp;
                SUBTRACT = 109, // "-" on the numeric keypad.
                DECIMAL = 110, // Decimal point on the numeric keypad.
                DIVIDE = 111, // "/" on the numeric keypad.
                F1 = 112, // F1 key.
                F2 = 113, // F2 key.
                F3 = 114, // F3 key.
                F4 = 115, // F4 key.
                F5 = 116, // F5 key.
                F6 = 117, // F6 key.
                F7 = 118, // F7 key.
                F8 = 119, // F8 key.
                F9 = 120, // F9 key.
                F10 = 121, // F10 key.
                F11 = 122, // F11 key.
                F12 = 123, // F12 key.
                F13 = 124, // F13 key.
                F14 = 125, // F14 key.
                F15 = 126, // F15 key.
                F16 = 127, // F16 key.
                F17 = 128, // F17 key.
                F18 = 129, // F18 key.
                F19 = 130, // F19 key.
                F20 = 131, // F20 key.
                F21 = 132, // F21 key.
                F22 = 133, // F22 key.
                F23 = 134, // F23 key.
                F24 = 135, // F24 key.
                NUM_LOCK = 144, // Num Lock key.
                SCROLL_LOCK = 145, // Scroll Lock key.
                WIN_OEM_FJ_JISHO = 146, // An OEM specific key on Windows. This was used for "Dictionary" key on Fujitsu OASYS.
                WIN_OEM_FJ_MASSHOU = 147, // An OEM specific key on Windows. This was used for "Unregister word" key on Fujitsu OASYS.
                WIN_OEM_FJ_TOUROKU = 148, // An OEM specific key on Windows. This was used for "Register word" key on Fujitsu OASYS.
                WIN_OEM_FJ_LOYA = 149, // An OEM specific key on Windows. This was used for "Left OYAYUBI" key on Fujitsu OASYS.
                WIN_OEM_FJ_ROYA = 150, // An OEM specific key on Windows. This was used for "Right OYAYUBI" key on Fujitsu OASYS.
                CIRCUMFLEX = 160, // Circumflex ("^") key.
                EXCLAMATION = 161, // Exclamation ("!") key.
                DOUBLE_QUOTE = 162, // Double quote (""") key.
                HASH = 163, // Hash ("#") key.
                DOLLAR = 164, // Dollar sign ("$") key.
                PERCENT = 165, // Percent ("%") key.
                AMPERSAND = 166, // Ampersand ("&amp;") key.
                UNDERSCORE = 167, // Underscore ("_") key.
                OPEN_PAREN = 168, // Open parenthesis ("(") key.
                CLOSE_PAREN = 169, // Close parenthesis (")") key.
                ASTERISK = 170, // Asterisk ("*") key.
                PLUS = 171, // Plus ("+") key.
                PIPE = 172, // Pipe ("|") key.
                HYPHEN_MINUS = 173, // Hyphen-US/docs/Minus ("-") key.
                OPEN_CURLY_BRACKET = 174, // Open curly bracket ("{") key.
                CLOSE_CURLY_BRACKET = 175, // Close curly bracket ("}") key.
                TILDE = 176, // Tilde ("~") key.
                VOLUME_MUTE = 181, // Audio mute key.
                VOLUME_DOWN = 182, // Audio volume down key
                VOLUME_UP = 183, // Audio volume up key
                COMMA = 188, // Comma (",") key.
                PERIOD = 190, // Period (".") key.
                SLASH = 191, // Slash ("/") key.
                BACK_QUOTE = 192, // Back tick ("`") key.
                OPEN_BRACKET = 219, // Open square bracket ("[") key.
                BACK_SLASH = 220, // Back slash ("\") key.
                CLOSE_BRACKET = 221, // Close square bracket ("]") key.
                QUOTE = 222, // Quote (''') key.
                META = 224, // Meta key on Linux, Command key on Mac.
                ALTGR = 225, // AltGr key (Level 3 Shift key or Level 5 Shift key) on Linux.
                WIN_ICO_HELP = 227, // An OEM specific key on Windows. This is (was?) used for Olivetti ICO keyboard.
                WIN_ICO_00 = 228, // An OEM specific key on Windows. This is (was?) used for Olivetti ICO keyboard.
                WIN_ICO_CLEAR = 230, // An OEM specific key on Windows. This is (was?) used for Olivetti ICO keyboard.
                WIN_OEM_RESET = 233, // An OEM specific key on Windows. This was used for Nokia/Ericsson's device.
                WIN_OEM_JUMP = 234, // An OEM specific key on Windows. This was used for Nokia/Ericsson's device.
                WIN_OEM_PA1 = 235, // An OEM specific key on Windows. This was used for Nokia/Ericsson's device.
                WIN_OEM_PA2 = 236, // An OEM specific key on Windows. This was used for Nokia/Ericsson's device.
                WIN_OEM_PA3 = 237, // An OEM specific key on Windows. This was used for Nokia/Ericsson's device.
                WIN_OEM_WSCTRL = 238, // An OEM specific key on Windows. This was used for Nokia/Ericsson's device.
                WIN_OEM_CUSEL = 239, // An OEM specific key on Windows. This was used for Nokia/Ericsson's device.
                WIN_OEM_ATTN = 240, // An OEM specific key on Windows. This was used for Nokia/Ericsson's device.
                WIN_OEM_FINISH = 241, // An OEM specific key on Windows. This was used for Nokia/Ericsson's device.
                WIN_OEM_COPY = 242, // An OEM specific key on Windows. This was used for Nokia/Ericsson's device.
                WIN_OEM_AUTO = 243, // An OEM specific key on Windows. This was used for Nokia/Ericsson's device.
                WIN_OEM_ENLW = 244, // An OEM specific key on Windows. This was used for Nokia/Ericsson's device.
                WIN_OEM_BACKTAB = 245, // An OEM specific key on Windows. This was used for Nokia/Ericsson's device.
                ATTN = 246, // Attn (Attension) key of IBM midrange computers, e.g., AS/400.
                CRSEL = 247, // CrSel (Cursor Selection) key of IBM 3270 keyboard layout.
                EXSEL = 248, // ExSel (Extend Selection) key of IBM 3270 keyboard layout.
                EREOF = 249, // Erase EOF key of IBM 3270 keyboard layout.
                PLAY = 250, // Play key of IBM 3270 keyboard layout.
                ZOOM = 251, // Zoom key.
                PA1 = 253, // PA1 key of IBM 3270 keyboard layout.
                WIN_OEM_CLEAR = 254, // Clear key, but we're not sure the meaning difference from DOM_VK_CLEAR.

                // Additional keys (specific to Minko and for native support of some keys)
                CONTROL_RIGHT = 300, // Right control key
                SHIFT_RIGHT = 301, // Right shift key

            } Key;

        //protected:
        public:
            enum class ScanCode
            {
                UNKNOWN = 0,

                A = 4,
                B = 5,
                C = 6,
                D = 7,
                E = 8,
                F = 9,
                G = 10,
                H = 11,
                I = 12,
                J = 13,
                K = 14,
                L = 15,
                M = 16,
                N = 17,
                O = 18,
                P = 19,
                Q = 20,
                R = 21,
                S = 22,
                T = 23,
                U = 24,
                V = 25,
                W = 26,
                X = 27,
                Y = 28,
                Z = 29,

                _1 = 30,
                _2 = 31,
                _3 = 32,
                _4 = 33,
                _5 = 34,
                _6 = 35,
                _7 = 36,
                _8 = 37,
                _9 = 38,
                _0 = 39,

                RETURN = 40,
                ESCAPE = 41,
                BACKSPACE = 42,
                TAB = 43,
                SPACE = 44,

                MINUS = 45,
                EQUALS = 46,
                LEFTBRACKET = 47,
                RIGHTBRACKET = 48,
                BACKSLASH = 49,
                NONUSHASH = 50,
                SEMICOLON = 51,
                APOSTROPHE = 52,
                GRAVE = 53,
                COMMA = 54,
                PERIOD = 55,
                SLASH = 56,

                CAPSLOCK = 57,

                F1 = 58,
                F2 = 59,
                F3 = 60,
                F4 = 61,
                F5 = 62,
                F6 = 63,
                F7 = 64,
                F8 = 65,
                F9 = 66,
                F10 = 67,
                F11 = 68,
                F12 = 69,

                PRINTSCREEN = 70,
                SCROLLLOCK = 71,
                PAUSE = 72,
                INSERT = 73,
                HOME = 74,
                PAGEUP = 75,
                DEL = 76,
                END = 77,
                PAGEDOWN = 78,
                RIGHT = 79,
                LEFT = 80,
                DOWN = 81,
                UP = 82,
                NUMLOCKCLEAR = 83,
                KP_DIVIDE = 84,
                KP_MULTIPLY = 85,
                KP_MINUS = 86,
                KP_PLUS = 87,
                KP_ENTER = 88,
                KP_1 = 89,
                KP_2 = 90,
                KP_3 = 91,
                KP_4 = 92,
                KP_5 = 93,
                KP_6 = 94,
                KP_7 = 95,
                KP_8 = 96,
                KP_9 = 97,
                KP_0 = 98,
                KP_PERIOD = 99,
                NONUSBACKSLASH = 100,
                APPLICATION = 101,
                POWER = 102,
                KP_EQUALS = 103,
                F13 = 104,
                F14 = 105,
                F15 = 106,
                F16 = 107,
                F17 = 108,
                F18 = 109,
                F19 = 110,
                F20 = 111,
                F21 = 112,
                F22 = 113,
                F23 = 114,
                F24 = 115,
                EXECUTE = 116,
                HELP = 117,
                MENU = 118,
                SELECT = 119,
                STOP = 120,
                AGAIN = 121,
                UNDO = 122,
                CUT = 123,
                COPY = 124,
                PASTE = 125,
                FIND = 126,
                MUTE = 127,
                VOLUMEUP = 128,
                VOLUMEDOWN = 129,
                KP_COMMA = 133,
                KP_EQUALSAS400 = 134,
                INTERNATIONAL1 = 135,
                INTERNATIONAL2 = 136,
                INTERNATIONAL3 = 137,
                INTERNATIONAL4 = 138,
                INTERNATIONAL5 = 139,
                INTERNATIONAL6 = 140,
                INTERNATIONAL7 = 141,
                INTERNATIONAL8 = 142,
                INTERNATIONAL9 = 143,
                LANG1 = 144,
                LANG2 = 145,
                LANG3 = 146,
                LANG4 = 147,
                LANG5 = 148,
                LANG6 = 149,
                LANG7 = 150,
                LANG8 = 151,
                LANG9 = 152,
                ALTERASE = 153,
                SYSREQ = 154,
                CANCEL = 155,
                CLEAR = 156,
                PRIOR = 157,
                RETURN2 = 158,
                SEPARATOR = 159,
                //OUT = 160,
                OPER = 161,
                CLEARAGAIN = 162,
                CRSEL = 163,
                EXSEL = 164,
                KP_00 = 176,
                KP_000 = 177,
                THOUSANDSSEPARATOR = 178,
                DECIMALSEPARATOR = 179,
                CURRENCYUNIT = 180,
                CURRENCYSUBUNIT = 181,
                KP_LEFTPAREN = 182,
                KP_RIGHTPAREN = 183,
                KP_LEFTBRACE = 184,
                KP_RIGHTBRACE = 185,
                KP_TAB = 186,
                KP_BACKSPACE = 187,
                KP_A = 188,
                KP_B = 189,
                KP_C = 190,
                KP_D = 191,
                KP_E = 192,
                KP_F = 193,
                KP_XOR = 194,
                KP_POWER = 195,
                KP_PERCENT = 196,
                KP_LESS = 197,
                KP_GREATER = 198,
                KP_AMPERSAND = 199,
                KP_DBLAMPERSAND = 200,
                KP_VERTICALBAR = 201,
                KP_DBLVERTICALBAR = 202,
                KP_COLON = 203,
                KP_HASH = 204,
                KP_SPACE = 205,
                KP_AT = 206,
                KP_EXCLAM = 207,
                KP_MEMSTORE = 208,
                KP_MEMRECALL = 209,
                KP_MEMCLEAR = 210,
                KP_MEMADD = 211,
                KP_MEMSUBTRACT = 212,
                KP_MEMMULTIPLY = 213,
                KP_MEMDIVIDE = 214,
                KP_PLUSMINUS = 215,
                KP_CLEAR = 216,
                KP_CLEARENTRY = 217,
                KP_BINARY = 218,
                KP_OCTAL = 219,
                KP_DECIMAL = 220,
                KP_HEXADECIMAL = 221,

                LCTRL = 224,
                LSHIFT = 225,
                LALT = 226,
                LGUI = 227,
                RCTRL = 228,
                RSHIFT = 229,
                RALT = 230,
                RGUI = 231,
                MODE = 257,

                AUDIONEXT = 258,
                AUDIOPREV = 259,
                AUDIOSTOP = 260,
                AUDIOPLAY = 261,
                AUDIOMUTE = 262,
                MEDIASELECT = 263,
                WWW = 264,
                MAIL = 265,
                CALCULATOR = 266,
                COMPUTER = 267,
                AC_SEARCH = 268,
                AC_HOME = 269,
                AC_BACK = 270,
                AC_FORWARD = 271,
                AC_STOP = 272,
                AC_REFRESH = 273,
                AC_BOOKMARKS = 274,

                BRIGHTNESSDOWN = 275,
                BRIGHTNESSUP = 276,
                DISPLAYSWITCH = 277,
                KBDILLUMTOGGLE = 278,
                KBDILLUMDOWN = 279,
                KBDILLUMUP = 280,
                EJECT = 281,
                SLEEP = 282,

                APP1 = 283,
                APP2 = 284
            };

            enum class KeyCode
            {
                UNKNOWN = 0,
                FIRST = 0,
                BACKSPACE = 8,
                TAB = 9,
                CLEAR = 12,
                RETURN = 13,
                PAUSE = 19,
                CANCEL = 24,
                ESCAPE = 27,
                FS = 28, // File separator
                GS = 29, // Group separator
                RS = 30, // Record separator
                US = 31, // Unit separator
                SPACE = 32,
                EXCLAIM = 33,
                QUOTEDBL = 34,
                HASH = 35,
                DOLLAR = 36,
                PERCENT = 37,
                AMPERSAND = 38,
                QUOTE = 39,
                LEFTPAREN = 40,
                RIGHTPAREN = 41,
                ASTERISK = 42,
                PLUS = 43,
                COMMA = 44,
                MINUS = 45,
                PERIOD = 46,
                SLASH = 47,
                _0 = 48,
                _1 = 49,
                _2 = 50,
                _3 = 51,
                _4 = 52,
                _5 = 53,
                _6 = 54,
                _7 = 55,
                _8 = 56,
                _9 = 57,
                COLON = 58,
                SEMICOLON = 59,
                LESS = 60,
                EQUALS = 61,
                GREATER = 62,
                QUESTION = 63,
                AT = 64,
                // 65 -> 90 = capital letters
                LEFTBRACKET = 91,
                BACKSLASH = 92,
                RIGHTBRACKET = 93,
                CARET = 94,
                UNDERSCORE = 95,
                BACKQUOTE = 96,
                A = 97,
                B = 98,
                C = 99,
                D = 100,
                E = 101,
                F = 102,
                G = 103,
                H = 104,
                I = 105,
                J = 106,
                K = 107,
                L = 108,
                M = 109,
                N = 110,
                O = 111,
                P = 112,
                Q = 113,
                R = 114,
                S = 115,
                T = 116,
                U = 117,
                V = 118,
                W = 119,
                X = 120,
                Y = 121,
                Z = 122,
                LEFTCURLYBRACKET = 123,
                PIPE = 124,
                RIGHTCURLYBRACKET = 125,
                TILDE = 126,
                DEL = 127,
                WORLD_0 = 160,
                WORLD_1 = 161,
                WORLD_2 = 162,
                WORLD_3 = 163,
                WORLD_4 = 164,
                WORLD_5 = 165,
                WORLD_6 = 166,
                WORLD_7 = 167,
                WORLD_8 = 168,
                WORLD_9 = 169,
                WORLD_10 = 170,
                WORLD_11 = 171,
                WORLD_12 = 172,
                WORLD_13 = 173,
                WORLD_14 = 174,
                WORLD_15 = 175,
                WORLD_16 = 176,
                WORLD_17 = 177,
                WORLD_18 = 178,
                WORLD_19 = 179,
                WORLD_20 = 180,
                WORLD_21 = 181,
                WORLD_22 = 182,
                WORLD_23 = 183,
                WORLD_24 = 184,
                WORLD_25 = 185,
                WORLD_26 = 186,
                WORLD_27 = 187,
                WORLD_28 = 188,
                WORLD_29 = 189,
                WORLD_30 = 190,
                WORLD_31 = 191,
                WORLD_32 = 192,
                WORLD_33 = 193,
                WORLD_34 = 194,
                WORLD_35 = 195,
                WORLD_36 = 196,
                WORLD_37 = 197,
                WORLD_38 = 198,
                WORLD_39 = 199,
                WORLD_40 = 200,
                WORLD_41 = 201,
                WORLD_42 = 202,
                WORLD_43 = 203,
                WORLD_44 = 204,
                WORLD_45 = 205,
                WORLD_46 = 206,
                WORLD_47 = 207,
                WORLD_48 = 208,
                WORLD_49 = 209,
                WORLD_50 = 210,
                WORLD_51 = 211,
                WORLD_52 = 212,
                WORLD_53 = 213,
                WORLD_54 = 214,
                WORLD_55 = 215,
                WORLD_56 = 216,
                WORLD_57 = 217,
                WORLD_58 = 218,
                WORLD_59 = 219,
                WORLD_60 = 220,
                WORLD_61 = 221,
                WORLD_62 = 222,
                WORLD_63 = 223,
                WORLD_64 = 224,
                WORLD_65 = 225,
                WORLD_66 = 226,
                WORLD_67 = 227,
                WORLD_68 = 228,
                WORLD_69 = 229,
                WORLD_70 = 230,
                WORLD_71 = 231,
                WORLD_72 = 232,
                WORLD_73 = 233,
                WORLD_74 = 234,
                WORLD_75 = 235,
                WORLD_76 = 236,
                WORLD_77 = 237,
                WORLD_78 = 238,
                WORLD_79 = 239,
                WORLD_80 = 240,
                WORLD_81 = 241,
                WORLD_82 = 242,
                WORLD_83 = 243,
                WORLD_84 = 244,
                WORLD_85 = 245,
                WORLD_86 = 246,
                WORLD_87 = 247,
                WORLD_88 = 248,
                WORLD_89 = 249,
                WORLD_90 = 250,
                WORLD_91 = 251,
                WORLD_92 = 252,
                WORLD_93 = 253,
                WORLD_94 = 254,
                WORLD_95 = 255,
            };

        public:
            static const uint NUM_KEYS = 350;

        private:

            typedef std::array<std::string, NUM_KEYS>           KeyToNameArray;

            static const KeyToNameArray                         _keyToName;

        protected:
            std::unordered_map<uint, Signal<Ptr, uint>::Ptr>    _keyDown;
            std::unordered_map<uint, Signal<Ptr, uint>::Ptr>    _keyUp;

            Signal<Ptr>::Ptr                                    _down;
            Signal<Ptr>::Ptr                                    _up;

            Signal<Ptr, char16_t>::Ptr                          _textInput;

        public:
            inline static
            const std::string&
            getKeyName(Key key)
            {
                return _keyToName[static_cast<int>(key)];
            }

            inline static
            const std::string&
            getKeyName(uint key)
            {
                return _keyToName[key];
            }

            virtual
            Signal<Ptr>::Ptr
            keyDown() const
            {
                return _down;
            }

            virtual
            Signal<Ptr, char16_t>::Ptr
            textInput() const
            {
                return _textInput;
            }

            virtual
            Signal<Ptr, uint>::Ptr
            keyDown(Key key)
            {
                auto index = static_cast<int>(key);

                if (_keyDown.count(index) == 0)
                    _keyDown[index] = Signal<Ptr, uint>::create();

                return _keyDown[index];
            }

            virtual
            Signal<Ptr>::Ptr
            keyUp() const
            {
                return _up;
            }

            Signal<Ptr, uint>::Ptr
            keyUp(Key key)
            {
                auto index = static_cast<int>(key);

                if (_keyUp.count(index) == 0)
                    _keyUp[index] = Signal<Ptr, uint>::create();

                return _keyUp[index];
            }

            virtual
            bool
            keyIsDown(Key key) = 0;

            virtual
            ~Keyboard()
            {
            }

        protected:
            Keyboard();

        private:
            static
            const KeyToNameArray
            initializeKeyNames();
        };
    }
}

namespace std
{
    template <>
    class hash<minko::input::Keyboard::Key>
    {
    public:
        size_t operator()(const minko::input::Keyboard::Key &key) const{
            return hash<int>()(key);
        }
    };
}

namespace minko
{
    namespace input
    {
        class KeyMap
        {
        public:
            static const std::unordered_map<Keyboard::Key, Keyboard::KeyCode> keyToKeyCodeMap;
            static const std::unordered_map<Keyboard::Key, Keyboard::ScanCode> keyToScanCodeMap;
        };
    }
}
