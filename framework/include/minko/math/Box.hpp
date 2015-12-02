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

#include "minko/math/AbstractShape.hpp"

namespace minko
{
	namespace math
	{
		class Box :
			public AbstractShape,
			public std::enable_shared_from_this<Box>
		{
		public:
			typedef std::shared_ptr<Box>	Ptr;

		private:
			vec3	_topRight;
			vec3	_bottomLeft;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<Box>(new Box());
			}

			inline static
			Ptr
			create(const math::vec3& topRight, const math::vec3& bottomLeft)
			{
				auto box = std::shared_ptr<Box>(new Box());

				box->_topRight = topRight;
				box->_bottomLeft = bottomLeft;

				return box;
			}

			static
			Ptr
			merge(Ptr box1, Ptr box2, Ptr out = nullptr);

			Ptr
			merge(Ptr box2);
			
			inline
			const vec3&
			topRight() const
			{
				return _topRight;
			}

			inline
			void
			topRight(const vec3& topRight)
			{
				_topRight = topRight;
			}

			inline
			const vec3
			bottomLeft() const
			{
				return _bottomLeft;
			}

			inline
			void
			bottomLeft(const vec3& bottomLeft)
			{
				_bottomLeft = bottomLeft;
			}

			inline
			float
			width() const
			{
				return _topRight.x - _bottomLeft.x;
			}

			inline
			float
			height() const
			{
				return _topRight.y - _bottomLeft.y;
			}

			inline
			float
			depth() const
			{
				return _topRight.z - _bottomLeft.z;
			}

			inline
			Ptr
			copyFrom(Ptr box)
			{
				_topRight = box->_topRight;
				_bottomLeft = box->_bottomLeft;

				return shared_from_this();
			}

            float
            distance(const math::vec3& position);

			bool
			cast(std::shared_ptr<Ray> ray, float& distance);

			std::array<vec3, 8>
			getVertices();
			
			ShapePosition
			testBoundingBox(std::shared_ptr<math::Box> box);

			void
			updateFromMatrix(const math::mat4& matrix);

		private:
			Box();
		};
	}
}