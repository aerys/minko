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
		class Frustum :
			public AbstractShape
		{

		public:
			typedef std::shared_ptr<Frustum> Ptr;

		private:
			std::vector<math::vec3>			_points;
			std::array<math::vec4, 6> 		_planes;
			
			std::array<bool, 6> 			_blfResult;
			std::array<bool, 6> 			_blbResult;
			std::array<bool, 6> 			_brfResult;
			std::array<bool, 6> 			_brbResult;
			std::array<bool, 6> 			_tlfResult;
			std::array<bool, 6> 			_tlbResult;
			std::array<bool, 6> 			_trfResult;
			std::array<bool, 6> 			_trbResult;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<Frustum>(new Frustum());
			}

			bool
			cast(std::shared_ptr<Ray> ray, float& distance);

			void
			updateFromMatrix(const math::mat4& matrix);

			std::pair<ShapePosition, unsigned int>
			testBoundingBox(std::shared_ptr<math::Box> box, unsigned int basePlaneId);

			ShapePosition
			testBoundingBox(std::shared_ptr<math::Box> box);

		private:
			Frustum() = default;
		};
	}
}

