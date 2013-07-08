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
	_physicsWorld(nullptr),
	_targetTransform(nullptr),
	_parentTransform(nullptr),
	_parents(nullptr),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr),
	_transformChangedSlot(nullptr),
	_parentAddedSlot(nullptr),
	_parentRemovedSlot(nullptr)
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

	_parents			= NodeSet::create(NodeSet::AUTO)
		->select(target)
		->ancestors(false)
		->hasController<Transform>();

	_parentAddedSlot	= _parents->nodeAdded()->connect(std::bind(
		&bullet::ColliderController::updateParentTransform,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));

	_parentRemovedSlot	= _parents->nodeAdded()->connect(std::bind(
		&bullet::ColliderController::updateParentTransform,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));

	updateParentTransform(_parents);

	_collider->initializeWorldTransform(_targetTransform->modelToWorldMatrix(true));

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
	_physicsWorld		= nullptr;
	_targetTransform	= nullptr;
	_parentTransform	= nullptr;
	_parents			= nullptr;

	_parentAddedSlot	= nullptr;
	_parentRemovedSlot	= nullptr;
}

void
	bullet::ColliderController::updateParentTransform(NodeSet::Ptr parents, Node::Ptr)
{
	_parentTransform	= nullptr;
	if (parents == nullptr || parents->nodes().empty())
		return;

	auto firstParent	= parents->nodes().front();
	if (!firstParent->hasController<Transform>())
		throw std::logic_error("All target's parents are expected to have a Transform.");

	_parentTransform	= firstParent->controller<Transform>();
}

void 
	bullet::ColliderController::transformChangedHandler(Collider::Ptr collider)
{

	auto worldToParentMatrix	= _parentTransform == nullptr
		? Matrix4x4::create()->identity()
		: Matrix4x4::create()->copyFrom(_parentTransform->modelToWorldMatrix(true))->invert();

	// compute the target-to-parent transform
	auto transform		= Matrix4x4::create()
		->copyFrom(collider->worldTransform())
		->append(worldToParentMatrix);

	_targetTransform->transform()->copyFrom(transform);
}
