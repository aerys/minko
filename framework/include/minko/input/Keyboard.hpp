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

            // from SDL_scancode.h
            enum ScanCode
            {
                UNKNOWN = 0,

                /**
                *  \name Usage page 0x07
                *
                *  These values are from usage page 0x07 (USB keyboard page).
                */
                /*@{*/

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
                /**< Located at the lower left of the return
                *   key on ISO keyboards and at the right end
                *   of the QWERTY row on ANSI keyboards.
                *   Produces REVERSE SOLIDUS (backslash) and
                *   VERTICAL LINE in a US layout, REVERSE
                *   SOLIDUS and VERTICAL LINE in a UK Mac
                *   layout, NUMBER SIGN and TILDE in a UK
                *   Windows layout, DOLLAR SIGN and POUND SIGN
                *   in a Swiss German layout, NUMBER SIGN and
                *   APOSTROPHE in a German layout, GRAVE
                *   ACCENT and POUND SIGN in a French Mac
                *   layout, and ASTERISK and MICRO SIGN in a
                *   French Windows layout.
                */
                BACKSLASH = 49, 
                /**< ISO USB keyboards actually use this code
                *   instead of 49 for the same key, but all
                *   OSes I've seen treat the two codes
                *   identically. So, as an implementor, unless
                *   your keyboard generates both of those
                *   codes and your OS treats them differently,
                *   you should generate BACKSLASH
                *   instead of this code. As a user, you
                *   should not rely on this code because SDL
                *   will never generate it with most (all?)
                *   keyboards.
                */
                NONUSHASH = 50, 
                SEMICOLON = 51,
                APOSTROPHE = 52,
                /**< Located in the top left corner (on both ANSI
                *   and ISO keyboards). Produces GRAVE ACCENT and
                *   TILDE in a US Windows layout and in US and UK
                *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                *   and NOT SIGN in a UK Windows layout, SECTION
                *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                *   layouts on ISO keyboards, SECTION SIGN and
                *   DEGREE SIGN in a Swiss German layout (Mac:
                *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                *   DEGREE SIGN in a German layout (Mac: only on
                *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                *   French Windows layout, COMMERCIAL AT and
                *   NUMBER SIGN in a French Mac layout on ISO
                *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                *   SIGN in a Swiss German, German, or French Mac
                *   layout on ANSI keyboards.
                */
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
                /**< insert on PC, help on some Mac keyboards (but
                does send code 73, not 117) */
                INSERT = 73, 
                HOME = 74,
                PAGEUP = 75,
                DELETE = 76,
                END = 77,
                PAGEDOWN = 78,
                RIGHT = 79,
                LEFT = 80,
                DOWN = 81,
                UP = 82,
                /**< num lock on PC, clear on Mac keyboards
                */
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

                /**< This is the additional key that ISO
                *   keyboards have over ANSI ones,
                *   located between left shift and Y.
                *   Produces GRAVE ACCENT and TILDE in a
                *   US or UK Mac layout, REVERSE SOLIDUS
                *   (backslash) and VERTICAL LINE in a
                *   US or UK Windows layout, and
                *   LESS-THAN SIGN and GREATER-THAN SIGN
                *   in a Swiss German, German, or French
                *   layout. */
                NONUSBACKSLASH = 100, 
                /**< windows contextual menu, compose */
                APPLICATION = 101, 
                /**< The USB document says this is a status flag,
                *   not a physical key - but some Mac keyboards
                *   do have a power key. */
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
                AGAIN = 121,   /**< redo */
                UNDO = 122,
                CUT = 123,
                COPY = 124,
                PASTE = 125,
                FIND = 126,
                MUTE = 127,
                VOLUMEUP = 128,
                VOLUMEDOWN = 129,
                /* not sure whether there's a reason to enable these */
                /*     LOCKINGCAPSLOCK = 130,  */
                /*     LOCKINGNUMLOCK = 131, */
                /*     LOCKINGSCROLLLOCK = 132, */
                KP_COMMA = 133,
                KP_EQUALSAS400 = 134,
                /**< used on Asian keyboards, see
                footnotes in USB doc */
                INTERNATIONAL1 = 135, 
                INTERNATIONAL2 = 136,
                INTERNATIONAL3 = 137, /**< Yen */
                INTERNATIONAL4 = 138,
                INTERNATIONAL5 = 139,
                INTERNATIONAL6 = 140,
                INTERNATIONAL7 = 141,
                INTERNATIONAL8 = 142,
                INTERNATIONAL9 = 143,
                LANG1 = 144, /**< Hangul/English toggle */
                LANG2 = 145, /**< Hanja conversion */
                LANG3 = 146, /**< Katakana */
                LANG4 = 147, /**< Hiragana */
                LANG5 = 148, /**< Zenkaku/Hankaku */
                LANG6 = 149, /**< reserved */
                LANG7 = 150, /**< reserved */
                LANG8 = 151, /**< reserved */
                LANG9 = 152, /**< reserved */

                ALTERASE = 153, /**< Erase-Eaze */
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
                LALT = 226, /**< alt, option */
                LGUI = 227, /**< windows, command (apple), meta */
                RCTRL = 228,
                RSHIFT = 229,
                RALT = 230, /**< alt gr, option */
                RGUI = 231, /**< windows, command (apple), meta */
                /**< I'm not sure if this is really not covered
                *   by any of the above, but since there's a
                *   special KMOD_MODE for it I'm adding it here
                */

                /*@}*//*Usage page 0x07*/

                /**
                *  \name Usage page 0x0C
                *
                *  These values are mapped from usage page 0x0C (USB consumer page).
                */
                /*@{*/
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

                /*@}*//*Usage page 0x0C*/

                /**
                *  \name Walther keys
                *
                *  These are values that Christian Walther added (for mac keyboard?).
                */
                /*@{*/

                BRIGHTNESSDOWN = 275,
                BRIGHTNESSUP = 276,
                /**< display mirroring/dual display
                switch, video mode switch */
                DISPLAYSWITCH = 277, 
                KBDILLUMTOGGLE = 278,
                KBDILLUMDOWN = 279,
                KBDILLUMUP = 280,
                EJECT = 281,
                SLEEP = 282,

                APP1 = 283,
                APP2 = 284
            };

        private:
            typedef std::array<std::string, 285>    ScanCodeToNameArray;

        public:
            static const uint NUM_KEYS;

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

        protected:
            Keyboard();

        private:
            static
            const ScanCodeToNameArray
            initializeKeyNames();
         };
    }
}
