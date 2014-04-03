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
		class Finger
		{
		public:
			typedef std::shared_ptr<Finger> Ptr;

		protected:
			std::shared_ptr<AbstractCanvas> _canvas;
            
            float                                _x;
			float                                _y;
            float                                _dx;
			float                                _dy;
            
			Signal<Ptr, float, float>::Ptr		_fingerMotion; // dx, dy
			Signal<Ptr, float, float>::Ptr		_fingerDown; // x, y
			Signal<Ptr, float, float>::Ptr		_fingerUp; // x, y
            
            // Gestures
            Signal<Ptr>::Ptr                    _swipeRight;
            Signal<Ptr>::Ptr                    _swipeLeft;
            Signal<Ptr>::Ptr                    _swipeUp;
            Signal<Ptr>::Ptr                    _swipeDown;

		public:
            inline
			float
			x()
			{
				return _x;
			}
            
			inline
			float
			y()
			{
				return _y;
			}
            
            inline
			float
			dx()
			{
				return _dx;
			}
            
			inline
			float
			dy()
			{
				return _dy;
			}

			inline
			Signal<Ptr, float, float>::Ptr
			fingerMotion()
			{
				return _fingerMotion;
			}

			inline
			Signal<Ptr, float, float>::Ptr
			fingerDown()
			{
				return _fingerDown;
			}

			inline
			Signal<Ptr, float, float>::Ptr
			fingerUp()
			{
				return _fingerUp;
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
            

		protected:
			Finger(std::shared_ptr<AbstractCanvas> canvas);
		};
	}
}
