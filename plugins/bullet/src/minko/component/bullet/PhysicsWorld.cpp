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

#include "PhysicsWorld.hpp"

#include <btBulletDynamicsCommon.h>
#include <minko/math/Matrix4x4.hpp>
#include <minko/scene/Node.hpp>
#include <minko/scene/NodeSet.hpp>
#include <minko/component/SceneManager.hpp>
#include <minko/component/bullet/Collider.hpp>
#include <minko/component/bullet/AbstractPhysicsShape.hpp>

using namespace minko;
using namespace minko::math;
using namespace minko::scene;
using namespace minko::component;

bullet::PhysicsWorld::PhysicsWorld():
	_colliderMap(),
	_btBroadphase(nullptr),
	_btCollisionConfiguration(nullptr),
	_btConstraintSolver(nullptr),
	_btDispatcher(nullptr),
	_btDynamicsWorld(nullptr),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_frameEndSlot(nullptr),
	_componentAddedOrRemovedSlot(nullptr),
	_addedOrRemovedSlot(nullptr)
{
}

void 
bullet::PhysicsWorld::initialize()
{
	// straightforward physics world initialization for the time being
	_btBroadphase				= std::shared_ptr<btDbvtBroadphase>(new btDbvtBroadphase());
	_btCollisionConfiguration	= std::shared_ptr<btDefaultCollisionConfiguration>(new btDefaultCollisionConfiguration());
	_btConstraintSolver			= std::shared_ptr<btSequentialImpulseConstraintSolver>(new btSequentialImpulseConstraintSolver());
	_btDispatcher				= std::shared_ptr<btCollisionDispatcher>(new btCollisionDispatcher(_btCollisionConfiguration.get()));

	_btDynamicsWorld			= std::shared_ptr<btDiscreteDynamicsWorld>(new btDiscreteDynamicsWorld(
		_btDispatcher.get(), 
		_btBroadphase.get(),
		_btConstraintSolver.get(),
		_btCollisionConfiguration.get()
	));

	_targetAddedSlot	= targetAdded()->connect(std::bind(
		&bullet::PhysicsWorld::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetRemovedSlot	= targetRemoved()->connect(std::bind(
		&bullet::PhysicsWorld::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void 
bullet::PhysicsWorld::targetAddedHandler(AbstractComponent::Ptr controller, 
										 Node::Ptr				target)
{
	if (targets().size() > 1)
		throw std::logic_error("The same PhysicsWorld cannot be used twice.");

	setSceneManager(target->root()->component<SceneManager>());
}

void 
bullet::PhysicsWorld::targetRemovedHandler(AbstractComponent::Ptr	controller, 
										   Node::Ptr				target)
{
	_sceneManager = nullptr;
	_frameEndSlot = nullptr;
	_addedOrRemovedSlot = nullptr;
	_componentAddedOrRemovedSlot = nullptr;
}

void
bullet::PhysicsWorld::setSceneManager(std::shared_ptr<SceneManager> sceneManager)
{
	if (sceneManager != _sceneManager || (!_componentAddedOrRemovedSlot && !_addedOrRemovedSlot))
	{
		auto target = targets()[0];
		auto componentCallback = [&](NodePtr node, NodePtr target, AbsCtrlPtr cmp)
		{
			setSceneManager(target->root()->component<SceneManager>());
		};
		auto nodeCallback = [&](NodePtr node, NodePtr target, NodePtr ancestor)
		{
			setSceneManager(target->root()->component<SceneManager>());
		};

		if (sceneManager)
		{
			_sceneManager = sceneManager;
			_frameEndSlot = sceneManager->cullingBegin()->connect(std::bind(
				&PhysicsWorld::frameEndHandler, shared_from_this(), std::placeholders::_1
			));

			_componentAddedOrRemovedSlot = target->componentRemoved()->connect(componentCallback);
			_addedOrRemovedSlot = target->removed()->connect(nodeCallback);
		}
		else
		{
			_sceneManager = nullptr;
			_frameEndSlot = nullptr;

			_componentAddedOrRemovedSlot = target->componentAdded()->connect(componentCallback);
			_addedOrRemovedSlot = target->added()->connect(nodeCallback);
		}
	}
}

void
bullet::PhysicsWorld::addChild(Collider::Ptr collider)
{
	if (hasCollider(collider))
		throw std::logic_error("The same collider cannot be added twice.");

	BulletCollider::Ptr btCollider = BulletCollider::create(collider);
	_colliderMap.insert(std::pair<Collider::Ptr, BulletCollider::Ptr>(collider, btCollider));

	std::dynamic_pointer_cast<btDiscreteDynamicsWorld>(_btDynamicsWorld)
		->addRigidBody(std::dynamic_pointer_cast<btRigidBody>(btCollider->collisionObject()).get());
	//_btDynamicsWorld->addCollisionObject(btCollider->collisionObject().get());
}

void
bullet::PhysicsWorld::removeChild(Collider::Ptr collider)
{
	ColliderMap::const_iterator	it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		throw std::invalid_argument("collider");

	_btDynamicsWorld->removeCollisionObject(it->second->collisionObject().get());

	_colliderMap.erase(it);
}

bool
bullet::PhysicsWorld::hasCollider(Collider::Ptr collider) const
{
	return _colliderMap.find(collider) != _colliderMap.end();
}


void
bullet::PhysicsWorld::setGravity(Vector3::Ptr gravity)
{
	_btDynamicsWorld->setGravity(btVector3(gravity->x(), gravity->y(), gravity->z()));
}

void
bullet::PhysicsWorld::frameEndHandler(std::shared_ptr<SceneManager> sceneManager)
{
	update();
}


void
bullet::PhysicsWorld::update(float timeStep)
{
	_btDynamicsWorld->stepSimulation(timeStep);
	updateColliders();
}

void
bullet::PhysicsWorld::updateColliders()
{
	for (ColliderMap::iterator it = _colliderMap.begin(); it != _colliderMap.end(); ++it)
	{
		Collider::Ptr		collider(it->first);
		BulletCollider::Ptr	btCollider(it->second);

		if (collider->isStatic())
			continue;

		const btTransform& colliderWorldTrf(btCollider->collisionObject()->getWorldTransform());	

		collider->updateColliderWorldTransform(fromBulletTransform(colliderWorldTrf));
	}
}

void
bullet::PhysicsWorld::setWorldTransformFromCollider(Collider::Ptr collider)
{
	auto it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		return;

	it->second->setWorldTransform(collider->worldTransform());
}

void
bullet::PhysicsWorld::forceColliderWorldTransform(Collider::Ptr collider, Matrix4x4::Ptr worldTransform)
{
	auto it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		return;

	const float scaling = powf(fabsf(worldTransform->determinant3x3()), 1.0f/3.0f);
	if (scaling < 1e-6f)
		throw std::logic_error("Failed to force collider's world transform (null scaling).");

	const float invScaling = 1.0f/scaling;
	auto scaleFreeMatrix = Matrix4x4::create()
		->copyFrom(worldTransform)
		->prependScaling(invScaling, invScaling, invScaling);

	it->second->setWorldTransform(scaleFreeMatrix);
	it->first->updateColliderWorldTransform(scaleFreeMatrix);
}

Matrix4x4::Ptr
bullet::PhysicsWorld::getPhysicsWorldTransform(Collider::Ptr collider) const
{
	auto it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		return Matrix4x4::create()->identity();

	return fromBulletTransform(it->second->collisionObject()->getWorldTransform());
}

void
bullet::PhysicsWorld::setPhysicsWorldTransform(Collider::Ptr collider, Matrix4x4::Ptr transform) const
{
	auto it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		return;

	it->second->setWorldTransform(transform);
}

void
bullet::PhysicsWorld::setLinearVelocity(Collider::Ptr collider, Vector3::Ptr velocity)
{
	auto it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		return;

	it->second->setLinearVelocity(velocity);
}

void
bullet::PhysicsWorld::prependLocalTranslation(Collider::Ptr collider, Vector3::Ptr translation)
{
	auto it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		return;

	it->second->prependLocalTranslation(translation);
}

void
bullet::PhysicsWorld::prependRotationY(Collider::Ptr collider, float radians)
{
	auto it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		return;

	it->second->prependRotationY(radians);
}

void
bullet::PhysicsWorld::applyRelativeImpulse(Collider::Ptr collider, Vector3::Ptr relativeForce)
{
	auto it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		return;

	it->second->applyRelativeImpulse(relativeForce);
}

/*static*/
Matrix4x4::Ptr
bullet::PhysicsWorld::fromBulletTransform(const btTransform& transform)
{
	auto basis			= transform.getBasis();
	auto translation	= transform.getOrigin();

	Matrix4x4::Ptr	output = Matrix4x4::create();
	output->initialize(
		basis[0][0], basis[0][1], basis[0][2], translation[0],
		basis[1][0], basis[1][1], basis[1][2], translation[1],
		basis[2][0], basis[2][1], basis[2][2], translation[2],
		0.0f, 0.0f, 0.0f, 1.0f
		);
	return output;
}

/*static*/
void
bullet::PhysicsWorld::toBulletTransform(Matrix4x4::Ptr transform,
	btTransform& output)
{
	toBulletTransform(
		transform->rotation(), 
		transform->translationVector(), output
	);
	/*
	auto translation	= transform->translationVector();
	auto rotation		= transform->rotation();

	btVector3		btOrigin(translation->x(), translation->y(), translation->z());
	btQuaternion	btRotation(rotation->i(), rotation->j(), rotation->k(), rotation->r());

	output.setOrigin(btOrigin);
	output.setRotation(btRotation);
	*/
}

/*static*/
void
bullet::PhysicsWorld::toBulletTransform(Quaternion::Ptr rotation, 
	Vector3::Ptr translation, 
	btTransform& output)
{
	btQuaternion	btRotation(rotation->i(), rotation->j(), rotation->k(), rotation->r());
	btVector3		btOrigin(translation->x(), translation->y(), translation->z());

	output.setOrigin(btOrigin);
	output.setRotation(btRotation);
}