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
#include "minko/Any.hpp"

namespace minko
{
    namespace input
    {
        class Touch
        {
        public:
            typedef std::shared_ptr<Touch>          Ptr;

            typedef struct TouchPoint {
                float x;
                float y;
                float dX;
                float dY;
            } TouchPoint;

        protected:
            std::shared_ptr<AbstractCanvas>             _canvas;
            
            std::map<int, TouchPoint>                   _touches; // identifier to x/y dx/dy

            std::vector<int>                            _identifiers;

            Signal<Ptr, int, float, float>::Ptr         _touchMove;
            Signal<Ptr, int, float, float>::Ptr         _touchDown;
            Signal<Ptr, int, float, float>::Ptr         _touchUp;

            // Gestures
            Signal<Ptr>::Ptr                            _swipeRight;
            Signal<Ptr>::Ptr                            _swipeLeft;
            Signal<Ptr>::Ptr                            _swipeUp;
            Signal<Ptr>::Ptr                            _swipeDown;
            Signal<Ptr, float>::Ptr                     _pinchZoom;
            Signal<Ptr, float, float>::Ptr              _tap;
            Signal<Ptr, float, float>::Ptr              _doubleTap;
            Signal<Ptr, float, float>::Ptr              _longHold;

        public:
            inline
            std::map<int, TouchPoint>
            touches()
            {
                return _touches;
            }

            inline
            std::vector<int>
            identifiers()
            {
                return _identifiers;
            }

            inline
            int
            numTouches()
            {
                return _identifiers.size();
            }

            inline
            TouchPoint
            touch(int identifier)
            {
                return _touches[identifier];
            }

            inline
            Signal<Ptr, int, float, float>::Ptr
            touchMove()
            {
                return _touchMove;
            }

            inline
            Signal<Ptr, int, float, float>::Ptr
            touchDown()
            {
                return _touchDown;
            }

            inline
            Signal<Ptr, int, float, float>::Ptr
            touchUp()
            {
                return _touchUp;
            }

            inline
            Signal<Ptr>::Ptr
            swipeLeft()
            {
                return _swipeLeft;
            }

            inline
            Signal<Ptr>::Ptr
            swipeRight()
            {
                return _swipeRight;
            }

            inline
            Signal<Ptr>::Ptr
            swipeUp()
            {
                return _swipeUp;
            }

            inline
            Signal<Ptr>::Ptr
            swipeDown()
            {
                return _swipeDown;
            }

            inline
            Signal<Ptr, float>::Ptr
            pinchZoom()
            {
                return _pinchZoom;
            }

            inline
            Signal<Ptr, float, float>::Ptr
            tap()
            {
                return _tap;
            }

            inline
            Signal<Ptr, float, float>::Ptr
            doubleTap()
            {
                return _doubleTap;
            }

            inline
            Signal<Ptr, float, float>::Ptr
            longHold()
            {
                return _longHold;
            }

            float
            averageX();

            float
            averageY();

            float
            averageDX();

            float
            averageDY();

            void
            resetDeltas();

        protected:
            Touch(std::shared_ptr<AbstractCanvas> canvas);
        };
    }
}
