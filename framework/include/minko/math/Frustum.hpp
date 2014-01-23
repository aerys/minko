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

namespace minko
{
	namespace math
	{
		enum class FrustumPosition
		{
			OUTSIDE = -3,
			AROUND	= -2,
			INSIDE	= -1,
			LEFT	= 0,
			TOP		= 1,
			RIGHT	= 2,
			BOTTOM	= 3,
			NEAR	= 4,
			FAR		= 5
		};

		enum class PlanePosition
		{
			LEFT	= 0,
			TOP		= 1,
			RIGHT	= 2,
			BOTTOM	= 3,
			NEAR	= 4,
			FAR		= 5
		};
	}
}

namespace std
{
	template<> 
	struct hash<minko::math::FrustumPosition>
	{
	    inline
	    size_t
	    operator()(const minko::math::FrustumPosition& p) const
	    {
	        return static_cast<int>(p);
	    }
	};
}

namespace minko
{
	namespace math
	{
		class Frustum
		{

		public:
			typedef std::shared_ptr<Frustum> Ptr;

		private:
			std::vector<std::shared_ptr<math::Vector3>>							_points;
			std::array<std::shared_ptr<math::Vector4>, 6> _planes;
			
			std::array<bool, 6> _blfResult;
			std::array<bool, 6> _blbResult;
			std::array<bool, 6> _brfResult;
			std::array<bool, 6> _brbResult;
			std::array<bool, 6> _tlfResult;
			std::array<bool, 6> _tlbResult;
			std::array<bool, 6> _trfResult;
			std::array<bool, 6> _trbResult;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<Frustum>(new Frustum());
			}

			void
			updateFromMatrix(std::shared_ptr<math::Matrix4x4> matrix);

			FrustumPosition
			testBoundingBox(std::shared_ptr<math::Box> box);

		private:
			Frustum();
		};
	}
}

