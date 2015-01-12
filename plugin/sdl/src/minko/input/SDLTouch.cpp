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

#include "minko/input/SDLTouch.hpp"

#include "minko/Canvas.hpp"

using namespace minko;
using namespace minko::input;

const float SDLTouch::SWIPE_PRECISION = 0.05f;

const float SDLTouch::TAP_MOVE_THRESHOLD = 10.f;
const float SDLTouch::TAP_DELAY_THRESHOLD = 300.f;
const float SDLTouch::DOUBLE_TAP_DELAY_THRESHOLD = 400.f;
const float SDLTouch::LONG_HOLD_DELAY_THRESHOLD = 1000.f;

SDLTouch::SDLTouch(std::shared_ptr<Canvas> canvas) :
    Touch(canvas),
    _lastTouchDownTime(-1.f),
    _lastTapTime(-1.f),
    _lastTouchDownX(0.f),
    _lastTouchDownY(0.f),
    _lastTapX(0.f),
    _lastTapY(0.f)
{
}

std::shared_ptr<SDLTouch>
SDLTouch::create(std::shared_ptr<Canvas> canvas)
{
    return std::shared_ptr<SDLTouch>(new SDLTouch(canvas));
}
