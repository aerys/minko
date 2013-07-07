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

#include "TransformController.hpp"

#include "minko/math/Matrix4x4.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/data/Container.hpp"
#include "minko/data/Provider.hpp"

using namespace minko::controller;
using namespace minko::data;
using namespace minko::math;
using namespace minko::scene;

TransformController::TransformController() :
	minko::controller::AbstractController(),
	_transform(Matrix4x4::create()),
	_modelToWorld(Matrix4x4::create()),
	_data(Provider::create())
{
}

void
TransformController::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&TransformController::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&TransformController::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_data->set<Matrix4x4::Ptr>("transform/modelToWorldMatrix", _modelToWorld);
	//_data->set("transform/worldToModelMatrix", _worldToModel);
}

void
TransformController::targetAddedHandler(std::shared_ptr<AbstractController> ctrl,
										std::shared_ptr<Node> 				target)
{
	if (targets().size() > 1)
		throw std::logic_error("TransformController cannot have more than one target.");
	if (target->controller<TransformController>(1) != nullptr)
		throw std::logic_error("A node cannot have more than one TransformController.");

	target->data()->addProvider(_data);

	auto callback = std::bind(
		&TransformController::addedOrRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	);

	_addedSlot = target->added()->connect(callback);
	_removedSlot = target->removed()->connect(callback);

	addedOrRemovedHandler(nullptr, target, target->parent());
}

void
TransformController::addedOrRemovedHandler(std::shared_ptr<Node> node,
										   std::shared_ptr<Node> target,
										   std::shared_ptr<Node> parent)
{
	if (target == targets()[0] && !target->root()->controller<RootTransformController>()
		&& (target != target->root() || target->children().size() != 0))
		target->root()->addController(RootTransformController::create());
}

void
TransformController::targetRemovedHandler(std::shared_ptr<AbstractController> 	ctrl,
										  std::shared_ptr<Node> 				target)
{
	target->data()->removeProvider(_data);

	_addedSlot = nullptr;
	_removedSlot = nullptr;
}

void
TransformController::RootTransformController::initialize()
{
	_targetSlots.push_back(targetAdded()->connect(std::bind(
		&TransformController::RootTransformController::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	)));

	_targetSlots.push_back(targetRemoved()->connect(std::bind(
		&TransformController::RootTransformController::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	)));
}

