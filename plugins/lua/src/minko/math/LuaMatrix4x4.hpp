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

#include "minko/LuaWrapper.hpp"

namespace minko
{
	namespace math
	{
		class LuaMatrix4x4
		{
		public:
			static
			void
			bind(LuaGlue& state)
			{
				state.Class<math::Matrix4x4>("Matrix4x4")
		            .method("create",               static_cast<math::Matrix4x4::Ptr (*)()>(&math::Matrix4x4::create))
		            .method("lookAt",               &math::Matrix4x4::lookAt)
		            .method("identity",             &math::Matrix4x4::identity)
		            .method("appendRotationX",      &math::Matrix4x4::appendRotationX)
		            .method("appendRotationY",      &math::Matrix4x4::appendRotationY)
		            .method("appendRotationZ",      &math::Matrix4x4::appendRotationZ)
		            .method("appendRotation",       &math::Matrix4x4::appendRotation)
		            .method("appendTranslation",    static_cast<math::Matrix4x4::Ptr (math::Matrix4x4::*)(float, float, float)>(&math::Matrix4x4::appendTranslation))
		            .method("appendTranslation",    static_cast<math::Matrix4x4::Ptr(math::Matrix4x4::*)(math::Vector3::Ptr)>(&math::Matrix4x4::appendTranslation))
		            .method("prependRotationX",     &math::Matrix4x4::prependRotationX)
		            .method("prependRotationY",     &math::Matrix4x4::prependRotationY)
		            .method("prependRotationZ",     &math::Matrix4x4::prependRotationZ)
		            .method("prependRotation",      &math::Matrix4x4::prependRotation)
		            .method("prependTranslation",   static_cast<math::Matrix4x4::Ptr(math::Matrix4x4::*)(float, float, float)>(&math::Matrix4x4::prependTranslation))
		            .method("prependTranslation",   static_cast<math::Matrix4x4::Ptr(math::Matrix4x4::*)(math::Vector3::Ptr)>(&math::Matrix4x4::prependTranslation));
			}
		};
	}
}
