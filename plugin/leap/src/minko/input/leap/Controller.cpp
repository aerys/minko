/*
Copyright (c) $year$ Aerys

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


#include "LeapPimpl.hpp" // voluntarily not pointed to by the includedirs
#include "minko/input/leap/Controller.hpp"
#include "minko/input/leap/Frame.hpp"

using namespace minko;
using namespace minko::input;
using namespace minko::input::leap;

class Controller::LeapToMinkoDispatcher: public Leap::Listener
{
private:
    Controller::Ptr    _controller;

public:
    LeapToMinkoDispatcher(Controller::Ptr controller):
        Leap::Listener(),
        _controller(controller)
    {
        if (_controller == nullptr)
            throw std::invalid_argument("controller");
    }

    virtual
    void
    onInit(const Leap::Controller&)
    {
        _controller->initialized()->execute(_controller);
    }

    virtual
    void
    onConnect(const Leap::Controller&)
    {
        _controller->connected()->execute(_controller);
    }

    virtual
    void
    onFrame(const Leap::Controller&)
    {
        _controller->enterFrame()->execute(_controller);
    }

    virtual
    void
    onFocusGained(const Leap::Controller&)
    {
        _controller->gainedFocus()->execute(_controller);
    }

    virtual
    void
    onFocusLost(const Leap::Controller&)
    {
        _controller->lostFocus()->execute(_controller);
    }

    /*
    virtual
    void
    onOmit(const Leap::Controller&)
    {
        _controller->omit()->execute(_controller);
    }
    */

    virtual
    void
    onDisconnect(const Leap::Controller&)
    {
        _controller->disconnected()->execute(_controller);
    }

    virtual
    void
    onExit(const Leap::Controller&)
    {
        _controller->exit()->execute(_controller);
    }
};



Controller::Controller(AbstractCanvas::Ptr canvas):
    _canvas(canvas),
    _leapController(new Leap::Controller()),
    _initialized    (Signal<Controller::Ptr>::create()),
    _connected        (Signal<Controller::Ptr>::create()),
    _enterFrame        (Signal<Controller::Ptr>::create()),
    _gainedFocus    (Signal<Controller::Ptr>::create()),
    _lostFocus        (Signal<Controller::Ptr>::create()),
    _omit            (Signal<Controller::Ptr>::create()),
    _disconnected    (Signal<Controller::Ptr>::create()),
    _exit            (Signal<Controller::Ptr>::create()),
    _leapListener(nullptr)
{
    if (_canvas == nullptr)
        throw std::invalid_argument("_canvas");
}

void
Controller::initialize()
{
    _leapListener = std::shared_ptr<LeapToMinkoDispatcher>(new LeapToMinkoDispatcher(shared_from_this()));
}

void
Controller::start()
{
    _leapController->addListener(*_leapListener);
}

void
Controller::enableGesture(Gesture::Type value, bool enable) const
{
    auto type = convert(value);

    if (type != Leap::Gesture::Type::TYPE_INVALID)
        _leapController->enableGesture(type);
}

bool
Controller::isGestureEnabled(Gesture::Type value) const
{
    auto type = convert(value);

    return type != Leap::Gesture::Type::TYPE_INVALID
    ? _leapController->isGestureEnabled(type)
    : false;
}

std::shared_ptr<Frame>
Controller::frame(int history) const
{
    return std::shared_ptr<Frame>(new Frame(_leapController->frame(history)));
}


