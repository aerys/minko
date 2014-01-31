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

#include "minko/component/Transform.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/math/Vector3.hpp"

#include "minko/LuaWrapper.hpp"

namespace minko
{
	namespace component
	{
		class LuaTransform :
			public LuaWrapper
		{
		public:
			static
			void
			bind(LuaGlue& state)
			{
				state.Class<Transform>("Transform")
					.method("create",				static_cast<Transform::Ptr(*)(void)>(&Transform::create))
					.method("createFromMatrix",		static_cast<Transform::Ptr(*)(math::Matrix4x4::Ptr)>(&Transform::create))
					.method("modelToWorld",			&LuaTransform::worldToModelWrapper)
					.method("deltaModelToWorld",	&LuaTransform::deltaModelToWorldWrapper)
					.method("worldToModel",			&LuaTransform::worldToModelWrapper)
					.method("deltaWorldToModel",	&LuaTransform::deltaWorldToModelWrapper)
					.property("matrix", 			&Transform::matrix)
					.property("modelToWorldMatrix",	static_cast<math::Matrix4x4::Ptr (Transform::*)()>(&Transform::modelToWorldMatrix));
			}

		private:
			static
			math::Vector3::Ptr
			modelToWorldWrapper(Transform::Ptr t, math::Vector3::Ptr v)
			{
				return t->modelToWorld(v);
			}

			static
			math::Vector3::Ptr
			deltaModelToWorldWrapper(Transform::Ptr t, math::Vector3::Ptr v)
			{
				return t->deltaModelToWorld(v);
			}

			static
			math::Vector3::Ptr
			worldToModelWrapper(Transform::Ptr t, math::Vector3::Ptr v)
			{
				return t->worldToModel(v);
			}

			static
			math::Vector3::Ptr
			deltaWorldToModelWrapper(Transform::Ptr t, math::Vector3::Ptr v)
			{
				return t->deltaWorldToModel(v);
			}
		};
	}
}
