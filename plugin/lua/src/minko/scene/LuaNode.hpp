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
#include "minko/component/BoundingBox.hpp"
#include "minko/component/MasterAnimation.hpp"
#include "minko/component/AbstractLight.hpp"

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
				state.Class<std::vector<Node::Ptr>>("std__vector_scene__Node__Ptr_")
					.methodWrapper("at",	&LuaNode::atWrapper)
					.property("size", 	    &std::vector<Node::Ptr>::size);
					//.index(&LuaNode::getWrapper);

				state.Class<Node>("Node")
		            .method("create",				        static_cast<Node::Ptr (*)(void)>(&Node::create))
		            .method("addChild",				        &Node::addChild)
		            .method("removeChild",			        &Node::removeChild)
		            .method("contains",				        &Node::contains)
		            .method("addComponent",			        &Node::addComponent)
		            .method("removeComponent",		        &Node::removeComponent)
					.method("toString", 					&Node::toString)
                    .methodWrapper("getChildren",           &LuaNode::childrenWrapper)
                    .methodWrapper("getBoundingBox",        &LuaNode::getBoundingBoxWrapper)
                    .methodWrapper("getTransform",          &LuaNode::getTransformWrapper)
					.methodWrapper("getAnimation",			&LuaNode::getAnimationWrapper)
					.methodWrapper("getMasterAnimation",	&LuaNode::getMasterAnimationWrapper)
                    .methodWrapper("getPerspectiveCamera",  &LuaNode::getPerspectiveCameraWrapper)
					.methodWrapper("getSurface",			&LuaNode::getSurfaceWrapper)
					.methodWrapper("getRenderer",			&LuaNode::getRendererWrapper)
					.methodWrapper("hasLight",				&LuaNode::hasLightWrapper)
					.methodWrapper("hasAnimation",			&LuaNode::hasAnimationWrapper)
					/*.methodWrapper("getChildrenByName",		&LuaNode::getChildrenByNameWrapper)*/
		            .property("children",					&Node::children)
		            .property("data",				        &Node::data)
					.property("id",							&Node::id)
		            .property("root",				        &Node::root)
					.property("parent", 					&Node::parent)
		            .property("name",				        &Node::name, &Node::name);
			}

			static
			Node::Ptr
			atWrapper(std::vector<Node::Ptr>* v, uint index)
			{
				return v->at(index - 1);
			}

			static
			std::vector<Node::Ptr>*
			childrenWrapper(Node::Ptr node)
			{
				return const_cast<std::vector<Node::Ptr>*>(&(node->children()));
			}

			static
			std::shared_ptr<component::BoundingBox>
			getBoundingBoxWrapper(Node::Ptr node)
			{
				return node->component<component::BoundingBox>();
			}

			static
			std::shared_ptr<component::Transform>
			getTransformWrapper(Node::Ptr node)
			{
				return node->component<component::Transform>();
			}

			static
			std::shared_ptr<component::Animation>
			getAnimationWrapper(Node::Ptr node)
			{
				return node->component<component::Animation>();
			}

			static
			std::shared_ptr<component::MasterAnimation>
			getMasterAnimationWrapper(Node::Ptr node)
			{
				return node->component<component::MasterAnimation>();
			}

			static
			std::shared_ptr<component::PerspectiveCamera>
			getPerspectiveCameraWrapper(Node::Ptr node)
			{
				return node->component<component::PerspectiveCamera>();
			}

			static
			std::shared_ptr<component::Surface>
			getSurfaceWrapper(Node::Ptr node)
			{
				return node->component<component::Surface>();
			}

			static
			std::shared_ptr<component::Renderer>
			getRendererWrapper(Node::Ptr node)
			{
				return node->component<component::Renderer>();
			}

			static
			bool
			hasLightWrapper(Node::Ptr node)
			{
				return node->hasComponent<component::AbstractLight>();
			}

			static
			bool
			hasAnimationWrapper(Node::Ptr node)
			{
				return node->hasComponent<component::AbstractAnimation>();
			}
		};
	}
}
