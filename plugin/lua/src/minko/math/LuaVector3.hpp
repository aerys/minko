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
		class LuaVector3 :
			public LuaWrapper
		{
		public:
			static
			void
			bind(LuaGlue& state)
			{
		        state.Class<Vector3>("Vector3")
		            .method("create",		static_cast<Vector3::Ptr (*)(float, float, float)>(&Vector3::create))
		            .method("toString",		&Vector3::toString)
		            .method("setTo",		&Vector3::setTo)
		            .method("copyFrom",		static_cast<Vector3::Ptr (Vector3::*)(Vector3::Ptr)>(&Vector3::copyFrom))
					.method("scaleBy",		&Vector3::scaleBy)
					.method("subtract",		&Vector3::subtract)
					.method("add",		static_cast<Vector3::Ptr (Vector3::*)(Vector3::Ptr)>(&Vector3::add))
		            .method("zero",			&Vector3::zero)
		            .method("one",			&Vector3::one)
		            .method("up",			&Vector3::up)
		            .method("forward",		&Vector3::forward)
		            .method("xAxis",		&Vector3::xAxis)
		            .method("yAxis",		&Vector3::yAxis)
		            .method("zAxis",		&Vector3::zAxis)
		            .method("max",     		&Vector3::max)
		            .method("min",     		&Vector3::min)
					.method("length",		&Vector3::length)
					.method("normalize",	&Vector3::normalize)
		            .property("x",			static_cast<float (Vector3::*)(void)>(&Vector3::x), static_cast<void (Vector3::*)(float)>(&Vector3::x))
		            .property("y",			static_cast<float (Vector3::*)(void)>(&Vector3::y), static_cast<void (Vector3::*)(float)>(&Vector3::y))
		            .property("z",			static_cast<float (Vector3::*)(void)>(&Vector3::z), static_cast<void (Vector3::*)(float)>(&Vector3::z));
			}
		};
	}
}
