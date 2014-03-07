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

            enum class ScanCode
            {
                UNKNOWN,

                A,
                B,
                C,
                D,
                E,
                F,
                G,
                H,
                I,
                J,
                K,
                L,
                M,
                N,
                O,
                P,
                Q,
                R,
                S,
                T,
                U,
                V,
                W,
                X,
                Y,
                Z,

                _1,
                _2,
                _3,
                _4,
                _5,
                _6,
                _7,
                _8,
                _9,
                _0,

                RETURN,
                ESCAPE,
                BACKSPACE,
                TAB,
                SPACE,

                MINUS,
                EQUALS,
                LEFTBRACKET,
                RIGHTBRACKET,
                BACKSLASH,
                NONUSHASH,
                SEMICOLON,
                APOSTROPHE,
                GRAVE,
                COMMA,
                PERIOD,
                SLASH,

                CAPSLOCK,

                F1,
                F2,
                F3,
                F4,
                F5,
                F6,
                F7,
                F8,
                F9,
                F10,
                F11,
                F12,

                PRINTSCREEN,
                SCROLLLOCK,
                PAUSE,
                INSERT,
                HOME,
                PAGEUP,
                DELETE,
                END,
                PAGEDOWN,
                RIGHT,
                LEFT,
                DOWN,
                UP,
                NUMLOCKCLEAR,
                KP_DIVIDE,
                KP_MULTIPLY,
                KP_MINUS,
                KP_PLUS,
                KP_ENTER,
                KP_1,
                KP_2,
                KP_3,
                KP_4,
                KP_5,
                KP_6,
                KP_7,
                KP_8,
                KP_9,
                KP_0,
                KP_PERIOD,
                NONUSBACKSLASH,
                APPLICATION,
                POWER,
                KP_EQUALS,
                F13,
                F14,
                F15,
                F16,
                F17,
                F18,
                F19,
                F20,
                F21,
                F22,
                F23,
                F24,
                EXECUTE,
                HELP,
                MENU,
                SELECT,
                STOP,
                AGAIN,
                UNDO,
                CUT,
                COPY,
                PASTE,
                FIND,
                MUTE,
                VOLUMEUP,
                VOLUMEDOWN,
                KP_COMMA,
                KP_EQUALSAS400,
                INTERNATIONAL1,
                INTERNATIONAL2,
                INTERNATIONAL3,
                INTERNATIONAL4,
                INTERNATIONAL5,
                INTERNATIONAL6,
                INTERNATIONAL7,
                INTERNATIONAL8,
                INTERNATIONAL9,
                LANG1,
                LANG2,
                LANG3,
                LANG4,
                LANG5,
                LANG6,
                LANG7,
                LANG8,
                LANG9,
                ALTERASE,
                SYSREQ,
                CANCEL,
                CLEAR,
                PRIOR,
                RETURN2,
                SEPARATOR,
                OUT,
                OPER,
                CLEARAGAIN,
                CRSEL,
                EXSEL,
                KP_00,
                KP_000,
                THOUSANDSSEPARATOR,
                DECIMALSEPARATOR,
                CURRENCYUNIT,
                CURRENCYSUBUNIT,
                KP_LEFTPAREN,
                KP_RIGHTPAREN,
                KP_LEFTBRACE,
                KP_RIGHTBRACE,
                KP_TAB,
                KP_BACKSPACE,
                KP_A,
                KP_B,
                KP_C,
                KP_D,
                KP_E,
                KP_F,
                KP_XOR,
                KP_POWER,
                KP_PERCENT,
                KP_LESS,
                KP_GREATER,
                KP_AMPERSAND,
                KP_DBLAMPERSAND,
                KP_VERTICALBAR,
                KP_DBLVERTICALBAR,
                KP_COLON,
                KP_HASH,
                KP_SPACE,
                KP_AT,
                KP_EXCLAM,
                KP_MEMSTORE,
                KP_MEMRECALL,
                KP_MEMCLEAR,
                KP_MEMADD,
                KP_MEMSUBTRACT,
                KP_MEMMULTIPLY,
                KP_MEMDIVIDE,
                KP_PLUSMINUS,
                KP_CLEAR,
                KP_CLEARENTRY,
                KP_BINARY,
                KP_OCTAL,
                KP_DECIMAL,
                KP_HEXADECIMAL,

                LCTRL,
                LSHIFT,
                LALT,
                LGUI,
                RCTRL,
                RSHIFT,
                RALT,
                RGUI,
                MODE,

                AUDIONEXT,
                AUDIOPREV,
                AUDIOSTOP,
                AUDIOPLAY,
                AUDIOMUTE,
                MEDIASELECT,
                WWW,
                MAIL,
                CALCULATOR,
                COMPUTER,
                AC_SEARCH,
                AC_HOME,
                AC_BACK,
                AC_FORWARD,
                AC_STOP,
                AC_REFRESH,
                AC_BOOKMARKS,

                BRIGHTNESSDOWN,
                BRIGHTNESSUP,
                DISPLAYSWITCH,
                KBDILLUMTOGGLE,
                KBDILLUMDOWN,
                KBDILLUMUP,
                EJECT,
                SLEEP,

                APP1,
                APP2
            };

            enum class KeyCode
            {
                UNKNOWN,
                FIRST,
                BACKSPACE,
                TAB,
                CLEAR,
                RETURN,
                PAUSE,
                ESCAPE,
                SPACE,
                EXCLAIM,
                QUOTEDBL,
                HASH,
                DOLLAR,
                AMPERSAND,
                QUOTE,
                LEFTPAREN,
                RIGHTPAREN,
                ASTERISK,
                PLUS,
                COMMA,
                MINUS,
                PERIOD,
                SLASH,
                _0,
                _1,
                _2,
                _3,
                _4,
                _5,
                _6,
                _7,
                _8,
                _9,
                COLON,
                SEMICOLON,
                LESS,
                EQUALS,
                GREATER,
                QUESTION,
                AT,
                LEFTBRACKET,
                BACKSLASH,
                RIGHTBRACKET,
                CARET,
                UNDERSCORE,
                BACKQUOTE,
                a,
                b,
                c,
                d,
                e,
                f,
                g,
                h,
                i,
                j,
                k,
                l,
                m,
                n,
                o,
                p,
                q,
                r,
                s,
                t,
                u,
                v,
                w,
                x,
                y,
                z,
                DELETE,
                WORLD_0,
                WORLD_1,
                WORLD_2,
                WORLD_3,
                WORLD_4,
                WORLD_5,
                WORLD_6,
                WORLD_7,
                WORLD_8,
                WORLD_9,
                WORLD_10,
                WORLD_11,
                WORLD_12,
                WORLD_13,
                WORLD_14,
                WORLD_15,
                WORLD_16,
                WORLD_17,
                WORLD_18,
                WORLD_19,
                WORLD_20,
                WORLD_21,
                WORLD_22,
                WORLD_23,
                WORLD_24,
                WORLD_25,
                WORLD_26,
                WORLD_27,
                WORLD_28,
                WORLD_29,
                WORLD_30,
                WORLD_31,
                WORLD_32,
                WORLD_33,
                WORLD_34,
                WORLD_35,
                WORLD_36,
                WORLD_37,
                WORLD_38,
                WORLD_39,
                WORLD_40,
                WORLD_41,
                WORLD_42,
                WORLD_43,
                WORLD_44,
                WORLD_45,
                WORLD_46,
                WORLD_47,
                WORLD_48,
                WORLD_49,
                WORLD_50,
                WORLD_51,
                WORLD_52,
                WORLD_53,
                WORLD_54,
                WORLD_55,
                WORLD_56,
                WORLD_57,
                WORLD_58,
                WORLD_59,
                WORLD_60,
                WORLD_61,
                WORLD_62,
                WORLD_63,
                WORLD_64,
                WORLD_65,
                WORLD_66,
                WORLD_67,
                WORLD_68,
                WORLD_69,
                WORLD_70,
                WORLD_71,
                WORLD_72,
                WORLD_73,
                WORLD_74,
                WORLD_75,
                WORLD_76,
                WORLD_77,
                WORLD_78,
                WORLD_79,
                WORLD_80,
                WORLD_81,
                WORLD_82,
                WORLD_83,
                WORLD_84,
                WORLD_85,
                WORLD_86,
                WORLD_87,
                WORLD_88,
                WORLD_89,
                WORLD_90,
                WORLD_91,
                WORLD_92,
                WORLD_93,
                WORLD_94,
                WORLD_95,
                KP0,
                KP1,
                KP2,
                KP3,
                KP4,
                KP5,
                KP6,
                KP7,
                KP8,
                KP9,
                KP_PERIOD,
                KP_DIVIDE,
                KP_MULTIPLY,
                KP_MINUS,
                KP_PLUS,
                KP_ENTER,
                KP_EQUALS,
                UP,
                DOWN,
                RIGHT,
                LEFT,
                INSERT,
                HOME,
                END,
                PAGEUP,
                PAGEDOWN,
                F1,
                F2,
                F3,
                F4,
                F5,
                F6,
                F7,
                F8,
                F9,
                F10,
                F11,
                F12,
                F13,
                F14,
                F15,
                NUMLOCK,
                CAPSLOCK,
                SCROLLOCK,
                RSHIFT,
                LSHIFT,
                RCTRL,
                LCTRL,
                RALT,
                LALT,
                RMETA,
                LMETA,
                LSUPER,
                RSUPER,
                MODE,
                COMPOSE,
                HELP,
                PRINT,
                SYSREQ,
                BREAK,
                MENU,
                POWER,
                EURO,
                UNDO
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
