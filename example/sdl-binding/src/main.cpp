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

#include "minko/Minko.hpp"
#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const uint			WINDOW_WIDTH	= 800;
const uint			WINDOW_HEIGHT	= 600;

int 
main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - SDL binding", WINDOW_WIDTH, WINDOW_HEIGHT);

    std::cout << "Please press a key." << std::endl;

    // currently, keyUp events seem not to be fired at the individual key level
	auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
    {
        // Scan code
        if (k->keyIsDown(input::Keyboard::ScanCode::UNKNOWN))
            std::cout << "[ScanCode]Key UNKNOWN down !" << std::endl;

        if (k->keyIsDown(input::Keyboard::ScanCode::A))
            std::cout << "[ScanCode]Key A down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::B))
            std::cout << "[ScanCode]Key B down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::C))
            std::cout << "[ScanCode]Key C down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::D))
            std::cout << "[ScanCode]Key D down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::E))
            std::cout << "[ScanCode]Key E down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F))
            std::cout << "[ScanCode]Key F down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::G))
            std::cout << "[ScanCode]Key G down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::H))
            std::cout << "[ScanCode]Key H down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::I))
            std::cout << "[ScanCode]Key I down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::J))
            std::cout << "[ScanCode]Key J down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::K))
            std::cout << "[ScanCode]Key K down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::L))
            std::cout << "[ScanCode]Key L down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::M))
            std::cout << "[ScanCode]Key M down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::N))
            std::cout << "[ScanCode]Key N down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::O))
            std::cout << "[ScanCode]Key O down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::P))
            std::cout << "[ScanCode]Key P down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::Q))
            std::cout << "[ScanCode]Key Q down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::R))
            std::cout << "[ScanCode]Key R down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::S))
            std::cout << "[ScanCode]Key S down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::T))
            std::cout << "[ScanCode]Key T down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::U))
            std::cout << "[ScanCode]Key U down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::V))
            std::cout << "[ScanCode]Key V down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::W))
            std::cout << "[ScanCode]Key W down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::X))
            std::cout << "[ScanCode]Key X down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::Y))
            std::cout << "[ScanCode]Key Y down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::Z))
            std::cout << "[ScanCode]Key Z down !" << std::endl;

        if (k->keyIsDown(input::Keyboard::ScanCode::_1))
            std::cout << "[ScanCode]Key _1 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::_2))
            std::cout << "[ScanCode]Key _2 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::_3))
            std::cout << "[ScanCode]Key _3 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::_4))
            std::cout << "[ScanCode]Key _4 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::_5))
            std::cout << "[ScanCode]Key _5 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::_6))
            std::cout << "[ScanCode]Key _6 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::_7))
            std::cout << "[ScanCode]Key _7 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::_8))
            std::cout << "[ScanCode]Key _8 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::_9))
            std::cout << "[ScanCode]Key _9 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::_0))
            std::cout << "[ScanCode]Key _0 down !" << std::endl;

        if (k->keyIsDown(input::Keyboard::ScanCode::RETURN))
            std::cout << "[ScanCode]Key RETURN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::ESCAPE))
            std::cout << "[ScanCode]Key ESCAPE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::BACKSPACE))
            std::cout << "[ScanCode]Key BACKSPACE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::TAB))
            std::cout << "[ScanCode]Key TAB down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::SPACE))
            std::cout << "[ScanCode]Key SPACE down !" << std::endl;

        if (k->keyIsDown(input::Keyboard::ScanCode::MINUS))
            std::cout << "[ScanCode]Key MINUS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::EQUALS))
            std::cout << "[ScanCode]Key EQUALS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::LEFTBRACKET))
            std::cout << "[ScanCode]Key LEFTBRACKET down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::RIGHTBRACKET))
            std::cout << "[ScanCode]Key RIGHTBRACKET down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::BACKSLASH))
            std::cout << "[ScanCode]Key BACKSLASH down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::NONUSHASH))
            std::cout << "[ScanCode]Key NONUSHASH down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::SEMICOLON))
            std::cout << "[ScanCode]Key SEMICOLON down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::APOSTROPHE))
            std::cout << "[ScanCode]Key APOSTROPHE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::GRAVE))
            std::cout << "[ScanCode]Key GRAVE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::COMMA))
            std::cout << "[ScanCode]Key COMMA down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::PERIOD))
            std::cout << "[ScanCode]Key PERIOD down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::SLASH))
            std::cout << "[ScanCode]Key SLASH down !" << std::endl;

        if (k->keyIsDown(input::Keyboard::ScanCode::CAPSLOCK))
            std::cout << "[ScanCode]Key CAPSLOCK down !" << std::endl;

        if (k->keyIsDown(input::Keyboard::ScanCode::F1))
            std::cout << "[ScanCode]Key F1 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F2))
            std::cout << "[ScanCode]Key F2 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F3))
            std::cout << "[ScanCode]Key F3 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F4))
            std::cout << "[ScanCode]Key F4 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F5))
            std::cout << "[ScanCode]Key F5 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F6))
            std::cout << "[ScanCode]Key F6 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F7))
            std::cout << "[ScanCode]Key F7 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F8))
            std::cout << "[ScanCode]Key F8 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F9))
            std::cout << "[ScanCode]Key F9 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F10))
            std::cout << "[ScanCode]Key F10 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F11))
            std::cout << "[ScanCode]Key F11 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F12))
            std::cout << "[ScanCode]Key F12 down !" << std::endl;

        if (k->keyIsDown(input::Keyboard::ScanCode::PRINTSCREEN))
            std::cout << "[ScanCode]Key PRINTSCREEN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::SCROLLLOCK))
            std::cout << "[ScanCode]Key SCROLLLOCK down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::PAUSE))
            std::cout << "[ScanCode]Key PAUSE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::INSERT))
            std::cout << "[ScanCode]Key INSERT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::HOME))
            std::cout << "[ScanCode]Key HOME down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::PAGEUP))
            std::cout << "[ScanCode]Key PAGEUP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::DELETE))
            std::cout << "[ScanCode]Key DELETE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::END))
            std::cout << "[ScanCode]Key END down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::PAGEDOWN))
            std::cout << "[ScanCode]Key PAGEDOWN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::RIGHT))
            std::cout << "[ScanCode]Key RIGHT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::LEFT))
            std::cout << "[ScanCode]Key LEFT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::DOWN))
            std::cout << "[ScanCode]Key DOWN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::UP))
            std::cout << "[ScanCode]Key UP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::NUMLOCKCLEAR))
            std::cout << "[ScanCode]Key NUMLOCKCLEAR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_DIVIDE))
            std::cout << "[ScanCode]Key KP_DIVIDE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_MULTIPLY))
            std::cout << "[ScanCode]Key KP_MULTIPLY down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_MINUS))
            std::cout << "[ScanCode]Key KP_MINUS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_PLUS))
            std::cout << "[ScanCode]Key KP_PLUS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_ENTER))
            std::cout << "[ScanCode]Key KP_ENTER down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_1))
            std::cout << "[ScanCode]Key KP_1 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_2))
            std::cout << "[ScanCode]Key KP_2 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_3))
            std::cout << "[ScanCode]Key KP_3 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_4))
            std::cout << "[ScanCode]Key KP_4 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_5))
            std::cout << "[ScanCode]Key KP_5 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_6))
            std::cout << "[ScanCode]Key KP_6 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_7))
            std::cout << "[ScanCode]Key KP_7 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_8))
            std::cout << "[ScanCode]Key KP_8 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_9))
            std::cout << "[ScanCode]Key KP_9 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_0))
            std::cout << "[ScanCode]Key KP_0 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_PERIOD))
            std::cout << "[ScanCode]Key KP_PERIOD down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::NONUSBACKSLASH))
            std::cout << "[ScanCode]Key NONUSBACKSLASH down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::APPLICATION))
            std::cout << "[ScanCode]Key APPLICATION down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::POWER))
            std::cout << "[ScanCode]Key POWER down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_EQUALS))
            std::cout << "[ScanCode]Key KP_EQUALS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F13))
            std::cout << "[ScanCode]Key F13 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F14))
            std::cout << "[ScanCode]Key F14 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F15))
            std::cout << "[ScanCode]Key F15 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F16))
            std::cout << "[ScanCode]Key F16 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F17))
            std::cout << "[ScanCode]Key F17 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F18))
            std::cout << "[ScanCode]Key F18 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F19))
            std::cout << "[ScanCode]Key F19 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F20))
            std::cout << "[ScanCode]Key F20 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F21))
            std::cout << "[ScanCode]Key F21 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F22))
            std::cout << "[ScanCode]Key F22 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F23))
            std::cout << "[ScanCode]Key F23 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::F24))
            std::cout << "[ScanCode]Key F24 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::EXECUTE))
            std::cout << "[ScanCode]Key EXECUTE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::HELP))
            std::cout << "[ScanCode]Key HELP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::MENU))
            std::cout << "[ScanCode]Key MENU down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::SELECT))
            std::cout << "[ScanCode]Key SELECT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::STOP))
            std::cout << "[ScanCode]Key STOP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::AGAIN))
            std::cout << "[ScanCode]Key AGAIN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::UNDO))
            std::cout << "[ScanCode]Key UNDO down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::CUT))
            std::cout << "[ScanCode]Key CUT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::COPY))
            std::cout << "[ScanCode]Key COPY down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::PASTE))
            std::cout << "[ScanCode]Key PASTE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::FIND))
            std::cout << "[ScanCode]Key FIND down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::MUTE))
            std::cout << "[ScanCode]Key MUTE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::VOLUMEUP))
            std::cout << "[ScanCode]Key VOLUMEUP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::VOLUMEDOWN))
            std::cout << "[ScanCode]Key VOLUMEDOWN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_COMMA))
            std::cout << "[ScanCode]Key KP_COMMA down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_EQUALSAS400))
            std::cout << "[ScanCode]Key KP_EQUALSAS400 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::INTERNATIONAL1))
            std::cout << "[ScanCode]Key INTERNATIONAL1 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::INTERNATIONAL2))
            std::cout << "[ScanCode]Key INTERNATIONAL2 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::INTERNATIONAL3))
            std::cout << "[ScanCode]Key INTERNATIONAL3 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::INTERNATIONAL4))
            std::cout << "[ScanCode]Key INTERNATIONAL4 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::INTERNATIONAL5))
            std::cout << "[ScanCode]Key INTERNATIONAL5 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::INTERNATIONAL6))
            std::cout << "[ScanCode]Key INTERNATIONAL6 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::INTERNATIONAL7))
            std::cout << "[ScanCode]Key INTERNATIONAL7 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::INTERNATIONAL8))
            std::cout << "[ScanCode]Key INTERNATIONAL8 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::INTERNATIONAL9))
            std::cout << "[ScanCode]Key INTERNATIONAL9 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::LANG1))
            std::cout << "[ScanCode]Key LANG1 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::LANG2))
            std::cout << "[ScanCode]Key LANG2 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::LANG3))
            std::cout << "[ScanCode]Key LANG3 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::LANG4))
            std::cout << "[ScanCode]Key LANG4 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::LANG5))
            std::cout << "[ScanCode]Key LANG5 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::LANG6))
            std::cout << "[ScanCode]Key LANG6 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::LANG7))
            std::cout << "[ScanCode]Key LANG7 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::LANG8))
            std::cout << "[ScanCode]Key LANG8 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::LANG9))
            std::cout << "[ScanCode]Key LANG9 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::ALTERASE))
            std::cout << "[ScanCode]Key ALTERASE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::SYSREQ))
            std::cout << "[ScanCode]Key SYSREQ down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::CANCEL))
            std::cout << "[ScanCode]Key CANCEL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::CLEAR))
            std::cout << "[ScanCode]Key CLEAR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::PRIOR))
            std::cout << "[ScanCode]Key PRIOR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::RETURN2))
            std::cout << "[ScanCode]Key RETURN2 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::SEPARATOR))
            std::cout << "[ScanCode]Key SEPARATOR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::OUT))
            std::cout << "[ScanCode]Key OUT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::OPER))
            std::cout << "[ScanCode]Key OPER down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::CLEARAGAIN))
            std::cout << "[ScanCode]Key CLEARAGAIN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::CRSEL))
            std::cout << "[ScanCode]Key CRSEL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::EXSEL))
            std::cout << "[ScanCode]Key EXSEL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_00))
            std::cout << "[ScanCode]Key KP_00 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_000))
            std::cout << "[ScanCode]Key KP_000 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::THOUSANDSSEPARATOR))
            std::cout << "[ScanCode]Key THOUSANDSSEPARATOR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::DECIMALSEPARATOR))
            std::cout << "[ScanCode]Key DECIMALSEPARATOR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::CURRENCYUNIT))
            std::cout << "[ScanCode]Key CURRENCYUNIT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::CURRENCYSUBUNIT))
            std::cout << "[ScanCode]Key CURRENCYSUBUNIT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_LEFTPAREN))
            std::cout << "[ScanCode]Key KP_LEFTPAREN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_RIGHTPAREN))
            std::cout << "[ScanCode]Key KP_RIGHTPAREN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_LEFTBRACE))
            std::cout << "[ScanCode]Key KP_LEFTBRACE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_RIGHTBRACE))
            std::cout << "[ScanCode]Key KP_RIGHTBRACE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_TAB))
            std::cout << "[ScanCode]Key KP_TAB down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_BACKSPACE))
            std::cout << "[ScanCode]Key KP_BACKSPACE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_A))
            std::cout << "[ScanCode]Key KP_A down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_B))
            std::cout << "[ScanCode]Key KP_B down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_C))
            std::cout << "[ScanCode]Key KP_C down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_D))
            std::cout << "[ScanCode]Key KP_D down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_E))
            std::cout << "[ScanCode]Key KP_E down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_F))
            std::cout << "[ScanCode]Key KP_F down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_XOR))
            std::cout << "[ScanCode]Key KP_XOR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_POWER))
            std::cout << "[ScanCode]Key KP_POWER down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_PERCENT))
            std::cout << "[ScanCode]Key KP_PERCENT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_LESS))
            std::cout << "[ScanCode]Key KP_LESS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_GREATER))
            std::cout << "[ScanCode]Key KP_GREATER down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_AMPERSAND))
            std::cout << "[ScanCode]Key KP_AMPERSAND down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_DBLAMPERSAND))
            std::cout << "[ScanCode]Key KP_DBLAMPERSAND down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_VERTICALBAR))
            std::cout << "[ScanCode]Key KP_VERTICALBAR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_DBLVERTICALBAR))
            std::cout << "[ScanCode]Key KP_DBLVERTICALBAR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_COLON))
            std::cout << "[ScanCode]Key KP_COLON down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_HASH))
            std::cout << "[ScanCode]Key KP_HASH down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_SPACE))
            std::cout << "[ScanCode]Key KP_SPACE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_AT))
            std::cout << "[ScanCode]Key KP_AT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_EXCLAM))
            std::cout << "[ScanCode]Key KP_EXCLAM down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_MEMSTORE))
            std::cout << "[ScanCode]Key KP_MEMSTORE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_MEMRECALL))
            std::cout << "[ScanCode]Key KP_MEMRECALL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_MEMCLEAR))
            std::cout << "[ScanCode]Key KP_MEMCLEAR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_MEMADD))
            std::cout << "[ScanCode]Key KP_MEMADD down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_MEMSUBTRACT))
            std::cout << "[ScanCode]Key KP_MEMSUBTRACT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_MEMMULTIPLY))
            std::cout << "[ScanCode]Key KP_MEMMULTIPLY down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_MEMDIVIDE))
            std::cout << "[ScanCode]Key KP_MEMDIVIDE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_PLUSMINUS))
            std::cout << "[ScanCode]Key KP_PLUSMINUS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_CLEAR))
            std::cout << "[ScanCode]Key KP_CLEAR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_CLEARENTRY))
            std::cout << "[ScanCode]Key KP_CLEARENTRY down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_BINARY))
            std::cout << "[ScanCode]Key KP_BINARY down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_OCTAL))
            std::cout << "[ScanCode]Key KP_OCTAL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_DECIMAL))
            std::cout << "[ScanCode]Key KP_DECIMAL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KP_HEXADECIMAL))
            std::cout << "[ScanCode]Key KP_HEXADECIMAL down !" << std::endl;

        if (k->keyIsDown(input::Keyboard::ScanCode::LCTRL))
            std::cout << "[ScanCode]Key LCTRL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::LSHIFT))
            std::cout << "[ScanCode]Key LSHIFT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::LALT))
            std::cout << "[ScanCode]Key LALT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::LGUI))
            std::cout << "[ScanCode]Key LGUI down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::RCTRL))
            std::cout << "[ScanCode]Key RCTRL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::RSHIFT))
            std::cout << "[ScanCode]Key RSHIFT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::RALT))
            std::cout << "[ScanCode]Key RALT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::RGUI))
            std::cout << "[ScanCode]Key RGUI down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::MODE))
            std::cout << "[ScanCode]Key MODE down !" << std::endl;

        if (k->keyIsDown(input::Keyboard::ScanCode::AUDIONEXT))
            std::cout << "[ScanCode]Key AUDIONEXT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::AUDIOPREV))
            std::cout << "[ScanCode]Key AUDIOPREV down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::AUDIOSTOP))
            std::cout << "[ScanCode]Key AUDIOSTOP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::AUDIOPLAY))
            std::cout << "[ScanCode]Key AUDIOPLAY down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::AUDIOMUTE))
            std::cout << "[ScanCode]Key AUDIOMUTE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::MEDIASELECT))
            std::cout << "[ScanCode]Key MEDIASELECT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::WWW))
            std::cout << "[ScanCode]Key WWW down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::MAIL))
            std::cout << "[ScanCode]Key MAIL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::CALCULATOR))
            std::cout << "[ScanCode]Key CALCULATOR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::COMPUTER))
            std::cout << "[ScanCode]Key COMPUTER down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::AC_SEARCH))
            std::cout << "[ScanCode]Key AC_SEARCH down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::AC_HOME))
            std::cout << "[ScanCode]Key AC_HOME down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::AC_BACK))
            std::cout << "[ScanCode]Key AC_BACK down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::AC_FORWARD))
            std::cout << "[ScanCode]Key AC_FORWARD down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::AC_STOP))
            std::cout << "[ScanCode]Key AC_STOP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::AC_REFRESH))
            std::cout << "[ScanCode]Key AC_REFRESH down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::AC_BOOKMARKS))
            std::cout << "[ScanCode]Key AC_BOOKMARKS down !" << std::endl;

        if (k->keyIsDown(input::Keyboard::ScanCode::BRIGHTNESSDOWN))
            std::cout << "[ScanCode]Key BRIGHTNESSDOWN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::BRIGHTNESSUP))
            std::cout << "[ScanCode]Key BRIGHTNESSUP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::DISPLAYSWITCH))
            std::cout << "[ScanCode]Key DISPLAYSWITCH down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KBDILLUMTOGGLE))
            std::cout << "[ScanCode]Key KBDILLUMTOGGLE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KBDILLUMDOWN))
            std::cout << "[ScanCode]Key KBDILLUMDOWN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::KBDILLUMUP))
            std::cout << "[ScanCode]Key KBDILLUMUP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::EJECT))
            std::cout << "[ScanCode]Key EJECT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::SLEEP))
            std::cout << "[ScanCode]Key SLEEP down !" << std::endl;

        if (k->keyIsDown(input::Keyboard::ScanCode::APP1))
            std::cout << "[ScanCode]Key APP1 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ScanCode::APP2))
            std::cout << "[ScanCode]Key APP2 down !" << std::endl;

        // Key codes
        if (k->keyIsDown(input::Keyboard::KeyCode::UNKNOWN))
            std::cout << "[KeyCode]Key UNKNOWN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::FIRST))
            std::cout << "[KeyCode]Key FIRST down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::BACKSPACE))
            std::cout << "[KeyCode]Key BACKSPACE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::TAB))
            std::cout << "[KeyCode]Key TAB down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::CLEAR))
            std::cout << "[KeyCode]Key CLEAR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::RETURN))
            std::cout << "[KeyCode]Key RETURN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::PAUSE))
            std::cout << "[KeyCode]Key PAUSE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::ESCAPE))
            std::cout << "[KeyCode]Key ESCAPE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::SPACE))
            std::cout << "[KeyCode]Key SPACE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::EXCLAIM))
            std::cout << "[KeyCode]Key EXCLAIM down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::QUOTEDBL))
            std::cout << "[KeyCode]Key QUOTEDBL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::HASH))
            std::cout << "[KeyCode]Key HASH down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::DOLLAR))
            std::cout << "[KeyCode]Key DOLLAR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::AMPERSAND))
            std::cout << "[KeyCode]Key AMPERSAND down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::QUOTE))
            std::cout << "[KeyCode]Key QUOTE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::LEFTPAREN))
            std::cout << "[KeyCode]Key LEFTPAREN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::RIGHTPAREN))
            std::cout << "[KeyCode]Key RIGHTPAREN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::ASTERISK))
            std::cout << "[KeyCode]Key ASTERISK down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::PLUS))
            std::cout << "[KeyCode]Key PLUS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::COMMA))
            std::cout << "[KeyCode]Key COMMA down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::MINUS))
            std::cout << "[KeyCode]Key MINUS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::PERIOD))
            std::cout << "[KeyCode]Key PERIOD down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::SLASH))
            std::cout << "[KeyCode]Key SLASH down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::_0))
            std::cout << "[KeyCode]Key _0 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::_1))
            std::cout << "[KeyCode]Key _1 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::_2))
            std::cout << "[KeyCode]Key _2 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::_3))
            std::cout << "[KeyCode]Key _3 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::_4))
            std::cout << "[KeyCode]Key _4 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::_5))
            std::cout << "[KeyCode]Key _5 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::_6))
            std::cout << "[KeyCode]Key _6 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::_7))
            std::cout << "[KeyCode]Key _7 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::_8))
            std::cout << "[KeyCode]Key _8 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::_9))
            std::cout << "[KeyCode]Key _9 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::COLON))
            std::cout << "[KeyCode]Key COLON down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::SEMICOLON))
            std::cout << "[KeyCode]Key SEMICOLON down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::LESS))
            std::cout << "[KeyCode]Key LESS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::EQUALS))
            std::cout << "[KeyCode]Key EQUALS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::GREATER))
            std::cout << "[KeyCode]Key GREATER down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::QUESTION))
            std::cout << "[KeyCode]Key QUESTION down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::AT))
            std::cout << "[KeyCode]Key AT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::LEFTBRACKET))
            std::cout << "[KeyCode]Key LEFTBRACKET down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::BACKSLASH))
            std::cout << "[KeyCode]Key BACKSLASH down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::RIGHTBRACKET))
            std::cout << "[KeyCode]Key RIGHTBRACKET down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::CARET))
            std::cout << "[KeyCode]Key CARET down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::UNDERSCORE))
            std::cout << "[KeyCode]Key UNDERSCORE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::BACKQUOTE))
            std::cout << "[KeyCode]Key BACKQUOTE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::a))
            std::cout << "[KeyCode]Key a down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::b))
            std::cout << "[KeyCode]Key b down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::c))
            std::cout << "[KeyCode]Key c down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::d))
            std::cout << "[KeyCode]Key d down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::e))
            std::cout << "[KeyCode]Key e down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::f))
            std::cout << "[KeyCode]Key f down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::g))
            std::cout << "[KeyCode]Key g down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::h))
            std::cout << "[KeyCode]Key h down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::i))
            std::cout << "[KeyCode]Key i down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::j))
            std::cout << "[KeyCode]Key j down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::k))
            std::cout << "[KeyCode]Key k down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::l))
            std::cout << "[KeyCode]Key l down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::m))
            std::cout << "[KeyCode]Key m down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::n))
            std::cout << "[KeyCode]Key n down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::o))
            std::cout << "[KeyCode]Key o down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::p))
            std::cout << "[KeyCode]Key p down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::q))
            std::cout << "[KeyCode]Key q down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::r))
            std::cout << "[KeyCode]Key r down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::s))
            std::cout << "[KeyCode]Key s down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::t))
            std::cout << "[KeyCode]Key t down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::u))
            std::cout << "[KeyCode]Key u down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::v))
            std::cout << "[KeyCode]Key v down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::w))
            std::cout << "[KeyCode]Key w down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::x))
            std::cout << "[KeyCode]Key x down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::y))
            std::cout << "[KeyCode]Key y down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::z))
            std::cout << "[KeyCode]Key z down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::DELETE))
            std::cout << "[KeyCode]Key DELETE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_0))
            std::cout << "[KeyCode]Key WORLD_0 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_1))
            std::cout << "[KeyCode]Key WORLD_1 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_2))
            std::cout << "[KeyCode]Key WORLD_2 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_3))
            std::cout << "[KeyCode]Key WORLD_3 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_4))
            std::cout << "[KeyCode]Key WORLD_4 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_5))
            std::cout << "[KeyCode]Key WORLD_5 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_6))
            std::cout << "[KeyCode]Key WORLD_6 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_7))
            std::cout << "[KeyCode]Key WORLD_7 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_8))
            std::cout << "[KeyCode]Key WORLD_8 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_9))
            std::cout << "[KeyCode]Key WORLD_9 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_10))
            std::cout << "[KeyCode]Key WORLD_10 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_11))
            std::cout << "[KeyCode]Key WORLD_11 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_12))
            std::cout << "[KeyCode]Key WORLD_12 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_13))
            std::cout << "[KeyCode]Key WORLD_13 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_14))
            std::cout << "[KeyCode]Key WORLD_14 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_15))
            std::cout << "[KeyCode]Key WORLD_15 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_16))
            std::cout << "[KeyCode]Key WORLD_16 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_17))
            std::cout << "[KeyCode]Key WORLD_17 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_18))
            std::cout << "[KeyCode]Key WORLD_18 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_19))
            std::cout << "[KeyCode]Key WORLD_19 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_20))
            std::cout << "[KeyCode]Key WORLD_20 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_21))
            std::cout << "[KeyCode]Key WORLD_21 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_22))
            std::cout << "[KeyCode]Key WORLD_22 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_23))
            std::cout << "[KeyCode]Key WORLD_23 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_24))
            std::cout << "[KeyCode]Key WORLD_24 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_25))
            std::cout << "[KeyCode]Key WORLD_25 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_26))
            std::cout << "[KeyCode]Key WORLD_26 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_27))
            std::cout << "[KeyCode]Key WORLD_27 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_28))
            std::cout << "[KeyCode]Key WORLD_28 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_29))
            std::cout << "[KeyCode]Key WORLD_29 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_30))
            std::cout << "[KeyCode]Key WORLD_30 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_31))
            std::cout << "[KeyCode]Key WORLD_31 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_32))
            std::cout << "[KeyCode]Key WORLD_32 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_33))
            std::cout << "[KeyCode]Key WORLD_33 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_34))
            std::cout << "[KeyCode]Key WORLD_34 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_35))
            std::cout << "[KeyCode]Key WORLD_35 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_36))
            std::cout << "[KeyCode]Key WORLD_36 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_37))
            std::cout << "[KeyCode]Key WORLD_37 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_38))
            std::cout << "[KeyCode]Key WORLD_38 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_39))
            std::cout << "[KeyCode]Key WORLD_39 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_40))
            std::cout << "[KeyCode]Key WORLD_40 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_41))
            std::cout << "[KeyCode]Key WORLD_41 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_42))
            std::cout << "[KeyCode]Key WORLD_42 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_43))
            std::cout << "[KeyCode]Key WORLD_43 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_44))
            std::cout << "[KeyCode]Key WORLD_44 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_45))
            std::cout << "[KeyCode]Key WORLD_45 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_46))
            std::cout << "[KeyCode]Key WORLD_46 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_47))
            std::cout << "[KeyCode]Key WORLD_47 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_48))
            std::cout << "[KeyCode]Key WORLD_48 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_49))
            std::cout << "[KeyCode]Key WORLD_49 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_50))
            std::cout << "[KeyCode]Key WORLD_50 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_51))
            std::cout << "[KeyCode]Key WORLD_51 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_52))
            std::cout << "[KeyCode]Key WORLD_52 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_53))
            std::cout << "[KeyCode]Key WORLD_53 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_54))
            std::cout << "[KeyCode]Key WORLD_54 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_55))
            std::cout << "[KeyCode]Key WORLD_55 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_56))
            std::cout << "[KeyCode]Key WORLD_56 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_57))
            std::cout << "[KeyCode]Key WORLD_57 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_58))
            std::cout << "[KeyCode]Key WORLD_58 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_59))
            std::cout << "[KeyCode]Key WORLD_59 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_60))
            std::cout << "[KeyCode]Key WORLD_60 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_61))
            std::cout << "[KeyCode]Key WORLD_61 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_62))
            std::cout << "[KeyCode]Key WORLD_62 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_63))
            std::cout << "[KeyCode]Key WORLD_63 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_64))
            std::cout << "[KeyCode]Key WORLD_64 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_65))
            std::cout << "[KeyCode]Key WORLD_65 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_66))
            std::cout << "[KeyCode]Key WORLD_66 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_67))
            std::cout << "[KeyCode]Key WORLD_67 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_68))
            std::cout << "[KeyCode]Key WORLD_68 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_69))
            std::cout << "[KeyCode]Key WORLD_69 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_70))
            std::cout << "[KeyCode]Key WORLD_70 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_71))
            std::cout << "[KeyCode]Key WORLD_71 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_72))
            std::cout << "[KeyCode]Key WORLD_72 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_73))
            std::cout << "[KeyCode]Key WORLD_73 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_74))
            std::cout << "[KeyCode]Key WORLD_74 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_75))
            std::cout << "[KeyCode]Key WORLD_75 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_76))
            std::cout << "[KeyCode]Key WORLD_76 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_77))
            std::cout << "[KeyCode]Key WORLD_77 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_78))
            std::cout << "[KeyCode]Key WORLD_78 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_79))
            std::cout << "[KeyCode]Key WORLD_79 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_80))
            std::cout << "[KeyCode]Key WORLD_80 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_81))
            std::cout << "[KeyCode]Key WORLD_81 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_82))
            std::cout << "[KeyCode]Key WORLD_82 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_83))
            std::cout << "[KeyCode]Key WORLD_83 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_84))
            std::cout << "[KeyCode]Key WORLD_84 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_85))
            std::cout << "[KeyCode]Key WORLD_85 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_86))
            std::cout << "[KeyCode]Key WORLD_86 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_87))
            std::cout << "[KeyCode]Key WORLD_87 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_88))
            std::cout << "[KeyCode]Key WORLD_88 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_89))
            std::cout << "[KeyCode]Key WORLD_89 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_90))
            std::cout << "[KeyCode]Key WORLD_90 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_91))
            std::cout << "[KeyCode]Key WORLD_91 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_92))
            std::cout << "[KeyCode]Key WORLD_92 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_93))
            std::cout << "[KeyCode]Key WORLD_93 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_94))
            std::cout << "[KeyCode]Key WORLD_94 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::WORLD_95))
            std::cout << "[KeyCode]Key WORLD_95 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP0))
            std::cout << "[KeyCode]Key KP0 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP1))
            std::cout << "[KeyCode]Key KP1 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP2))
            std::cout << "[KeyCode]Key KP2 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP3))
            std::cout << "[KeyCode]Key KP3 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP4))
            std::cout << "[KeyCode]Key KP4 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP5))
            std::cout << "[KeyCode]Key KP5 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP6))
            std::cout << "[KeyCode]Key KP6 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP7))
            std::cout << "[KeyCode]Key KP7 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP8))
            std::cout << "[KeyCode]Key KP8 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP9))
            std::cout << "[KeyCode]Key KP9 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP_PERIOD))
            std::cout << "[KeyCode]Key KP_PERIOD down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP_DIVIDE))
            std::cout << "[KeyCode]Key KP_DIVIDE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP_MULTIPLY))
            std::cout << "[KeyCode]Key KP_MULTIPLY down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP_MINUS))
            std::cout << "[KeyCode]Key KP_MINUS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP_PLUS))
            std::cout << "[KeyCode]Key KP_PLUS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP_ENTER))
            std::cout << "[KeyCode]Key KP_ENTER down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::KP_EQUALS))
            std::cout << "[KeyCode]Key KP_EQUALS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::UP))
            std::cout << "[KeyCode]Key UP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::DOWN))
            std::cout << "[KeyCode]Key DOWN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::RIGHT))
            std::cout << "[KeyCode]Key RIGHT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::LEFT))
            std::cout << "[KeyCode]Key LEFT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::INSERT))
            std::cout << "[KeyCode]Key INSERT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::HOME))
            std::cout << "[KeyCode]Key HOME down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::END))
            std::cout << "[KeyCode]Key END down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::PAGEUP))
            std::cout << "[KeyCode]Key PAGEUP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::PAGEDOWN))
            std::cout << "[KeyCode]Key PAGEDOWN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::F1))
            std::cout << "[KeyCode]Key F1 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::F2))
            std::cout << "[KeyCode]Key F2 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::F3))
            std::cout << "[KeyCode]Key F3 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::F4))
            std::cout << "[KeyCode]Key F4 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::F5))
            std::cout << "[KeyCode]Key F5 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::F6))
            std::cout << "[KeyCode]Key F6 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::F7))
            std::cout << "[KeyCode]Key F7 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::F8))
            std::cout << "[KeyCode]Key F8 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::F9))
            std::cout << "[KeyCode]Key F9 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::F10))
            std::cout << "[KeyCode]Key F10 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::F11))
            std::cout << "[KeyCode]Key F11 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::F12))
            std::cout << "[KeyCode]Key F12 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::F13))
            std::cout << "[KeyCode]Key F13 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::F14))
            std::cout << "[KeyCode]Key F14 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::F15))
            std::cout << "[KeyCode]Key F15 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::NUMLOCK))
            std::cout << "[KeyCode]Key NUMLOCK down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::CAPSLOCK))
            std::cout << "[KeyCode]Key CAPSLOCK down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::SCROLLOCK))
            std::cout << "[KeyCode]Key SCROLLOCK down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::RSHIFT))
            std::cout << "[KeyCode]Key RSHIFT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::LSHIFT))
            std::cout << "[KeyCode]Key LSHIFT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::RCTRL))
            std::cout << "[KeyCode]Key RCTRL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::LCTRL))
            std::cout << "[KeyCode]Key LCTRL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::RALT))
            std::cout << "[KeyCode]Key RALT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::LALT))
            std::cout << "[KeyCode]Key LALT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::RMETA))
            std::cout << "[KeyCode]Key RMETA down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::LMETA))
            std::cout << "[KeyCode]Key LMETA down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::LSUPER))
            std::cout << "[KeyCode]Key LSUPER down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::RSUPER))
            std::cout << "[KeyCode]Key RSUPER down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::MODE))
            std::cout << "[KeyCode]Key MODE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::COMPOSE))
            std::cout << "[KeyCode]Key COMPOSE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::HELP))
            std::cout << "[KeyCode]Key HELP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::PRINT))
            std::cout << "[KeyCode]Key PRINT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::SYSREQ))
            std::cout << "[KeyCode]Key SYSREQ down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::BREAK))
            std::cout << "[KeyCode]Key BREAK down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::MENU))
            std::cout << "[KeyCode]Key MENU down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::POWER))
            std::cout << "[KeyCode]Key POWER down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::EURO))
            std::cout << "[KeyCode]Key EURO down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KeyCode::UNDO))
            std::cout << "[KeyCode]Key UNDO down !" << std::endl;
	});

	canvas->run();

	return 0;
}