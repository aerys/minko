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

#include "minko/scene/NodeSet.hpp"

#include "LuaGlue/LuaGlue.h"

namespace minko
{
	namespace scene
	{
		class LuaNodeSet
		{
		public:
			static
			void
			bind(LuaGlue& state)
			{
			    state.Class<scene::NodeSet>("NodeSet")
			        .method("create", 		        static_cast<scene::NodeSet::Ptr (*)(std::shared_ptr<scene::Node>)>(&scene::NodeSet::create))
                    .methodWrapper("descendants",   &LuaNodeSet::descendantsWrapper)
                    .methodWrapper("ancestors",     &LuaNodeSet::ancestorsWrapper)
                    .methodWrapper("children",      &LuaNodeSet::childrenWrapper)
                    .methodWrapper("roots",         &LuaNodeSet::rootsWrapper)
                    .methodWrapper("where",         &LuaNodeSet::whereWrapper)
                    .methodWrapper("get",           &LuaNodeSet::getWrapper)
                    .methodWrapper("size",          &LuaNodeSet::sizeWrapper);
			}

			static
			NodeSet::Ptr
			descendantsWrapper(NodeSet::Ptr nodeSet, bool andSelf)
			{
				return nodeSet->descendants(andSelf);
			}

			static
			NodeSet::Ptr
			childrenWrapper(NodeSet::Ptr nodeSet, bool andSelf)
			{
				return nodeSet->children(andSelf);
			}

			static
			NodeSet::Ptr
			ancestorsWrapper(NodeSet::Ptr nodeSet, bool andSelf)
			{
				return nodeSet->ancestors(andSelf);
			}

			static
			NodeSet::Ptr
			rootsWrapper(NodeSet::Ptr nodeSet)
			{
				return nodeSet->roots();
			}

			static
			NodeSet::Ptr
			whereWrapper(NodeSet::Ptr nodeSet, std::shared_ptr<LuaGlueFunctionRef> p)
			{
				return nodeSet->where(std::bind(
					&LuaGlueFunctionRef::invoke<bool, std::shared_ptr<Node>>,
					p,
					std::placeholders::_1
				));
			}

			static
			std::shared_ptr<Node>
			getWrapper(NodeSet::Ptr nodeSet, int index)
			{
				return nodeSet->nodes().at(index - 1);
			}

			static
			unsigned int
			sizeWrapper(NodeSet::Ptr nodeSet)
			{
				return nodeSet->nodes().size();
			}
		};
	}
}

