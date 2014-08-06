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

#include "minko/input/leap/Gesture.hpp"

namespace Leap
{
    class Controller;
    class Frame;
    class Listener;
}

namespace minko
{
    namespace input
    {
        namespace leap
        {
            class Frame;

            class Controller: public std::enable_shared_from_this<Controller>
            {
            public:
                typedef std::shared_ptr<Controller>        Ptr;

            private:
                class LeapToMinkoDispatcher;

            private:
                AbstractCanvas::Ptr                            _canvas;
                std::shared_ptr<Leap::Controller>            _leapController;

                std::shared_ptr<Signal<Ptr>>                _initialized;
                std::shared_ptr<Signal<Ptr>>                _connected;
                std::shared_ptr<Signal<Ptr>>                _enterFrame;
                std::shared_ptr<Signal<Ptr>>                _gainedFocus;
                std::shared_ptr<Signal<Ptr>>                _lostFocus;
                std::shared_ptr<Signal<Ptr>>                _omit;
                std::shared_ptr<Signal<Ptr>>                _disconnected;
                std::shared_ptr<Signal<Ptr>>                _exit;

                std::shared_ptr<LeapToMinkoDispatcher>        _leapListener;

            public:
                inline static
                Ptr
                create(AbstractCanvas::Ptr canvas)
                {
                    Ptr ptr (new Controller(canvas));

                    ptr->initialize();

                    return ptr;
                }

                void
                start();

                std::shared_ptr<Frame>
                frame(int history = 0) const;

                void
                enableGesture(Gesture::Type, bool enable = true) const;

                bool
                isGestureEnabled(Gesture::Type) const;

                inline
                std::shared_ptr<Signal<Ptr>>
                initialized() const
                {
                    return _initialized;
                }

                inline
                std::shared_ptr<Signal<Ptr>>
                connected() const
                {
                    return _connected;
                }

                inline
                std::shared_ptr<Signal<Ptr>>
                enterFrame() const
                {
                    return _enterFrame;
                }

                inline
                std::shared_ptr<Signal<Ptr>>
                gainedFocus() const
                {
                    return _gainedFocus;
                }

                inline
                std::shared_ptr<Signal<Ptr>>
                lostFocus() const
                {
                    return _lostFocus;
                }

                inline
                std::shared_ptr<Signal<Ptr>>
                omit() const
                {
                    return _omit;
                }

                inline
                std::shared_ptr<Signal<Ptr>>
                disconnected() const
                {
                    return _disconnected;
                }

                inline
                std::shared_ptr<Signal<Ptr>>
                exit() const
                {
                    return _exit;
                }

            private:
                Controller(AbstractCanvas::Ptr);

                void
                initialize();
            };
        }
    }
}
