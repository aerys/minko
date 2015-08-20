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
		class Mouse
		{
		public:
			typedef std::shared_ptr<Mouse>	Ptr;

		protected:
			std::shared_ptr<AbstractCanvas>	_canvas;

            int                             _x;
            int                             _y;

            int                             _dX;
            int                             _dY;

			bool							_leftButtonIsDown;
			bool							_rightButtonIsDown;
            bool                            _middleButtonIsDown;

			Signal<Ptr, int, int>::Ptr		_mouseMove;
			Signal<Ptr, int, int>::Ptr		_mouseWheel;
			Signal<Ptr>::Ptr				_mouseLeftButtonDown;
			Signal<Ptr>::Ptr				_mouseLeftButtonUp;
            Signal<Ptr>::Ptr                _mouseLeftClick;
			Signal<Ptr>::Ptr				_mouseRightButtonDown;
			Signal<Ptr>::Ptr				_mouseRightButtonUp;
            Signal<Ptr>::Ptr                _mouseRightClick;
			Signal<Ptr>::Ptr				_mouseMiddleButtonDown;
			Signal<Ptr>::Ptr				_mouseMiddleButtonUp;
            Signal<Ptr>::Ptr                _mouseMiddleClick;

			std::list<Any>					_slots;

            int                             _lastMouseLeftDownX;
            int                             _lastMouseLeftDownY;

            int                             _lastMouseRightDownX;
            int                             _lastMouseRightDownY;

            int                             _lastMouseMiddleDownX;
            int                             _lastMouseMiddleDownY;

		public:
			inline static
			Ptr
			create(std::shared_ptr<AbstractCanvas> canvas)
			{
				return std::shared_ptr<Mouse>(new Mouse(canvas));
			}

			inline
            int
			x() const
			{
				return _x;
			}

			inline
            int
			y() const
			{
				return _y;
			}

            inline
			void
            x(int x)
			{
				_x = x;
			}

			inline
			void
            y(int y)
			{
				_y = y;
			}

			inline
            int
			dX() const
			{
				return _dX;
			}

			inline
            int
			dY() const
			{
				return _dY;
			}

            inline
			void
            dX(int dX)
			{
				_dX = dX;
			}

			inline
			void
            dY(int dY)
			{
				_dY = dY;
			}

			inline
			bool
			leftButtonIsDown() const
			{
				return _leftButtonIsDown;
			}

			inline
			bool
			rightButtonIsDown() const
			{
				return _rightButtonIsDown;
			}

			inline
            bool
            middleButtonIsDown() const
            {
                return _middleButtonIsDown;
            }

            inline
			float
			normalizedX() const
			{
				return 2.f * ((float)_x / _canvas->width() - .5f);
			}

			inline
			float
			normalizedY() const
			{
				return 2.f * ((float)_y / _canvas->height() - .5f);
			}

			inline
			std::shared_ptr<Signal<Ptr, int, int>>
			move()
			{
				return _mouseMove;
			}

			inline
			std::shared_ptr<Signal<Ptr, int, int>>
			wheel()
			{
				return _mouseWheel;
			}

			inline
			std::shared_ptr<Signal<Ptr>>
			leftButtonDown()
			{
				return _mouseLeftButtonDown;
			}

			inline
			std::shared_ptr<Signal<Ptr>>
			leftButtonUp()
			{
				return _mouseLeftButtonUp;
			}

			inline
			std::shared_ptr<Signal<Ptr>>
            leftButtonClick()
            {
                return _mouseLeftClick;
            }

            inline
            std::shared_ptr<Signal<Ptr>>
			rightButtonDown()
			{
				return _mouseRightButtonDown;
			}

			inline
			std::shared_ptr<Signal<Ptr>>
			rightButtonUp()
			{
				return _mouseRightButtonUp;
			}

			inline
			std::shared_ptr<Signal<Ptr>>
            rightButtonClick()
            {
                return _mouseRightClick;
            }

            inline
            std::shared_ptr<Signal<Ptr>>
			middleButtonDown()
			{
				return _mouseMiddleButtonDown;
			}

			inline
			std::shared_ptr<Signal<Ptr>>
			middleButtonUp()
			{
				return _mouseMiddleButtonUp;
			}

            inline
            std::shared_ptr<Signal<Ptr>>
            middleButtonClick()
            {
                return _mouseMiddleClick;
            }

			virtual
			~Mouse()
			{
			}

		protected:

            static const int CLICK_MOVE_THRESHOLD;

			Mouse(std::shared_ptr<AbstractCanvas> canvas);
		};
	}
}
