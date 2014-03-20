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

#include "minko/BulletCommon.hpp"

#include "minko/MinkoLua.hpp"
#include "minko/component/bullet/ColliderData.hpp"

namespace minko
{
	class LuaWrapper;

	namespace component
	{
		namespace bullet
		{
			class LuaColliderData :
				public LuaWrapper
			{
			private:
				typedef std::shared_ptr<math::Vector3> Vector3Ptr;
			public:

				static
				void
				bind(LuaGlue& state)
				{
					auto collideData = state.Class<ColliderData>("ColliderData")
						/*.method("getMass",				&ColliderData::mass)
						.method("getInertia",			&ColliderData::inertia)
						.method("getIsStatic",			&ColliderData::isStatic)
						.method("setRestitution",		static_cast<void (ColliderData::*)(float)>(&ColliderData::restitution))
						.method("setFriction",			static_cast<void (ColliderData::*)(float)>(&ColliderData::friction))
						.method("setRollingFriction",	static_cast<void (ColliderData::*)(float)>(&ColliderData::rollingFriction))
						.method("getRestitution",		static_cast<float (ColliderData::*)(void) const>(&ColliderData::restitution))
						.method("getFriction",			static_cast<float (ColliderData::*)(void) const>(&ColliderData::friction))
						.method("getRollingFriction",	static_cast<float (ColliderData::*)(void) const>(&ColliderData::rollingFriction))
						*/
						//.property("triggeredCollision", &ColliderData::triggerCollisions, &ColliderData::triggerCollisions)
						//.method("disableDeactivation",	&ColliderData::disableDeactivation)
						/*.method("getangularVelocity",	static_cast<Vector3Ptr(ColliderData::*)(void) const>(&ColliderData::angularVelocity))
						.method("setangularVelocity",	static_cast<void (ColliderData::*)(float, float, float)>(&ColliderData::angularVelocity))
						.method("getLinearVelocity",	static_cast<Vector3Ptr(ColliderData::*)(void) const>(&ColliderData::linearVelocity))
						.method("setLinearVelocity",	static_cast<void (ColliderData::*)(float, float, float)>(&ColliderData::linearVelocity))
						.method("getLinearFactor",		static_cast<Vector3Ptr(ColliderData::*)(void) const>(&ColliderData::linearFactor))
						.method("setLinearFactor",		static_cast<void (ColliderData::*)(float, float, float)>(&ColliderData::linearFactor))
						.method("getAngularDamping",	static_cast<float(ColliderData::*)(void) const>(&ColliderData::angularDamping))
						.method("setAngularDamping",	static_cast<void (ColliderData::*)(float)>(&ColliderData::angularDamping))
						.method("getLinearDamping",		static_cast<float(ColliderData::*)(void) const>(&ColliderData::linearDamping))
						.method("setLinearDamping",		static_cast<void (ColliderData::*)(float)>(&ColliderData::linearDamping))
						*/;

	/*				MINKO_LUAGLUE_BIND_SIGNAL(state, ColliderData::Ptr, ColliderData::Ptr);
					collideData.method("collisionStated", &ColliderData::collisionStarted);
					collideData.method("collisionEnded", &ColliderData::collisionEnded);
					*/
				}

			};
		}
	}
}
