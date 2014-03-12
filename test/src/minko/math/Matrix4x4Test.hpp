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

#include "minko/Minko.hpp"

#include "gtest/gtest.h"

#include "minko/math/Vector3.hpp"
#include "minko/math/Matrix4x4.hpp"

namespace minko
{
	namespace math
	{
		class Matrix4x4Test :
			public ::testing::Test
		{
		public:
			static inline
			float
			random(float max = 1000.f)
			{
				return max * rand() / (float)RAND_MAX;
			}

			static inline
			std::shared_ptr<Vector3>
			randomVector3(float max = 1000.f)
			{
				return Vector3::create(random(max), random(max), random(max));
			}

			static inline
			std::shared_ptr<Matrix4x4>
			randomMatrix(float max = 1000.f)
			{
				return Matrix4x4::create()->initialize(
					random(max), random(max), random(max), random(max),
					random(max), random(max), random(max), random(max),
					random(max), random(max), random(max), random(max),
					random(max), random(max), random(max), random(max)
				);
			}

			static inline
			bool
			nearEqual(float x, float y, float epsilon = 1e-6f)
			{
				return fabsf(x - y) < epsilon;
			}

			static inline
			bool
			nearEqual(std::shared_ptr<Matrix4x4> m1, std::shared_ptr<Matrix4x4> m2, float epsilon = 1e-6f)
			{
				for (auto i = 0; i < 16; ++i)
					if (!nearEqual(m1->data()[i], m2->data()[i], epsilon))
						return false;
				return true;
			}
		};
	}
}
