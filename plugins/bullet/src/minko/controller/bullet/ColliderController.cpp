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

#include "ColliderController.hpp"
#include <minko/math/Matrix4x4.hpp>
#include <minko/scene/Node.hpp>
#include <minko/scene/NodeSet.hpp>
#include <minko/controller/Transform.hpp>
#include <minko/controller/bullet/Collider.hpp>
#include <minko/controller/bullet/PhysicsWorld.hpp>

using namespace minko;
using namespace minko::math;
using namespace minko::scene;
using namespace minko::controller;

bullet::ColliderController::ColliderController(Collider::Ptr collider):
	AbstractController(),
	_collider(collider),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr),
	_physicsWorld(nullptr),
	_targetTransform(nullptr),
	_parentTransform(nullptr)
{

}

void
	bullet::ColliderController::initialize()
{
	_targetAddedSlot	= targetAdded()->connect(std::bind(
		&bullet::ColliderController::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));

	_targetRemovedSlot	= targetRemoved()->connect(std::bind(
		&bullet::ColliderController::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));

	_transformChangedSlot	= _collider->transformChanged()->connect(std::bind(
		&bullet::ColliderController::transformChangedHandler,
		shared_from_this(),
		std::placeholders::_1
		));
}

void
	bullet::ColliderController::targetAddedHandler(
	AbstractController::Ptr controller, 
	Node::Ptr target)
{
	if (targets().size() > 1)
		throw std::logic_error("ColliderController cannot have more than one target.");

	_addedSlot	= targets().front()->added()->connect(std::bind(
		&bullet::ColliderController::addedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
		));

	_removedSlot	= targets().front()->removed()->connect(std::bind(
		&bullet::ColliderController::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
		));
}

void
	bullet::ColliderController::targetRemovedHandler(
	AbstractController::Ptr controller, 
	Node::Ptr target)
{
	_addedSlot				= nullptr;
	_removedSlot			= nullptr;
	_transformChangedSlot	= nullptr;
}

void 
	bullet::ColliderController::addedHandler(
	Node::Ptr node, 
	Node::Ptr target, 
	Node::Ptr parent)
{
	if (!target->hasController<Transform>())
		throw std::logic_error("A ColliderController's target must have a Transform.");

	_targetTransform	= target->controller<Transform>();

	_parentTransform	= nullptr;
	Node::Ptr current	= target->parent();
	do
	{
		if (current == nullptr)
			break;
		if (current->hasController<Transform>())
		{
			_parentTransform	= current->controller<Transform>();
			break;
		}
		current	= current->parent();
	}
	while(current != nullptr && current != current->parent());

	auto targetLocalToWorldMatrix = Matrix4x4::create()
		->copyFrom(_targetTransform->transform())
		->append(_targetTransform->modelToWorldMatrix());

	_collider->initializeWorldTransform(targetLocalToWorldMatrix);

#ifdef DEBUG
	std::cout << "collider controller's target has been added\tto be added to physics world." << std::endl;
#endif // DEBUG

	auto nodeSet	= NodeSet::create(NodeSet::AUTO);
	nodeSet->select(target->root())
		->descendants(true)
		->hasController<bullet::PhysicsWorld>();
	if (nodeSet->nodes().size() != 1)
		throw std::logic_error("ColliderController requires exactly one PhysicsWorld controller among the descendants of its target node.");

	_physicsWorld	= nodeSet->nodes().front()->controller<bullet::PhysicsWorld>();
	_physicsWorld->addChild(_collider);
}

void
	bullet::ColliderController::removedHandler(
	Node::Ptr node, 
	Node::Ptr target, 
	Node::Ptr parent)
{
	_physicsWorld->removeChild(_collider);
	_physicsWorld	= nullptr;
	_targetTransform	= nullptr;
	_parentTransform	= nullptr;
}

void 
	bullet::ColliderController::transformChangedHandler(Collider::Ptr collider)
{
	auto targetTransform	= Matrix4x4::create(); // local-to-parent
	if (_parentTransform != nullptr)
	{
		auto parentToWorldInvTrf	= Matrix4x4::create()
			->append(_parentTransform->transform())
			->append(_parentTransform->modelToWorldMatrix())
			->invert(); // parent's world-to-local

		targetTransform
			->copyFrom(_collider->worldTransform())
			->append(parentToWorldInvTrf);
	}
	else
		targetTransform
			->copyFrom(collider->worldTransform());

	_targetTransform->transform()->copyFrom(targetTransform);
}
