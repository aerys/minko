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

#include "ColliderComponent.hpp"

#include <minko/math/Matrix4x4.hpp>
#include <minko/scene/Node.hpp>
#include <minko/scene/NodeSet.hpp>
#include <minko/component/Transform.hpp>
#include <minko/component/bullet/AbstractPhysicsShape.hpp>
#include <minko/component/bullet/Collider.hpp>
#include <minko/component/bullet/PhysicsWorld.hpp>

using namespace minko;
using namespace minko::math;
using namespace minko::scene;
using namespace minko::component;

bullet::ColliderComponent::ColliderComponent(Collider::Ptr collider):
	AbstractComponent(),
	_collider(collider),
	_physicsWorld(nullptr),
	_targetTransform(nullptr),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr),
	_graphicsTransformChangedSlot(nullptr)
{
	if (collider == nullptr)
		throw std::invalid_argument("collider");
}

void
bullet::ColliderComponent::initialize()
{
	_targetAddedSlot	= targetAdded()->connect(std::bind(
		&bullet::ColliderComponent::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));

	_targetRemovedSlot	= targetRemoved()->connect(std::bind(
		&bullet::ColliderComponent::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));

	_graphicsTransformChangedSlot	= _collider->graphicsWorldTransformChanged()->connect(std::bind(
		&bullet::ColliderComponent::graphicsWorldTransformChangedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));
}

void
bullet::ColliderComponent::targetAddedHandler(
	AbstractComponent::Ptr controller, 
	Node::Ptr target)
{
	if (targets().size() > 1)
		throw std::logic_error("ColliderComponent cannot have more than one target.");

	_addedSlot	= targets().front()->added()->connect(std::bind(
		&bullet::ColliderComponent::addedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
		));

	_removedSlot	= targets().front()->removed()->connect(std::bind(
		&bullet::ColliderComponent::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
		));

	// initialize from node if possible (mostly for adding a controller to the camera)
	initializeFromNode(target);
}

void
bullet::ColliderComponent::targetRemovedHandler(
	AbstractComponent::Ptr controller, 
	Node::Ptr target)
{
	_addedSlot						= nullptr;
	_removedSlot					= nullptr;
}

void 
bullet::ColliderComponent::addedHandler(
	Node::Ptr node, 
	Node::Ptr target, 
	Node::Ptr parent)
{
	initializeFromNode(node);
}

void
bullet::ColliderComponent::removedHandler(
	Node::Ptr node, 
	Node::Ptr target, 
	Node::Ptr parent)
{
	if (_physicsWorld != nullptr)
		_physicsWorld->removeChild(_collider);

	_physicsWorld		= nullptr;
	_targetTransform	= nullptr;
}

void
bullet::ColliderComponent::initializeFromNode(Node::Ptr node)
{
	if (_targetTransform != nullptr && _physicsWorld != nullptr)
		return;

	if (!node->hasComponent<Transform>())
		node->addComponent(Transform::create());
	
	_targetTransform = node->component<Transform>();

	auto nodeSet = NodeSet::create(node)
		->ancestors(true)
		->where([](Node::Ptr node)
		{
			return node->hasComponent<bullet::PhysicsWorld>();
		});

	if (nodeSet->nodes().size() != 1)
	{
#ifdef DEBUG_PHYSICS
		std::cout << "[" << node->name() << "]\tcollider CANNOT be added (# PhysicsWorld = " << nodeSet->nodes().size() << ")." << std::endl;
#endif // DEBUG_PHYSICS

		return;
	}

	_collider->name(node->name());

	_physicsWorld	= nodeSet->nodes().front()->component<bullet::PhysicsWorld>();
	_physicsWorld->addChild(_collider);

	synchronizePhysicsWithGraphics();
}

void
bullet::ColliderComponent::synchronizePhysicsWithGraphics()
{
	if (_physicsWorld == nullptr || _targetTransform == nullptr)
		return;

	auto graphicsTransform = _targetTransform->modelToWorldMatrix(true);

	// remove the influence of scaling and shear
	auto graphicsNoScaleTransform	= Matrix4x4::create();
	auto correction					= Matrix4x4::create();
	PhysicsWorld::removeScalingShear(
		graphicsTransform, 
		graphicsNoScaleTransform, 
		correction
	);

	// record the lost scaling and shear of the graphics transform
	_collider->correction(correction);

#ifdef DEBUG_PHYSICS
	std::cout << "[" << _collider->name() << "]\tsynchro graphics->physics" << std::endl;
	PhysicsWorld::print(std::cout << "- correction =\n", correction) << std::endl;
	PhysicsWorld::print(std::cout << "- scalefree(graphics) =\n", graphicsNoScaleTransform) << std::endl;
#endif // DEBUG_PHYSICS

	_physicsWorld->synchronizePhysicsWithGraphics(_collider, graphicsNoScaleTransform);
}

void
bullet::ColliderComponent::graphicsWorldTransformChangedHandler(Collider::Ptr collider, 
																Matrix4x4::Ptr graphicsTransform)
{
	if (_targetTransform == nullptr)
		return;

	// get the world-to-parent matrix in order to update the target's Transform
	auto worldToParent	= Matrix4x4::create()
		->copyFrom(_targetTransform->modelToWorldMatrix(true))
		->invert()
		->append(_targetTransform->transform());

	_targetTransform->transform()
		->copyFrom(graphicsTransform)
		->append(worldToParent);
}

void
bullet::ColliderComponent::prependLocalTranslation(Vector3::Ptr localTranslation)
{
	if (_physicsWorld != nullptr)
		_physicsWorld->prependLocalTranslation(_collider, localTranslation);
}

void
bullet::ColliderComponent::prependRotationY(float radians)
{
	if (_physicsWorld != nullptr)
		_physicsWorld->prependRotationY(_collider, radians);
}

void
bullet::ColliderComponent::applyRelativeImpulse(Vector3::Ptr localImpulse)
{
	if (_physicsWorld != nullptr)
		_physicsWorld->applyRelativeImpulse(_collider, localImpulse);
}