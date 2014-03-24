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
		class LuaMatrix4x4 :
			public LuaWrapper
		{
		public:
			static
			void
			bind(LuaGlue& state)
			{
				state.Class<Matrix4x4>("Matrix4x4")
		            .method("create",               static_cast<Matrix4x4::Ptr (*)()>(&Matrix4x4::create))
					.method("copyFrom",				&Matrix4x4::copyFrom)
		            .method("lookAt",               &Matrix4x4::lookAt)
		            .method("identity",             &Matrix4x4::identity)
		            .method("invert",				&Matrix4x4::invert)
					.method("translation",			static_cast<Matrix4x4::Ptr (Matrix4x4::*)(float, float, float)>(&Matrix4x4::translation))
					.method("translationVector",	static_cast<Matrix4x4::Ptr (Matrix4x4::*)(Vector3::Ptr)>(&Matrix4x4::translation))
					.method("transform",			static_cast<Vector3::Ptr (Matrix4x4::*)(Vector3::Ptr) const>(&Matrix4x4::transform))
					.method("deltaTransform",		static_cast<Vector3::Ptr(Matrix4x4::*)(Vector3::Ptr) const>(&Matrix4x4::deltaTransform))
		            .method("appendUniformScale",	static_cast<Matrix4x4::Ptr (Matrix4x4::*)(float)>(&Matrix4x4::appendScale))
		            .method("appendScale",			static_cast<Matrix4x4::Ptr (Matrix4x4::*)(float, float, float)>(&Matrix4x4::appendScale))
		            .method("appendRotationX",      &Matrix4x4::appendRotationX)
		            .method("appendRotationY",      &Matrix4x4::appendRotationY)
		            .method("appendRotationZ",      &Matrix4x4::appendRotationZ)
		            .method("appendRotation",       &Matrix4x4::appendRotation)
		            .method("appendTranslation",    static_cast<Matrix4x4::Ptr (Matrix4x4::*)(float, float, float)>(&Matrix4x4::appendTranslation))
		            .method("appendTranslationVector",    static_cast<Matrix4x4::Ptr(Matrix4x4::*)(Vector3::Ptr)>(&Matrix4x4::appendTranslation))
		            .method("prependUniformScale",	static_cast<Matrix4x4::Ptr (Matrix4x4::*)(float)>(&Matrix4x4::prependScale))
		            .method("prependScale",			static_cast<Matrix4x4::Ptr (Matrix4x4::*)(float, float, float)>(&Matrix4x4::prependScale))
		            .method("prependRotationX",     &Matrix4x4::prependRotationX)
		            .method("prependRotationY",     &Matrix4x4::prependRotationY)
		            .method("prependRotationZ",     &Matrix4x4::prependRotationZ)
		            .method("prependRotation",      &Matrix4x4::prependRotation)
		            .method("prependTranslation",   static_cast<Matrix4x4::Ptr (Matrix4x4::*)(float, float, float)>(&Matrix4x4::prependTranslation))
		            .method("prependTranslationVector",   static_cast<Matrix4x4::Ptr (Matrix4x4::*)(Vector3::Ptr)>(&Matrix4x4::prependTranslation))
					.method("getTranslation",		static_cast<Vector3::Ptr (Matrix4x4::*)(void) const>(&Matrix4x4::translation));
			}
		};
	}
}
