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
					typedef std::shared_ptr<ColliderData> ColliderDataPtr;

				public:

					static
					std::shared_ptr<Collider>
					extractColliderFromNode(std::shared_ptr<scene::Node> node)
					{
						return node->component<Collider>(0);
					}

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
							.method("extractCollider",					&LuaCollider::extractColliderFromNode);
					}

			};
		}
	}
}
