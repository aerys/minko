/*
Copyright (c) 2014 Aerys

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

#include "minko/ParticlesCommon.hpp"

#include "minko/MinkoLua.hpp"
#include "minko/component/ParticleSystem.hpp"
#include "minko/scene/Node.hpp"

namespace minko
{
	class LuaWrapper;

	namespace component
	{
		class LuaParticleSystem: public LuaWrapper
		{
		public:
			static 
			ParticleSystem::Ptr
			extractParticleSystemFromNode(scene::Node::Ptr node)
			{
				return node->hasComponent<ParticleSystem>() 
					? node->component<ParticleSystem>() 
					: nullptr;
			}

			static
			void
			bind(LuaGlue& state)
			{
				auto node = (LuaGlueClass<scene::Node>*)state.lookupClass("Node");
				
				node->methodWrapper("getParticles",	&LuaParticleSystem::extractParticleSystemFromNode);
				
				state.Class<ParticleSystem>("ParticleSystem")
					.method("getEmitting",			static_cast<bool (ParticleSystem::*)() const>				(&ParticleSystem::emitting))
					.method("setEmitting",			static_cast<ParticleSystem::Ptr (ParticleSystem::*)(bool)>	(&ParticleSystem::emitting))
					.method("getIsInWorldSpace",	static_cast<bool (ParticleSystem::*)() const>				(&ParticleSystem::isInWorldSpace))
					.method("setIsInWorldSpace",	static_cast<ParticleSystem::Ptr (ParticleSystem::*)(bool)>	(&ParticleSystem::isInWorldSpace));
			}
		};
	}
}