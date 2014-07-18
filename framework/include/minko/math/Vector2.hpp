/*
Copyright (c) 2013 Aerys

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
#include "minko/math/Convertible.hpp"

namespace minko
{
	namespace math
	{
		class Vector2 :
			public std::enable_shared_from_this<Vector2>,
			public Convertible<Vector2>
		{
		public:
			typedef std::shared_ptr<Vector2>	Ptr;

		protected:
			float _x;
			float _y;

		public:
			inline static
			Ptr
			create(float x = 0.f, float y = 0.f)
			{
				return std::shared_ptr<Vector2>(new Vector2(x, y));
			}

			inline static
			Ptr
			create(Ptr value)
			{
				return create(value->x(), value->y());
			}

			inline static
			Ptr
			create(float* data)
			{
				return create(*data, *(data + 1));
			}

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
			void
			x(float x)
			{
				_x = x;
			}

			inline
			void
			y(float y)
			{
				_y = y;
			}

           	inline
			Ptr
			copyFrom(Ptr value)
			{
				return setTo(value->_x, value->_y);
			}

			inline
			Ptr
			copyFrom(float* data)
			{
				return setTo(*data, *(data + 1));
			}

			inline
			Ptr
			setTo(float x, float y)
			{
				_x = x;
				_y = y;

				return std::static_pointer_cast<Vector2>(shared_from_this());
			}

			std::string
			toString()
			{
				std::stringstream stream;
				
				stream << "(" << _x << ", " << _y << ")";

				return stream.str();
			}

			Ptr
			normalize()
			{
				float l = sqrtf(_x * _x + _y * _y);
				
				if (l != 0.)
				{
					_x /= l;
					_y /= l;
				}

				return shared_from_this();
			}

			inline
			Ptr
			cross(Ptr value)
			{
				float x = _y * value->_x - _x * value->_y;
				float y = _x * value->_y - _y * value->_x;

				_x = x;
				_y = y;

				return std::static_pointer_cast<Vector2>(shared_from_this());
			}

			inline
			float
			dot(Ptr value) const
			{
				return _x * value->_x + _y * value->_y;
			}

			inline
			Ptr
			operator-()
			{
				return create(-_x, -_y);
			}

			inline
			Ptr
			operator-(Ptr value)
			{
				return create(_x - value->_x, _y - value->_y);
			}

			inline
			Ptr
			operator+(Ptr value)
			{
				return create(_x + value->_x, _y + value->_y);
			}

			inline
			Ptr
			operator+=(Ptr value)
			{
				_x += value->_x;
				_y += value->_y;

				return std::static_pointer_cast<Vector2>(shared_from_this());
			}

			inline
			Ptr
			operator-=(Ptr value)
			{
				_x -= value->_x;
				_y -= value->_y;

				return std::static_pointer_cast<Vector2>(shared_from_this());
			}

			inline
			Ptr
			lerp(Ptr target, float ratio)
			{
				return setTo(_x + (target->_x - _x) * ratio, _y + (target->_y - _y) * ratio);
			}

			inline
			Ptr
			scaleBy(float scale)
			{
				_x *= scale;
				_y *= scale;

				return std::static_pointer_cast<Vector2>(shared_from_this());
			}
			
			bool
			equals(std::shared_ptr<Vector2> vect2)
			{
				return vect2->x() == _x && 
					   vect2->y() == _y;
			}

		protected:
			Vector2(float x, float y) :
				_x(x),
				_y(y)
			{
			}
		};
	}
}
