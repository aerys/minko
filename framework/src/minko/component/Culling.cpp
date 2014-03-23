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

std::shared_ptr<math::OctTree>	Culling::_octTree;

Culling::Culling(ShapePtr shape, std::string bindProperty):
_frustum(shape),
_bindProperty(bindProperty)
{
}

void
Culling::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
        &Culling::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
    ));
    _targetRemovedSlot = targetAdded()->connect(std::bind(
        &Culling::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
    ));
}

void
Culling::targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target)
{
	if (target->components<Culling>().size() > 1)
		throw std::logic_error("The same camera node cannot have more than one Culling.");
	if (target->components<component::PerspectiveCamera>().size() < 1)
		throw std::logic_error("Culling must be added to a camera");

	// compute scene bounding box

	if (_octTree == nullptr)
		_octTree = math::OctTree::create(50, 7, math::vec3(0.f));
	
	if (target->root()->hasComponent<SceneManager>())
		targetAddedToScene(nullptr, target, nullptr);
	else
		_addedToSceneSlot = target->added()->connect(std::bind(
		&Culling::targetAddedToScene,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));


	_viewMatrixChangedSlot = target->data()->propertyValueChanged(_bindProperty)->connect(std::bind(
		&Culling::worldToScreenChanged,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
Culling::targetRemovedHandler(AbstractComponent::Ptr ctrl, NodePtr target)
{
	_addedSlot			= nullptr;
	_layoutChangedSlot	= nullptr;
}

void
Culling::targetAddedToScene(NodePtr node, NodePtr target, NodePtr ancestor)
{
	if (target->root()->hasComponent<SceneManager>())
	{
		_addedToSceneSlot = nullptr;

		_layoutChangedSlot = target->root()->layoutsChanged()->connect(std::bind(
			&Culling::layoutChanged,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2
		));

		_addedSlot = target->root()->added()->connect(std::bind(
			&Culling::addedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3
		));
	}
}

void
Culling::addedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
	scene::NodeSet::Ptr nodeSet = scene::NodeSet::create(target)->descendants(true)->where([](NodePtr descendant)
	{
		return (descendant->layouts() & (1u << 17)) != 0;
	});

	for (auto n : nodeSet->nodes())
		_octTree->insert(n);
}

void
Culling::layoutChanged(NodePtr node, NodePtr target)
{
	if (target->layouts() & (1u << 17))
		_octTree->insert(target);
	else
		_octTree->remove(target);
}

void
Culling::worldToScreenChanged(std::shared_ptr<data::Container> data, const std::string& propertyName)
{
	_frustum->updateFromMatrix(data->get<math::mat4>(propertyName));
	
	auto renderer = targets()[0]->component<Renderer>();

	_octTree->testFrustum(
		_frustum, 
		[&](NodePtr node)
		{
			node->component<Surface>()->computedVisibility(renderer, true);
		},
		[&](NodePtr node)
		{
			node->component<Surface>()->computedVisibility(renderer, false);
		}
	);
}