void
TransformController::RootTransformController::targetAddedHandler(std::shared_ptr<AbstractController> 	ctrl,
																 std::shared_ptr<Node>					target)
{
	_targetSlots.push_back(target->added()->connect(std::bind(
		&TransformController::RootTransformController::addedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));
	_targetSlots.push_back(target->removed()->connect(std::bind(
		&TransformController::RootTransformController::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));
	_targetSlots.push_back(target->controllerAdded()->connect(std::bind(
		&TransformController::RootTransformController::controllerAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));
	_targetSlots.push_back(target->controllerRemoved()->connect(std::bind(
		&TransformController::RootTransformController::controllerRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));

	addedHandler(nullptr, target, target->parent());
}

void
TransformController::RootTransformController::targetRemovedHandler(std::shared_ptr<AbstractController> 	ctrl,
																   std::shared_ptr<Node>				target)
{
	_targetSlots.clear();
	_enterFrameSlots.clear();
}

void
TransformController::RootTransformController::controllerAddedHandler(std::shared_ptr<Node>					node,
																	 std::shared_ptr<Node> 					target,
														 			 std::shared_ptr<AbstractController>	ctrl)
{
	auto renderingCtrl = std::dynamic_pointer_cast<RenderingController>(ctrl);

	if (renderingCtrl != nullptr)
		_enterFrameSlots[renderingCtrl] = renderingCtrl->enterFrame()->connect(std::bind(
			&TransformController::RootTransformController::enterFrameHandler,
			shared_from_this(),
			std::placeholders::_1
		));
	else if (std::dynamic_pointer_cast<TransformController>(ctrl) != nullptr)
		_invalidLists = true;
}

void
TransformController::RootTransformController::controllerRemovedHandler(std::shared_ptr<Node>				node,
																	   std::shared_ptr<Node> 				target,
																	   std::shared_ptr<AbstractController>	ctrl)
{
	auto renderingCtrl = std::dynamic_pointer_cast<RenderingController>(ctrl);

	if (renderingCtrl != nullptr)
		_enterFrameSlots.erase(renderingCtrl);
	else if (std::dynamic_pointer_cast<TransformController>(ctrl) != nullptr)
		_invalidLists = true;
}

void
TransformController::RootTransformController::addedHandler(std::shared_ptr<Node> node,
												  		   std::shared_ptr<Node> target,
														   std::shared_ptr<Node> parent)
{
	auto enterFrameCallback = std::bind(
		&TransformController::RootTransformController::enterFrameHandler,
		shared_from_this(),
		std::placeholders::_1
	);

	auto descendants = NodeSet::create(NodeSet::Mode::MANUAL)
        ->select(target)
        ->descendants(true);

	for (auto descendant : descendants->nodes())
	{
		auto rootTransformCtrl = descendant->controller<RootTransformController>();

		if (rootTransformCtrl && rootTransformCtrl != shared_from_this())
			descendant->removeController(rootTransformCtrl);

		for (auto renderingCtrl : descendant->controllers<RenderingController>())
			_enterFrameSlots[renderingCtrl] = renderingCtrl->enterFrame()->connect(enterFrameCallback);
	}

	_invalidLists = true;
}

void
TransformController::RootTransformController::removedHandler(std::shared_ptr<Node> node,
														     std::shared_ptr<Node> target,
															 std::shared_ptr<Node> parent)
{
	auto descendants = NodeSet::create(NodeSet::Mode::MANUAL)
        ->select(target)
        ->descendants(true);

	for (auto descendant : descendants->nodes())
		for (auto renderingCtrl : descendant->controllers<RenderingController>())
			_enterFrameSlots.erase(renderingCtrl);

	_invalidLists = true;
}

void
TransformController::RootTransformController::updateTransformsList()
{
	unsigned int nodeId = 0;

	_transform.clear();
	_modelToWorld.clear();
	//_worldToModel.clear();

	auto descendants = NodeSet::create(NodeSet::Mode::MANUAL)
        ->select(targets().begin(), targets().end())
		->descendants(true, false)
        ->hasController<TransformController>();

	for (auto node : descendants->nodes())
	{
		auto transformCtrl  = node->controller<TransformController>();

		_nodeToId[node] = nodeId;

		_idToNode.push_back(node);
		_transform.push_back(transformCtrl->_transform);
		_modelToWorld.push_back(transformCtrl->_modelToWorld);
		_numChildren.push_back(0);
		_firstChildId.push_back(0);

		auto ancestor = node->parent();
		while (ancestor != nullptr && _nodeToId.count(ancestor) == 0)
			ancestor = ancestor->parent();

		if (ancestor != nullptr)
		{
			auto ancestorId = _nodeToId[ancestor];

			_parentId.push_back(ancestorId);
			if (_numChildren[ancestorId] == 0)
				_firstChildId[ancestorId] = nodeId;
			_numChildren[ancestorId]++;
		}
		else
			_parentId.push_back(-1);

		++nodeId;
	}

	_invalidLists = false;
}

void
TransformController::RootTransformController::updateTransforms()
{
	unsigned int numNodes 	= _transform.size();
	unsigned int nodeId 	= 0;

	while (nodeId < numNodes)
	{
		auto parentModelToWorldMatrix 	= _modelToWorld[nodeId];
		auto numChildren 				= _numChildren[nodeId];
		auto firstChildId 				= _firstChildId[nodeId];
		auto lastChildId 				= firstChildId + numChildren;
		auto parentId 					= _parentId[nodeId];

		if (parentId == -1)
			parentModelToWorldMatrix->copyFrom(_transform[nodeId]);

		for (auto childId = firstChildId; childId < lastChildId; ++childId)
			_modelToWorld[childId]->copyFrom(_transform[childId])->append(parentModelToWorldMatrix);

		++nodeId;
	}
}

void
TransformController::RootTransformController::enterFrameHandler(std::shared_ptr<RenderingController> ctrl)
{
	if (_invalidLists)
		updateTransformsList();

	updateTransforms();
}
