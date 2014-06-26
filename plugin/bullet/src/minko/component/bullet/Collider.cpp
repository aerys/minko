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

#include "minko/component/bullet/Collider.hpp"

#include <minko/math/Matrix4x4.hpp>
#include <minko/scene/Node.hpp>
#include <minko/scene/NodeSet.hpp>
#include <minko/component/Transform.hpp>
#include <minko/component/Surface.hpp>
#include <minko/component/bullet/AbstractPhysicsShape.hpp>
#include <minko/component/bullet/ColliderData.hpp>
#include <minko/component/bullet/PhysicsWorld.hpp>
#include <minko/component/bullet/PhysicsWorld.hpp>
#include <minko/file/AssetLibrary.hpp>

#include "minko/math/tools.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::scene;
using namespace minko::component;

bullet::Collider::Collider(ColliderData::Ptr data):
	AbstractComponent(Layout::Mask::COLLISIONS_DYNAMIC_DEFAULT),
	_colliderData(data),
	_canSleep(false),
	_triggerCollisions(false),
	_linearFactor(Vector3::create(1.0f, 1.0f, 1.0f)),
	_linearDamping(0.0f),
	_linearSleepingThreshold(0.8f),
	_angularFactor(Vector3::create(1.0f, 1.0f, 1.0f)),
	_angularDamping(0.0f),
	_angularSleepingThreshold(1.0f),
	_physicsWorld(nullptr),
	_correction(Matrix4x4::create()),
	_physicsTransform(Matrix4x4::create()),
	_graphicsTransform(nullptr),
	_propertiesChanged(Signal<Ptr>::create()),
	_collisionStarted(Signal<Ptr, Ptr>::create()),
	_collisionEnded(Signal<Ptr, Ptr>::create()),
	_physicsTransformChanged(Signal<Ptr, Matrix4x4::Ptr>::create()),
	_graphicsTransformChanged(Signal<Ptr, Transform::Ptr>::create()),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr)
{
	if (data == nullptr)
		throw std::invalid_argument("data");
}

AbstractComponent::Ptr
bullet::Collider::clone(const CloneOption& option)
{
	Collider::Ptr origin = std::static_pointer_cast<Collider>(shared_from_this());
	return Collider::create(origin->_colliderData);
}

