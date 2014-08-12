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

#include "minko/component/Culling.hpp"
#include "minko/scene/Node.hpp"
#include "minko/data/Container.hpp"
#include "minko/math/Frustum.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/math/OctTree.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Renderer.hpp"

using namespace minko;
using namespace minko::component;

/*static*/ std::shared_ptr<math::OctTree>	Culling::_octTree;

Culling::Culling(ShapePtr shape, 
				 const std::string& bindProperty):
	AbstractComponent(scene::Layout::Group::CULLING),
	_frustum(shape),
	_bindProperty(bindProperty)
{
}

void
Culling::targetAdded(NodePtr target)
{
	if (target->components<Culling>().size() > 1)
		throw std::logic_error("The same camera node cannot have more than one Culling.");
	if (target->components<component::PerspectiveCamera>().size() < 1)
		throw std::logic_error("Culling must be added to a camera");

	// compute scene bounding box

	if (_octTree == nullptr)
		_octTree = math::OctTree::create(50, 7, math::vec3(0.f));
	
	if (target->root()->hasComponent<SceneManager>())
		targetAddedToSceneHandler(nullptr, target, nullptr);
	else
		_addedToSceneSlot = target->added().connect(std::bind(
			&Culling::targetAddedToSceneHandler,
			std::static_pointer_cast<Culling>(shared_from_this()),
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3
		));
	
	_viewMatrixChangedSlot = target->data().propertyChanged(_bindProperty)->connect(std::bind(
		&Culling::worldToScreenChangedHandler,
		std::static_pointer_cast<Culling>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
        std::placeholders::_3
	));
}

void
Culling::targetRemoved(NodePtr target)
{
	_addedSlot			= nullptr;
	_layoutChangedSlot	= nullptr;
}

void
Culling::targetAddedToSceneHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
	if (target->root()->hasComponent<SceneManager>())
	{
		_addedToSceneSlot = nullptr;

		_layoutChangedSlot = target->root()->layoutsChanged().connect(std::bind(
			&Culling::layoutChangedHandler,
			std::static_pointer_cast<Culling>(shared_from_this()),
			std::placeholders::_1,
			std::placeholders::_2
		));

		_addedSlot = target->root()->added().connect(std::bind(
			&Culling::addedHandler,
			std::static_pointer_cast<Culling>(shared_from_this()),
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3
		));
	}
}

void
Culling::addedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
	auto layoutMask = this->layoutMask();
	scene::NodeSet::Ptr nodeSet = scene::NodeSet::create(target)->descendants(true)->where([layoutMask](NodePtr descendant)
	{
		return (descendant->layouts() & layoutMask) != 0;
	});

	for (auto n : nodeSet->nodes())
		_octTree->insert(n);
}

void
Culling::layoutChangedHandler(NodePtr node, NodePtr target)
{
	if ((target->layouts() & layoutMask()) != 0)
		_octTree->insert(target);
	else
		_octTree->remove(target);
}

void
Culling::worldToScreenChangedHandler(data::Container&       data,
                                     const std::string&     propertyName,
                                     const std::string&     fullPropertyName)
{
	_frustum->updateFromMatrix(data.get<math::mat4>(propertyName));
	
	auto renderer = target()->component<Renderer>();

	_octTree->testFrustum(
		_frustum, 
		[&](NodePtr node)
		{
			//node->component<Surface>()->computedVisibility(renderer, true);
		},
		[&](NodePtr node)
		{
			//node->component<Surface>()->computedVisibility(renderer, false);
		}
	);
}
