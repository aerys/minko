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

#include "minko/component/FrustumCulling.hpp"
#include "minko/scene/Node.hpp"
#include "minko/data/Container.hpp"
#include "minko/math/Frustum.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/math/OctTree.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Surface.hpp"

using namespace minko;
using namespace minko::component;

FrustumCulling::FrustumCulling()
{
}

void
FrustumCulling::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
        &FrustumCulling::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
    ));
    _targetRemovedSlot = targetAdded()->connect(std::bind(
        &FrustumCulling::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
    ));
}

void
FrustumCulling::targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target)
{
	if (target->components<FrustumCulling>().size() > 1)
		throw std::logic_error("The same camera node cannot have more than one FrustumCulling.");
	if (target->components<component::PerspectiveCamera>().size() < 1)
		throw std::logic_error("FrustumCulling must be added to a camera");


	// compute scene bounding box

	_octTree = math::OctTree::create(50, 7, math::Vector3::create(0, 0, 0));
	_frustum = math::Frustum::create();

	if (target->root()->hasComponent<SceneManager>())
		targetAddedToScene(nullptr, target, nullptr);
	else
		_addedToSceneSlot = target->added()->connect(std::bind(
		&FrustumCulling::targetAddedToScene,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3));


	_viewMatrixChangedSlot = target->data()->propertyValueChanged("camera.worldToScreenMatrix")->connect(std::bind(
		&FrustumCulling::worldToScreenChanged,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2));
}

void
FrustumCulling::targetRemovedHandler(AbstractComponent::Ptr ctrl, NodePtr target)
{
	_addedSlot			= nullptr;
	_layoutChangedSlot	= nullptr;
}

void
FrustumCulling::targetAddedToScene(NodePtr node, NodePtr target, NodePtr ancestor)
{
	if (target->root()->hasComponent<SceneManager>())
	{
		_addedToSceneSlot = nullptr;

		_layoutChangedSlot = target->root()->layoutsChanged()->connect(std::bind(
			&FrustumCulling::layoutChanged,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2
		));

		_addedSlot = target->root()->added()->connect(std::bind(
			&FrustumCulling::addedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3
		));
	}
}

void
FrustumCulling::addedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
	scene::NodeSet::Ptr nodeSet = scene::NodeSet::create(target)->descendants(true)->where([](NodePtr descendant)
	{
		return descendant->layouts() & (1u << 17);
	});

	for (auto n : nodeSet->nodes())
		_octTree->insert(n);
}

void
FrustumCulling::layoutChanged(NodePtr node, NodePtr target)
{
	if (target->layouts() & (1u << 17))
		_octTree->insert(target);
	else
		_octTree->remove(target);
}

void
FrustumCulling::worldToScreenChanged(std::shared_ptr<data::Container> data, const std::string& propertyName)
{
	//std::cout << "update octTree" << std::endl;
	_frustum->updateFromMatrix(data->get<std::shared_ptr<math::Matrix4x4>>(propertyName));
	
	_octTree->testFrustum(
		_frustum, 
		[](NodePtr node)
		{
			//std::cout << 1 << std::endl;
			node->component<Surface>()->computedVisibility(true);
		},
		[](NodePtr node)
		{
			//std::cout << 0 << std::endl;
			node->component<Surface>()->computedVisibility(false);
		});
}