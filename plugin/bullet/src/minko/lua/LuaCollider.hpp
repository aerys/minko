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
#include "minko/component/bullet/Collider.hpp"
#include "minko/scene/Node.hpp"

namespace minko
{
	class LuaWrapper;

	namespace component
	{
		namespace bullet
		{
			class LuaCollider :
				public LuaWrapper
			{
		
			private:
				typedef std::shared_ptr<ColliderData>	ColliderDataPtr;
				typedef std::shared_ptr<math::Vector3>	Vector3Ptr;

			public:

				static
				void
				bind(LuaGlue& state)
				{
					auto scene_node = (LuaGlueClass<scene::Node>*)state.lookupClass("Node");

					scene_node->methodWrapper("getCollider", &LuaCollider::extractColliderFromNode);

					state.Class<Collider>("Collider")
						.property("colliderData",					&Collider::colliderData)
						.method("create",							static_cast<Collider::Ptr (*)(ColliderDataPtr)>(&Collider::create))
						.method("synchronizePhysicsWithGraphics",	&Collider::synchronizePhysicsWithGraphics)
						.method("show",								&Collider::show)
						.method("hide",								&Collider::hide)
						.method("extractCollider",					&LuaCollider::extractColliderFromNode)
						.method("setLinearVelocity",				static_cast<Collider::Ptr (Collider::*)(Vector3Ptr)>				(&Collider::linearVelocity))
						.method("getLinearVelocity",				&LuaCollider::getLinearVelocityWrapper)
						.method("setAngularVelocity",				static_cast<Collider::Ptr (Collider::*)(Vector3Ptr)>				(&Collider::angularVelocity))
						.method("getAngularVelocity",				&LuaCollider::getAngularVelocityWrapper)
						.method("applyImpulse",						static_cast<Collider::Ptr (Collider::*)(Vector3Ptr, Vector3Ptr)>	(&Collider::applyImpulse))
						.method("applyRelativeImpulse",				static_cast<Collider::Ptr (Collider::*)(Vector3Ptr, Vector3Ptr)>	(&Collider::applyRelativeImpulse))
						.method("applyCentralImpulse",				&LuaCollider::applyCentralImpulse)
						.method("applyCentralRelativeImpulse",		&LuaCollider::applyCentralRelativeImpulse)
						.method("setLinearFactor",					static_cast<Collider::Ptr (Collider::*)(Vector3Ptr)>				(&Collider::linearFactor))
						.method("getLinearFactor",					&LuaCollider::getLinearFactorsWrapper)
						.method("setAngularFactor",					static_cast<Collider::Ptr (Collider::*)(Vector3Ptr)>				(&Collider::angularFactor))
						.method("getAngularFactor",					&LuaCollider::getAngularFactorsWrapper)
						.method("setCanSleep",						static_cast<Collider::Ptr (Collider::*)(bool)>						(&Collider::canSleep))
						.method("getCanSleep",						&LuaCollider::getCanSleepWrapper);
				}

			private:
				static
				bullet::Collider::Ptr
				extractColliderFromNode(std::shared_ptr<scene::Node> node)
				{
					return node->component<Collider>(0);
				}

				static
				Vector3Ptr
				getLinearVelocityWrapper(bullet::Collider::Ptr collider)
				{
					return collider->linearVelocity();
				}

				static
				Vector3Ptr
				getAngularVelocityWrapper(bullet::Collider::Ptr collider)
				{
					return collider->angularVelocity();
				}

				static
				Vector3Ptr
				getLinearFactorsWrapper(bullet::Collider::Ptr collider)
				{
					return collider->linearFactor();
				}

				static
				Vector3Ptr
				getAngularFactorsWrapper(bullet::Collider::Ptr collider)
				{
					return collider->angularFactor();
				}

				static
				bullet::Collider::Ptr
				applyCentralImpulse(bullet::Collider::Ptr collider, Vector3Ptr impulse)
				{
					return collider->applyImpulse(impulse, nullptr);
				}

				static
				bullet::Collider::Ptr
				applyCentralRelativeImpulse(bullet::Collider::Ptr collider, Vector3Ptr impulse)
				{
					return collider->applyRelativeImpulse(impulse, nullptr);
				}

				static
				bullet::Collider::Ptr
				getCanSleepWrapper(bullet::Collider::Ptr collider, bool value)
				{
					return collider->canSleep(value);
				}
			};
		}
	}
}
