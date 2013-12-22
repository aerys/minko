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

#include "minko/math/Vector3.hpp"

#include "minko/LuaWrapper.hpp"

namespace minko
{
	namespace math
	{
		class LuaVector3
		{
		public:
			static
			void
			bind(LuaGlue& state)
			{
		        state.Class<math::Vector3>("Vector3")
		            .method("create",   static_cast<math::Vector3::Ptr (*)(float, float, float)>(&math::Vector3::create))
		            .method("toString", &math::Vector3::toString)
		            .method("zero",     &math::Vector3::zero)
		            .method("one",      &math::Vector3::one)
		            .method("up",       &math::Vector3::up)
		            .method("forward",  &math::Vector3::forward)
		            .method("xAxis",    &math::Vector3::xAxis)
		            .method("yAxis",    &math::Vector3::yAxis)
		            .method("zAxis",    &math::Vector3::zAxis)
		            .property("x",      static_cast<float (math::Vector3::*)(void)>(&math::Vector3::x), static_cast<void (math::Vector3::*)(float)>(&math::Vector3::x))
		            .property("y",      static_cast<float (math::Vector3::*)(void)>(&math::Vector3::y), static_cast<void (math::Vector3::*)(float)>(&math::Vector3::y))
		            .property("z",      static_cast<float (math::Vector3::*)(void)>(&math::Vector3::z), static_cast<void (math::Vector3::*)(float)>(&math::Vector3::z));
			}
		};
	}
}
