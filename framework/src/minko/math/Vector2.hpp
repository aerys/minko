#pragma once

#include "minko/Common.hpp"

namespace minko
{
	namespace math
	{
		class Vector2 :
			public std::enable_shared_from_this<Vector2>
		{
		public:
			typedef std::shared_ptr<Vector2>	Ptr;

		protected:
			float _x;
			float _y;

		public:
			inline static
			Ptr
			create(float x, float y)
			{
				return std::shared_ptr<Vector2>(new Vector2(x, y));
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
			setTo(float x, float y)
			{
				_x = x;
				_y = y;

				return std::static_pointer_cast<Vector2>(shared_from_this());
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
			dot(Ptr value)
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

		protected:
			Vector2(float x, float y) :
				_x(x),
				_y(y)
			{
			}
		};
	}
}
