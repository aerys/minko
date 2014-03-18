/*
Copyright (c) 2013 Aerys

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

            enum
            {
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
            };

            enum class ScanCode
            {
                UNKNOWN = 0,

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

                SCROLLLOCK = 71,
                PAUSE = 72,
                INSERT = 73,
                HOME = 74,
                PAGEUP = 75,
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
                OUT = 160,
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
                ESCAPE = 27,
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
                LEFTBRACKET = 91,
                BACKSLASH = 92,
                RIGHTBRACKET = 93,
                CARET = 94,
                UNDERSCORE = 95,
                BACKQUOTE = 96,
                a = 97,
                b = 98,
                c = 99,
                d = 100,
                e = 101,
                f = 102,
                g = 103,
                h = 104,
                i = 105,
                j = 106,
                k = 107,
                l = 108,
                m = 109,
                n = 110,
                o = 111,
                p = 112,
                q = 113,
                r = 114,
                s = 115,
                t = 116,
                u = 117,
                v = 118,
                w = 119,
                x = 120,
                y = 121,
                z = 122,

                LEFTCURLYBRACKET = 123,
                PIPE = 124,
                RIGHTCURLYBRACKET = 125,
                TILDE = 126,
                DELETE = 127,

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

                MODE = 313,
                COMPOSE = 314,
                HELP = 315,
                PRINT = 316,
                SYSREQ = 317,
                BREAK = 318,
                MENU = 319,
                POWER = 320,
                EURO = 321,
                UNDO = 322
            };

        public:
            static const uint NUM_SCANCODES = 285;

        private:
            typedef std::array<std::string, NUM_SCANCODES>      ScanCodeToNameArray;

        private:
            static const ScanCodeToNameArray                    _scanCodeToName;

        protected:
            std::unordered_map<uint, Signal<Ptr, uint>::Ptr>    _keyDown;
            std::unordered_map<uint, Signal<Ptr, uint>::Ptr>    _keyUp;

            Signal<Ptr>::Ptr                                    _down;
            Signal<Ptr>::Ptr                                    _up;

        public:
            inline static
            const std::string&
            getKeyName(ScanCode scanCode)
            {
                return _scanCodeToName[static_cast<int>(scanCode)];
            }

            inline static
            const std::string&
            getKeyName(uint scanCode)
            {
                return _scanCodeToName[scanCode];
            }

            virtual
            Signal<Ptr>::Ptr
            keyDown()
            {
                return _down;
            }

            virtual
            Signal<Ptr, uint>::Ptr
            keyDown(ScanCode scanCode)
            {
                auto index = static_cast<int>(scanCode);

                if (_keyDown.count(index) == 0)
                    _keyDown[index] = Signal<Ptr, uint>::create();

                return _keyDown[index];
            }

            virtual
            Signal<Ptr>::Ptr
            keyUp()
            {
                return _up;
            }

            Signal<Ptr, uint>::Ptr
            keyUp(ScanCode scanCode)
            {
                auto index = static_cast<int>(scanCode);

                if (_keyUp.count(index) == 0)
                    _keyUp[index] = Signal<Ptr, uint>::create();

                return _keyUp[index];
            }

            virtual
            bool
            keyIsDown(ScanCode scanCode) = 0;

            virtual
            bool
            keyIsDown(KeyCode keyCode) = 0;

        protected:
            Keyboard();

        private:
            static
            const ScanCodeToNameArray
            initializeKeyNames();
         };
    }
}
