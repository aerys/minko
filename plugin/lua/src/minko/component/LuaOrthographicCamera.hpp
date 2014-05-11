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

#include "minko/component/OrthographicCamera.hpp"
#include "minko/math/Matrix4x4.hpp"

#include "minko/LuaWrapper.hpp"

namespace minko
{
	namespace component
	{
		class LuaOrthographicCamera :
			public LuaWrapper
		{
		public:
			static
			void
			bind(LuaGlue& state)
			{
				state.Class<OrthographicCamera>("OrthographicCamera")
					.method("create",					static_cast<OrthographicCamera::Ptr(*)(float, float, float, float, float, float)>(&OrthographicCamera::create))
					.method("createWithPostProjection", static_cast<OrthographicCamera::Ptr(*)(float, float, float, float, float, float, math::Matrix4x4::Ptr)>(&OrthographicCamera::create))
					.method("updateProjection",			&OrthographicCamera::updateProjection)
					.property("left",					&OrthographicCamera::left, &OrthographicCamera::left)
					.property("right",					&OrthographicCamera::right, &OrthographicCamera::right)
					.property("top",					&OrthographicCamera::top, &OrthographicCamera::top)
					.property("bottom",					&OrthographicCamera::bottom, &OrthographicCamera::bottom)
					.property("zNear",					&OrthographicCamera::zNear, &OrthographicCamera::zNear)
					.property("zFar",					&OrthographicCamera::zFar, &OrthographicCamera::zFar);
			}
		};
	}
}
