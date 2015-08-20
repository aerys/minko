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

#include "minko/input/Touch.hpp"

namespace minko
{
    class Canvas;

    namespace input
    {
        class SDLTouch :
            public Touch
        {
            friend Canvas;

        public :
            static
            std::shared_ptr<SDLTouch>
            create(std::shared_ptr<Canvas> canvas);
        
            inline
            void addTouch(int identifier, float x, float y, float dX, float dY)
            {
                if (_touches.find(identifier) != _touches.end())
                {
                    updateTouch(identifier, x, y, dX, dY);
                }
                else
                {
                    _identifiers.push_back(identifier);

                    _touches[identifier] = {x, y, dX, dY};
                }
            }

            inline
            void updateTouch(int identifier, float x, float y, float dX, float dY)
            {
                if (_touches.find(identifier) == _touches.end())
                {
                    addTouch(identifier, x, y, dX, dY);
                }
                else
                {
                    _touches[identifier].x = x;
                    _touches[identifier].y = y;
                    _touches[identifier].dX = dX;
                    _touches[identifier].dY = dY;
                }
            }

            inline
            void removeTouch(int identifier)
            {
                if (_touches.find(identifier) != _touches.end())
                {
                    _touches.erase(identifier);
                    auto it = std::find(_identifiers.begin(), _identifiers.end(), identifier);

                    if (it != _identifiers.end())
                        _identifiers.erase(it);
                }
            }

            inline
            void lastTouchDownX(float v)
            {
                _lastTouchDownX = v;
            }

            inline
            void lastTouchDownY(float v)
            {
                _lastTouchDownY = v;
            }

            inline
            void lastTapX(float v)
            {
                _lastTapX = v;
            }

            inline
            void lastTapY(float v)
            {
                _lastTapY = v;
            }

            inline
            void lastTapTime(float v)
            {
                _lastTapTime = v;
            }

            inline
            void lastTouchDownTime(float v)
            {
                _lastTouchDownTime = v;
            }

            inline
            float lastTouchDownX()
            {
                return _lastTouchDownX;
            }

            inline
            float lastTouchDownY()
            {
                return _lastTouchDownY;
            }

            inline
            float lastTouchDownTime()
            {
                return _lastTouchDownTime;
            }

            inline
            float lastTapX()
            {
                return _lastTapX;
            }

            inline
            float lastTapY()
            {
                return _lastTapY;
            }

            inline
            float lastTapTime()
            {
                return _lastTapTime;
            }

        private:
            SDLTouch(std::shared_ptr<Canvas> canvas);

            static const float TAP_MOVE_THRESHOLD;
            static const float TAP_DELAY_THRESHOLD;
            static const float DOUBLE_TAP_DELAY_THRESHOLD;
            static const float LONG_HOLD_DELAY_THRESHOLD;

            float _lastTouchDownX;
            float _lastTouchDownY;
            float _lastTouchDownTime;

            float _lastTapX;
            float _lastTapY;
            float _lastTapTime;
        public:

            static const float SWIPE_PRECISION;
        };
    }
}
