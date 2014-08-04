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

const uint    WINDOW_WIDTH = 800;
const uint    WINDOW_HEIGHT = 600;

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - SDL binding", WINDOW_WIDTH, WINDOW_HEIGHT);

    std::cout << "Please press a key." << std::endl;

    // currently, keyUp events seem not to be fired at the individual key level
    auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
    {
        // Keyboard
        if (k->keyIsDown(input::Keyboard::CANCEL))
            std::cout << "[Key]Key CANCEL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::HELP))
            std::cout << "[Key]Key HELP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::BACK_SPACE))
            std::cout << "[Key]Key BACK_SPACE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::TAB))
            std::cout << "[Key]Key TAB down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::CLEAR))
            std::cout << "[Key]Key CLEAR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::RETURN))
            std::cout << "[Key]Key RETURN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ENTER))
            std::cout << "[Key]Key ENTER down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::SHIFT))
            std::cout << "[Key]Key SHIFT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::CONTROL))
            std::cout << "[Key]Key CONTROL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ALT))
            std::cout << "[Key]Key ALT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::PAUSE))
            std::cout << "[Key]Key PAUSE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::CAPS_LOCK))
            std::cout << "[Key]Key CAPS_LOCK down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::KANA))
            std::cout << "[Key]Key KANA down !" << std::endl;
        /*if (k->keyIsDown(input::Keyboard::HANGUL))
            std::cout << "[Key]Key HANGUL down !" << std::endl;
        */
        if (k->keyIsDown(input::Keyboard::EISU))
            std::cout << "[Key]Key EISU down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::JUNJA))
            std::cout << "[Key]Key JUNJA down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::FINAL))
            std::cout << "[Key]Key FINAL down !" << std::endl;
        /*if (k->keyIsDown(input::Keyboard::HANJA))
            std::cout << "[Key]Key HANJA down !" << std::endl;
        */
        if (k->keyIsDown(input::Keyboard::KANJI))
            std::cout << "[Key]Key KANJI down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ESCAPE))
            std::cout << "[Key]Key ESCAPE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::CONVERT))
            std::cout << "[Key]Key CONVERT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::NONCONVERT))
            std::cout << "[Key]Key NONCONVERT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ACCEPT))
            std::cout << "[Key]Key ACCEPT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::MODECHANGE))
            std::cout << "[Key]Key MODECHANGE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::SPACE))
            std::cout << "[Key]Key SPACE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::PAGE_UP))
            std::cout << "[Key]Key PAGE_UP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::PAGE_DOWN))
            std::cout << "[Key]Key PAGE_DOWN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::END))
            std::cout << "[Key]Key END down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::HOME))
            std::cout << "[Key]Key HOME down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::LEFT))
            std::cout << "[Key]Key LEFT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::UP))
            std::cout << "[Key]Key UP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::RIGHT))
            std::cout << "[Key]Key RIGHT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::DOWN))
            std::cout << "[Key]Key DOWN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::SELECT))
            std::cout << "[Key]Key SELECT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::PRINT))
            std::cout << "[Key]Key PRINT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::EXECUTE))
            std::cout << "[Key]Key EXECUTE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::PRINTSCREEN))
            std::cout << "[Key]Key PRINTSCREEN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::INSERT))
            std::cout << "[Key]Key INSERT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::DELETE))
            std::cout << "[Key]Key DELETE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::_0))
            std::cout << "[Key]Key _0 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::_1))
            std::cout << "[Key]Key _1 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::_2))
            std::cout << "[Key]Key _2 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::_3))
            std::cout << "[Key]Key _3 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::_4))
            std::cout << "[Key]Key _4 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::_5))
            std::cout << "[Key]Key _5 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::_6))
            std::cout << "[Key]Key _6 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::_7))
            std::cout << "[Key]Key _7 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::_8))
            std::cout << "[Key]Key _8 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::_9))
            std::cout << "[Key]Key _9 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::COLON))
            std::cout << "[Key]Key COLON down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::SEMICOLON))
            std::cout << "[Key]Key SEMICOLON down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::LESS_THAN))
            std::cout << "[Key]Key LESS_THAN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::EQUALS))
            std::cout << "[Key]Key EQUALS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::GREATER_THAN))
            std::cout << "[Key]Key GREATER_THAN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::QUESTION_MARK))
            std::cout << "[Key]Key QUESTION_MARK down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::AT))
            std::cout << "[Key]Key AT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::A))
            std::cout << "[Key]Key A down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::B))
            std::cout << "[Key]Key B down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::C))
            std::cout << "[Key]Key C down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::D))
            std::cout << "[Key]Key D down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::E))
            std::cout << "[Key]Key E down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F))
            std::cout << "[Key]Key F down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::G))
            std::cout << "[Key]Key G down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::H))
            std::cout << "[Key]Key H down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::I))
            std::cout << "[Key]Key I down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::J))
            std::cout << "[Key]Key J down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::K))
            std::cout << "[Key]Key K down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::L))
            std::cout << "[Key]Key L down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::M))
            std::cout << "[Key]Key M down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::N))
            std::cout << "[Key]Key N down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::O))
            std::cout << "[Key]Key O down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::P))
            std::cout << "[Key]Key P down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::Q))
            std::cout << "[Key]Key Q down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::R))
            std::cout << "[Key]Key R down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::S))
            std::cout << "[Key]Key S down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::T))
            std::cout << "[Key]Key T down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::U))
            std::cout << "[Key]Key U down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::V))
            std::cout << "[Key]Key V down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::W))
            std::cout << "[Key]Key W down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::X))
            std::cout << "[Key]Key X down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::Y))
            std::cout << "[Key]Key Y down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::Z))
            std::cout << "[Key]Key Z down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN))
            std::cout << "[Key]Key WIN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::CONTEXT_MENU))
            std::cout << "[Key]Key CONTEXT_MENU down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::SLEEP))
            std::cout << "[Key]Key SLEEP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::NUMPAD0))
            std::cout << "[Key]Key NUMPAD0 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::NUMPAD1))
            std::cout << "[Key]Key NUMPAD1 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::NUMPAD2))
            std::cout << "[Key]Key NUMPAD2 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::NUMPAD3))
            std::cout << "[Key]Key NUMPAD3 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::NUMPAD4))
            std::cout << "[Key]Key NUMPAD4 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::NUMPAD5))
            std::cout << "[Key]Key NUMPAD5 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::NUMPAD6))
            std::cout << "[Key]Key NUMPAD6 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::NUMPAD7))
            std::cout << "[Key]Key NUMPAD7 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::NUMPAD8))
            std::cout << "[Key]Key NUMPAD8 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::NUMPAD9))
            std::cout << "[Key]Key NUMPAD9 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::MULTIPLY))
            std::cout << "[Key]Key MULTIPLY down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ADD))
            std::cout << "[Key]Key ADD down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::SEPARATOR))
            std::cout << "[Key]Key SEPARATOR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::SUBTRACT))
            std::cout << "[Key]Key SUBTRACT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::DECIMAL))
            std::cout << "[Key]Key DECIMAL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::DIVIDE))
            std::cout << "[Key]Key DIVIDE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F1))
            std::cout << "[Key]Key F1 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F2))
            std::cout << "[Key]Key F2 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F3))
            std::cout << "[Key]Key F3 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F4))
            std::cout << "[Key]Key F4 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F5))
            std::cout << "[Key]Key F5 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F6))
            std::cout << "[Key]Key F6 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F7))
            std::cout << "[Key]Key F7 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F8))
            std::cout << "[Key]Key F8 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F9))
            std::cout << "[Key]Key F9 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F10))
            std::cout << "[Key]Key F10 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F11))
            std::cout << "[Key]Key F11 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F12))
            std::cout << "[Key]Key F12 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F13))
            std::cout << "[Key]Key F13 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F14))
            std::cout << "[Key]Key F14 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F15))
            std::cout << "[Key]Key F15 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F16))
            std::cout << "[Key]Key F16 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F17))
            std::cout << "[Key]Key F17 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F18))
            std::cout << "[Key]Key F18 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F19))
            std::cout << "[Key]Key F19 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F20))
            std::cout << "[Key]Key F20 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F21))
            std::cout << "[Key]Key F21 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F22))
            std::cout << "[Key]Key F22 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F23))
            std::cout << "[Key]Key F23 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::F24))
            std::cout << "[Key]Key F24 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::NUM_LOCK))
            std::cout << "[Key]Key NUM_LOCK down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::SCROLL_LOCK))
            std::cout << "[Key]Key SCROLL_LOCK down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_FJ_JISHO))
            std::cout << "[Key]Key WIN_OEM_FJ_JISHO down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_FJ_MASSHOU))
            std::cout << "[Key]Key WIN_OEM_FJ_MASSHOU down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_FJ_TOUROKU))
            std::cout << "[Key]Key WIN_OEM_FJ_TOUROKU down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_FJ_LOYA))
            std::cout << "[Key]Key WIN_OEM_FJ_LOYA down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_FJ_ROYA))
            std::cout << "[Key]Key WIN_OEM_FJ_ROYA down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::CIRCUMFLEX))
            std::cout << "[Key]Key CIRCUMFLEX down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::EXCLAMATION))
            std::cout << "[Key]Key EXCLAMATION down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::DOUBLE_QUOTE))
            std::cout << "[Key]Key DOUBLE_QUOTE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::HASH))
            std::cout << "[Key]Key HASH down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::DOLLAR))
            std::cout << "[Key]Key DOLLAR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::PERCENT))
            std::cout << "[Key]Key PERCENT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::AMPERSAND))
            std::cout << "[Key]Key AMPERSAND down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::UNDERSCORE))
            std::cout << "[Key]Key UNDERSCORE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::OPEN_PAREN))
            std::cout << "[Key]Key OPEN_PAREN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::CLOSE_PAREN))
            std::cout << "[Key]Key CLOSE_PAREN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ASTERISK))
            std::cout << "[Key]Key ASTERISK down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::PLUS))
            std::cout << "[Key]Key PLUS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::PIPE))
            std::cout << "[Key]Key PIPE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::HYPHEN_MINUS))
            std::cout << "[Key]Key HYPHEN_MINUS down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::OPEN_CURLY_BRACKET))
            std::cout << "[Key]Key OPEN_CURLY_BRACKET down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::CLOSE_CURLY_BRACKET))
            std::cout << "[Key]Key CLOSE_CURLY_BRACKET down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::TILDE))
            std::cout << "[Key]Key TILDE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::VOLUME_MUTE))
            std::cout << "[Key]Key VOLUME_MUTE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::VOLUME_DOWN))
            std::cout << "[Key]Key VOLUME_DOWN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::VOLUME_UP))
            std::cout << "[Key]Key VOLUME_UP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::COMMA))
            std::cout << "[Key]Key COMMA down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::PERIOD))
            std::cout << "[Key]Key PERIOD down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::SLASH))
            std::cout << "[Key]Key SLASH down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::BACK_QUOTE))
            std::cout << "[Key]Key BACK_QUOTE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::OPEN_BRACKET))
            std::cout << "[Key]Key OPEN_BRACKET down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::BACK_SLASH))
            std::cout << "[Key]Key BACK_SLASH down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::CLOSE_BRACKET))
            std::cout << "[Key]Key CLOSE_BRACKET down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::QUOTE))
            std::cout << "[Key]Key QUOTE down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::META))
            std::cout << "[Key]Key META down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ALTGR))
            std::cout << "[Key]Key ALTGR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_ICO_HELP))
            std::cout << "[Key]Key WIN_ICO_HELP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_ICO_00))
            std::cout << "[Key]Key WIN_ICO_00 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_ICO_CLEAR))
            std::cout << "[Key]Key WIN_ICO_CLEAR down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_RESET))
            std::cout << "[Key]Key WIN_OEM_RESET down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_JUMP))
            std::cout << "[Key]Key WIN_OEM_JUMP down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_PA1))
            std::cout << "[Key]Key WIN_OEM_PA1 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_PA2))
            std::cout << "[Key]Key WIN_OEM_PA2 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_PA3))
            std::cout << "[Key]Key WIN_OEM_PA3 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_WSCTRL))
            std::cout << "[Key]Key WIN_OEM_WSCTRL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_CUSEL))
            std::cout << "[Key]Key WIN_OEM_CUSEL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_ATTN))
            std::cout << "[Key]Key WIN_OEM_ATTN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_FINISH))
            std::cout << "[Key]Key WIN_OEM_FINISH down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_COPY))
            std::cout << "[Key]Key WIN_OEM_COPY down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_AUTO))
            std::cout << "[Key]Key WIN_OEM_AUTO down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_ENLW))
            std::cout << "[Key]Key WIN_OEM_ENLW down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_BACKTAB))
            std::cout << "[Key]Key WIN_OEM_BACKTAB down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ATTN))
            std::cout << "[Key]Key ATTN down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::CRSEL))
            std::cout << "[Key]Key CRSEL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::EXSEL))
            std::cout << "[Key]Key EXSEL down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::EREOF))
            std::cout << "[Key]Key EREOF down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::PLAY))
            std::cout << "[Key]Key PLAY down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::ZOOM))
            std::cout << "[Key]Key ZOOM down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::PA1))
            std::cout << "[Key]Key PA1 down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::WIN_OEM_CLEAR))
            std::cout << "[Key]Key WIN_OEM_CLEAR down !" << std::endl;

        // Additional keys (specific to Minko and for native support of some keys)
        if (k->keyIsDown(input::Keyboard::CONTROL_RIGHT))
            std::cout << "[Key]Key CONTROL_RIGHT down !" << std::endl;
        if (k->keyIsDown(input::Keyboard::SHIFT_RIGHT))
            std::cout << "[Key]Key SHIFT_RIGHT down !" << std::endl;
    });

    canvas->run();

    return 0;
}