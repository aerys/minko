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

#include "minko/math/AbstractShape.hpp"
#include "minko/math/Vector3.hpp"

namespace minko
{
	namespace math
	{
		class Box :
			public AbstractShape
		{
		public:
			typedef std::shared_ptr<Box>	Ptr;

		private:
			std::shared_ptr<Vector3>	_topRight;
			std::shared_ptr<Vector3>	_bottomLeft;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<Box>(new Box());
			}

			inline static
			Ptr
			create(std::shared_ptr<math::Vector3> topRight, std::shared_ptr<math::Vector3> bottomLeft)
			{
				auto box = std::shared_ptr<Box>(new Box());

				box->_topRight->copyFrom(topRight);
				box->_bottomLeft->copyFrom(bottomLeft);

				return box;
			}

			static
			Ptr
			merge(Ptr box1, Ptr box2, Ptr out = nullptr)
			{
				if (out == nullptr)
					out = create();

				out->topRight()->setTo(
					std::max(box1->_topRight->x(), box2->_topRight->x()),
					std::max(box1->_topRight->y(), box2->_topRight->y()),
					std::max(box1->_topRight->z(), box2->_topRight->z())
				);

				out->bottomLeft()->setTo(
					std::min(box1->_bottomLeft->x(), box2->_bottomLeft->x()),
					std::min(box1->_bottomLeft->y(), box2->_bottomLeft->y()),
					std::min(box1->_bottomLeft->z(), box2->_bottomLeft->z())
				);

				return out;
			}

			inline
			std::shared_ptr<Vector3>
			topRight() const
			{
				return _topRight;
			}

			inline
			std::shared_ptr<Vector3>
			bottomLeft() const
			{
				return _bottomLeft;
			}

			inline
			float
			width() const
			{
				return _topRight->x() - _bottomLeft->x();
			}

			inline
			float
			height() const
			{
				return _topRight->y() - _bottomLeft->y();
			}

			inline
			float
			depth() const
			{
				return _topRight->z() - _bottomLeft->z();
			}

			bool
			cast(std::shared_ptr<Ray> ray, float& distance);

			std::array<std::shared_ptr<Vector3>, 8>
			getVertices();

		private:
			Box();
		};
	}
}