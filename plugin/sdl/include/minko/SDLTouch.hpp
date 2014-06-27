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

#include "minko/Canvas.hpp"
#include "minko/input/Finger.hpp"

namespace minko
{
    class Canvas;

    class SDLFinger :
    public input::Finger
    {
        friend class Canvas;
        
    public :
        static
        std::shared_ptr<SDLFinger>
        create(std::shared_ptr<Canvas> canvas);
        
        void
        fingerId(int fingerId)
        {
            _fingerId = fingerId;
        }
        
        void
        x(float x)
        {
            _x = float(x);
        }
        
        void
        y(float y)
        {
            _y = float(y);
        }
        
        void
        dx(float dx)
        {
            _dx = float(dx);
        }
        
        void
        dy(float dy)
        {
            _dy = float(dy);
        }
        
    private:
        SDLFinger(std::shared_ptr<Canvas> canvas);
        
    public:
        static const float SWIPE_PRECISION;
    };
}