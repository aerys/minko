#pragma once

#include "minko/Common.hpp"
#include "minko/math/Vector3.hpp"

namespace minko
{
	namespace math
	{
		class Vector4 :
			public Vector3
		{
		public:
			typedef std::shared_ptr<Vector4>	Ptr;

		protected:
			float _w;

		public:
			inline static
			Ptr
			create(float x, float y, float z, float w = 1.)
			{
				return std::shared_ptr<Vector4>(new Vector4(x, y, z, w));
			}

			inline
			float
			w()
			{
				return _w;
			}

			inline
			void
			w(float w)
			{
				_w = w;
			}

			inline
			Ptr
			copyFrom(Ptr value)
			{
				return setTo(value->_x, value->_y, value->_z, value->_w);
			}

			inline
			Ptr
			setTo(float x, float y, float z, float w)
			{
				_x = x;
				_y = y;
				_z = z;
				_w = w;

				return std::static_pointer_cast<Vector4>(shared_from_this());
			}	

			inline
			Ptr
			normalize()
			{
				float l = sqrtf(_x * _x + _y * _y + _z * _z + _w * _w);

				if (l != 0.)
				{
					_x /= l;
					_y /= l;
					_z /= l;
					_w /= l;
				}

				return std::static_pointer_cast<Vector4>(shared_from_this());
			}

			inline
			Ptr
			cross(Ptr value)
			{
				float x = _y * value->_z - _z * value->_y;
				float y = _z * value->_w - _w * value->_z;
				float z = _w * value->_x - _x * value->_w;
				float w = _x * value->_y - _y * value->_x;

				_x = x;
				_y = y;
				_z = z;
				_w = w;

				return std::static_pointer_cast<Vector4>(shared_from_this());
			}

			inline
			float
			dot(Ptr value)
			{
				return _x * value->_x + _y * value->_y + _z * value->_z + _w * value->_w;
			}

			inline
			Ptr
			operator-()
			{
				return create(-_x, -_y, -_z, -_w);
			}

			inline
			Ptr
			operator-(Ptr value)
			{
				return create(_x - value->_x, _y - value->_y, _z - value->_z, _w - value->_w);
			}

			inline
			Ptr
			operator+(Ptr value)
			{
				return create(_x + value->_x, _y + value->_y, _z + value->_z, _w + value->_w);
			}

			inline
			Ptr
			operator+=(Ptr value)
			{
				_x += value->_x;
				_y += value->_y;
				_z += value->_z;
				_w += value->_w;

				return std::static_pointer_cast<Vector4>(shared_from_this());
			}

			inline
			Ptr
			operator-=(Ptr value)
			{
				_x -= value->_x;
				_y -= value->_y;
				_z -= value->_z;
				_w -= value->_w;

				return std::static_pointer_cast<Vector4>(shared_from_this());
			}

			inline
			Ptr
			lerp(Ptr target, float ratio)
			{
				return setTo(
					_x + (target->_x - _x) * ratio,
					_y + (target->_y - _y) * ratio,
					_z + (target->_z - _z) * ratio,
					_w + (target->_w - _w) * ratio
				);
			}

		protected:
			Vector4(float x, float y, float z, float w) :
				Vector3(x, y, z),
				_w(w)
			{
			}
		};

	}
}
