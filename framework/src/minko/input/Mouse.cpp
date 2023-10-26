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

#include "minko/input/Mouse.hpp"

using namespace minko;
using namespace minko::input;

const int Mouse::CLICK_MOVE_THRESHOLD = 5;

Mouse::Mouse(std::shared_ptr<AbstractCanvas> canvas) :
    _canvas(canvas),
    _x(0),
    _y(0),
    _dX(0),
    _dY(0),
    _leftButtonIsDown(false),
    _rightButtonIsDown(false),
    _middleButtonIsDown(false),
    _mouseMove(Signal<Ptr, int, int>::create()),
    _mouseWheel(Signal<Ptr, int, int>::create()),
    _mouseLeftButtonDown(Signal<Ptr>::create()),
    _mouseLeftButtonUp(Signal<Ptr>::create()),
    _mouseLeftClick(Signal<Ptr>::create()),
    _mouseLeftDoubleClick(Signal<Ptr>::create()),
    _mouseRightButtonDown(Signal<Ptr>::create()),
    _mouseRightButtonUp(Signal<Ptr>::create()),
    _mouseRightClick(Signal<Ptr>::create()),
    _mouseRightDoubleClick(Signal<Ptr>::create()),
    _mouseMiddleButtonDown(Signal<Ptr>::create()),
    _mouseMiddleButtonUp(Signal<Ptr>::create()),
    _mouseMiddleClick(Signal<Ptr>::create())
{
    _slots.push_front(_mouseLeftButtonDown->connect([&](Ptr mouse)
    {
        _leftButtonIsDown = true;

        // Click logic is now handled in the app in `InputStateHandlerScript`.
        // FIXME: This code should be removed once input events come from
        //        the UI on native platforms, like on Android and HTML5.
        //        See https://git.aerys.in/aerys/smartshape/smartshape-app/-/issues/1969.
#if MINKO_PLATFORM == MINKO_PLATFORM_LINUX || MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS
        _lastMouseLeftDownX = x();
        _lastMouseLeftDownY = y();
#endif
    }));
    _slots.push_front(_mouseLeftButtonUp->connect([&](Ptr mouse)
    {
        _leftButtonIsDown = false;

        // Click logic is now handled in the app in `InputStateHandlerScript`.
        // FIXME: This code should be removed once input events come from
        //        the UI on native platforms, like on Android and HTML5.
        //        See https://git.aerys.in/aerys/smartshape/smartshape-app/-/issues/1969.
#if MINKO_PLATFORM == MINKO_PLATFORM_LINUX || MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS
        auto dX = std::abs(x() - _lastMouseLeftDownX);
        auto dY = std::abs(y() - _lastMouseLeftDownY);

        if (dX < CLICK_MOVE_THRESHOLD && dY < CLICK_MOVE_THRESHOLD)
            leftButtonClick()->execute(mouse);
#endif
    }));

    _slots.push_front(_mouseRightButtonDown->connect([&](Ptr mouse)
    {
        _rightButtonIsDown = true;

        // Click logic is now handled in the app in `InputStateHandlerScript`.
        // FIXME: This code should be removed once input events come from
        //        the UI on native platforms, like on Android and HTML5.
        //        See https://git.aerys.in/aerys/smartshape/smartshape-app/-/issues/1969.
#if MINKO_PLATFORM == MINKO_PLATFORM_LINUX || MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS
        _lastMouseRightDownX = x();
        _lastMouseRightDownY = y();
#endif
    }));
    _slots.push_front(_mouseRightButtonUp->connect([&](Ptr mouse)
    {
        _rightButtonIsDown = false;

        // Click logic is now handled in the app in `InputStateHandlerScript`.
        // FIXME: This code should be removed once input events come from
        //        the UI on native platforms, like on Android and HTML5.
        //        See https://git.aerys.in/aerys/smartshape/smartshape-app/-/issues/1969.
#if MINKO_PLATFORM == MINKO_PLATFORM_LINUX || MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS
        auto dX = std::abs(x() - _lastMouseRightDownX);
        auto dY = std::abs(y() - _lastMouseRightDownY);

        if (dX < CLICK_MOVE_THRESHOLD && dY < CLICK_MOVE_THRESHOLD)
            rightButtonClick()->execute(mouse);
#endif
    }));
    
    _slots.push_front(_mouseMiddleButtonDown->connect([&](Ptr mouse)
    {
        _middleButtonIsDown = true;

        // Click logic is now handled in the app in `InputStateHandlerScript`.
        // FIXME: This code should be removed once input events come from
        //        the UI on native platforms, like on Android and HTML5.
        //        See https://git.aerys.in/aerys/smartshape/smartshape-app/-/issues/1969.
#if MINKO_PLATFORM == MINKO_PLATFORM_LINUX || MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS
        _lastMouseMiddleDownX = x();
        _lastMouseMiddleDownY = y();
#endif
    }));
    _slots.push_front(_mouseMiddleButtonUp->connect([&](Ptr mouse)
    {
        _middleButtonIsDown = false;

        // Click logic is now handled in the app in `InputStateHandlerScript`.
        // FIXME: This code should be removed once input events come from
        //        the UI on native platforms, like on Android and HTML5.
        //        See https://git.aerys.in/aerys/smartshape/smartshape-app/-/issues/1969.
#if MINKO_PLATFORM == MINKO_PLATFORM_LINUX || MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS
        auto dX = std::abs(x() - _lastMouseMiddleDownX);
        auto dY = std::abs(y() - _lastMouseMiddleDownY);

        if (dX < CLICK_MOVE_THRESHOLD && dY < CLICK_MOVE_THRESHOLD)
            middleButtonClick()->execute(mouse);
#endif
    }));
}