void
bullet::Collider::initialize()
{
	_targetAddedSlot	= targetAdded()->connect(std::bind(
		&bullet::Collider::targetAddedHandler,
		std::static_pointer_cast<Collider>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetRemovedSlot	= targetRemoved()->connect(std::bind(
		&bullet::Collider::targetRemovedHandler,
		std::static_pointer_cast<Collider>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
bullet::Collider::targetAddedHandler(AbstractComponent::Ptr, 
									 Node::Ptr target)
{
	if (targets().size() > 1)
		throw std::logic_error("Collider cannot have more than one target.");

	_addedSlot		= targets().front()->added()->connect(std::bind(
		&bullet::Collider::addedHandler,
		std::static_pointer_cast<Collider>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
		));

	_removedSlot	= targets().front()->removed()->connect(std::bind(
		&bullet::Collider::removedHandler,
		std::static_pointer_cast<Collider>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
		));
}

void
bullet::Collider::targetRemovedHandler(AbstractComponent::Ptr, 
									   Node::Ptr target)
{
	if (_physicsWorld != nullptr)
		_physicsWorld->removeChild(std::static_pointer_cast<Collider>(shared_from_this()));

	_physicsWorld		= nullptr;
	_graphicsTransform	= nullptr;

	_addedSlot			= nullptr;
	_removedSlot		= nullptr;
}

void 
bullet::Collider::addedHandler(Node::Ptr node, 
							   Node::Ptr, 
							   Node::Ptr)
{
	initializeFromNode(node);

	assert(_graphicsTransform);
}

void
bullet::Collider::removedHandler(Node::Ptr, Node::Ptr, Node::Ptr)
{
	//if (_physicsWorld != nullptr)
	//	_physicsWorld->removeChild(std::static_pointer_cast<Collider>(shared_from_this()));

	//_physicsWorld		= nullptr;
	//_graphicsTransform	= nullptr;
}

void
bullet::Collider::initializeFromNode(Node::Ptr node)
{
	if (_graphicsTransform != nullptr && _physicsWorld != nullptr)
		return;

	_physicsTransform->identity(); // matrix automatically updated by physicsWorldTransformChangedHandler 

	// get existing transform component or create one if necessary
	if (!node->hasComponent<Transform>())
		node->addComponent(Transform::create());

	_graphicsTransform = node->component<Transform>();

	if (fabsf(_graphicsTransform->modelToWorldMatrix(true)->determinant()) < 1e-3f)
		throw std::logic_error("The node's model-to-world matrix cannot be inverted.");

	// identify physics world
	auto withPhysicsWorld = NodeSet::create(node)
		->ancestors(true)
		->where([](Node::Ptr n){ return n->hasComponent<bullet::PhysicsWorld>(); });

	if (withPhysicsWorld->nodes().size() > 1)
		throw std::logic_error("Scene cannot contain more than one PhysicsWorld component.");

	_physicsWorld = withPhysicsWorld->nodes().empty() 
		? nullptr
		: withPhysicsWorld->nodes().front()->component<bullet::PhysicsWorld>();

	if (_physicsWorld)
		_physicsWorld->addChild(std::static_pointer_cast<Collider>(shared_from_this()));

	synchronizePhysicsWithGraphics();
}

void
bullet::Collider::synchronizePhysicsWithGraphics(bool forceTransformUpdate)
{
	assert(_graphicsTransform);

	auto		graphicsTransform		= _graphicsTransform->modelToWorldMatrix(forceTransformUpdate);
	static auto graphicsNoScale			= Matrix4x4::create();
	static auto graphicsNoScaleInverse	= Matrix4x4::create();
	static auto centerOfMassOffset		= Matrix4x4::create();
	static auto physicsTransform		= Matrix4x4::create();

	// remove the scaling/shear from the graphics transform, but record it to restitute it during rendering
	removeScalingShear(
		graphicsTransform, 
		graphicsNoScale,
		_correction
	);

	graphicsNoScaleInverse
		->copyFrom(graphicsNoScale)
		->invert();

	centerOfMassOffset
		->copyFrom(graphicsNoScaleInverse)
		->append(_colliderData->shape()->deltaTransformInverse())
		->append(graphicsNoScale);

	physicsTransform
		->copyFrom(_colliderData->shape()->deltaTransform())
		->prepend(graphicsNoScale);

	setPhysicsTransform(physicsTransform, _graphicsTransform->matrix());

	if (_physicsWorld)
		_physicsWorld->updateRigidBodyState(
			std::static_pointer_cast<Collider>(shared_from_this()), 
			graphicsNoScale, 
			centerOfMassOffset
		);
}

bullet::Collider::Ptr
bullet::Collider::setPhysicsTransform(Matrix4x4::Ptr	physicsTransform,
									  Matrix4x4::Ptr	graphicsModelToParent,
									  bool				forceTransformUpdate)
{
	assert(_graphicsTransform);

	// update the physics world transform
	_physicsTransform->copyFrom(physicsTransform);

	if (graphicsModelToParent)
		_graphicsTransform->matrix()->copyFrom(graphicsModelToParent);
	else
	{
		// recompute graphics transform from the physics transform

		// update the graphics local transform
		static auto worldToParent	= Matrix4x4::create();
	
		worldToParent
			->copyFrom(_graphicsTransform->modelToWorldMatrix(forceTransformUpdate))->invert()
			->append(_graphicsTransform->matrix());

		_graphicsTransform->matrix()
			->copyFrom(_correction)
			->append(_colliderData->shape()->deltaTransformInverse())
			->append(physicsTransform)
			->append(worldToParent);
	}

	// fire update signals
	_physicsTransformChanged->execute(std::static_pointer_cast<Collider>(shared_from_this()), _physicsTransform);
	_graphicsTransformChanged->execute(std::static_pointer_cast<Collider>(shared_from_this()), _graphicsTransform);

	return std::static_pointer_cast<Collider>(shared_from_this());
}

Matrix4x4::Ptr
bullet::Collider::getPhysicsTransform(Matrix4x4::Ptr output) const
{
	return output
		? output->copyFrom(_physicsTransform)
		: Matrix4x4::create(_physicsTransform);
}

Vector3::Ptr
bullet::Collider::linearVelocity(Vector3::Ptr output) const
{
	return _physicsWorld
		? _physicsWorld->getColliderLinearVelocity(
			std::static_pointer_cast<const Collider>(shared_from_this()), 
			output
		)
		: output;
}

bullet::Collider::Ptr
bullet::Collider::linearVelocity(Vector3::Ptr value)
{
	if (_physicsWorld)
		_physicsWorld->setColliderLinearVelocity(
			std::static_pointer_cast<Collider>(shared_from_this()), 
			value
		);

	return std::static_pointer_cast<Collider>(shared_from_this());
}

Vector3::Ptr
bullet::Collider::angularVelocity(Vector3::Ptr output) const
{
	return _physicsWorld
		? _physicsWorld->getColliderAngularVelocity(
			std::static_pointer_cast<const Collider>(shared_from_this()), 
			output
		)
		: output;
}

bullet::Collider::Ptr
bullet::Collider::angularVelocity(Vector3::Ptr value)
{
	if (_physicsWorld)
		_physicsWorld->setColliderAngularVelocity(
			std::static_pointer_cast<Collider>(shared_from_this()), 
			value
		);

	return std::static_pointer_cast<Collider>(shared_from_this());
}

bullet::Collider::Ptr
bullet::Collider::applyImpulse(Vector3::Ptr impulse, Vector3::Ptr relPosition)
{
	if (_physicsTransform)
		_physicsWorld->applyImpulse(
			std::static_pointer_cast<Collider>(shared_from_this()), 
			impulse, 
			false, 
			relPosition
		);

	return std::static_pointer_cast<Collider>(shared_from_this());
}

bullet::Collider::Ptr
bullet::Collider::applyRelativeImpulse(Vector3::Ptr impulse, Vector3::Ptr relPosition)
{
	if (_physicsTransform)
		_physicsWorld->applyImpulse(
			std::static_pointer_cast<Collider>(shared_from_this()), 
			impulse, 
			true, 
			nullptr
		);

	return std::static_pointer_cast<Collider>(shared_from_this());
}

bullet::Collider::Ptr
bullet::Collider::linearFactor(Vector3::Ptr values)
{
	const bool changed = fabsf(values->x() - _linearFactor->x()) > 1e-3f 
		|| fabsf(values->y() - _linearFactor->y()) > 1e-3f 
		|| fabsf(values->z() - _linearFactor->z()) > 1e-3f;

	_linearFactor->copyFrom(values);

	if (changed)
		_propertiesChanged->execute(std::static_pointer_cast<Collider>(shared_from_this()));

	return std::static_pointer_cast<Collider>(shared_from_this());
}


bullet::Collider::Ptr
bullet::Collider::angularFactor(Vector3::Ptr values)
{
	const bool changed = fabsf(values->x() - _angularFactor->x()) > 1e-3f 
		|| fabsf(values->y() - _angularFactor->y()) > 1e-3f 
		|| fabsf(values->z() - _angularFactor->z()) > 1e-3f;

	_angularFactor->copyFrom(values);

	if (changed)
		_propertiesChanged->execute(std::static_pointer_cast<Collider>(shared_from_this()));

	return std::static_pointer_cast<Collider>(shared_from_this());
}


bullet::Collider::Ptr
bullet::Collider::damping(float linearDamping, float angularDamping)
{
	const bool changed = fabsf(_linearDamping - linearDamping) > 1e-3f 
		|| fabsf(_angularDamping - angularDamping) > 1e-3f;

	_linearDamping	= linearDamping;
	_angularDamping	= angularDamping;

	if (changed)
		_propertiesChanged->execute(std::static_pointer_cast<Collider>(shared_from_this()));

	return std::static_pointer_cast<Collider>(shared_from_this());
}

bullet::Collider::Ptr
bullet::Collider::sleepingThresholds(float linearSleepingThreshold, float angularSleepingThreshold)
{
	const bool changed = fabsf(_linearSleepingThreshold - linearSleepingThreshold) > 1e-3f 
		|| fabsf(_angularSleepingThreshold - angularSleepingThreshold) > 1e-3f;

	_linearSleepingThreshold	= linearSleepingThreshold;
	_angularSleepingThreshold	= angularSleepingThreshold;

	if (changed)
		_propertiesChanged->execute(std::static_pointer_cast<Collider>(shared_from_this()));

	return std::static_pointer_cast<Collider>(shared_from_this());
}

bullet::Collider::Ptr
bullet::Collider::canSleep(bool value)
{
	const bool changed = _canSleep != value;

	_canSleep = value;

	if (changed)
		_propertiesChanged->execute(std::static_pointer_cast<Collider>(shared_from_this()));

	return std::static_pointer_cast<Collider>(shared_from_this());
}