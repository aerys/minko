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

#include "minko/scene/Node.hpp"

#include "minko/LuaWrapper.hpp"

namespace minko
{
	namespace scene
	{
		class LuaNode
		{
		public:
			static
			void
			bind(LuaGlue& state)
			{
				state.Class<scene::Node>("Node")
		            //.method("getName",          static_cast<const std::string& (scene::Node::*)(void)>(&scene::Node::name))
		            //.method("setName",          static_cast<void (scene::Node::*)(const std::string&)>(&scene::Node::name))
		            //.prop("name", &scene::Node::name, &scene::Node::name)
		            .method("create",           static_cast<scene::Node::Ptr (*)(void)>(&scene::Node::create))
		            .method("addChild",         &scene::Node::addChild)
		            .method("removeChild",      &scene::Node::removeChild)
		            .method("contains",         &scene::Node::contains)
		            .method("addComponent",     &scene::Node::addComponent)
		            .method("removeComponent",  &scene::Node::removeComponent)
		            .property("data",           &scene::Node::data)
		            .property("root",           &scene::Node::root);
			}
		};
	}
}
