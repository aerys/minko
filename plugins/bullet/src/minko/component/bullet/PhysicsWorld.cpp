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
#include <minko/component/Rendering.hpp>
#include <minko/component/bullet/Collider.hpp>
#include <minko/component/bullet/ColliderData.hpp>
#include <minko/component/bullet/AbstractPhysicsShape.hpp>

using namespace minko;
using namespace minko::math;
using namespace minko::scene;
using namespace minko::component;

bullet::PhysicsWorld::PhysicsWorld():
	_colliderMap(),
	_rendering(rendering),
	_bulletBroadphase(nullptr),
	_bulletCollisionConfiguration(nullptr),
	_bulletConstraintSolver(nullptr),
	_bulletDispatcher(nullptr),
	_bulletDynamicsWorld(nullptr),
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
	_bulletBroadphase				= std::shared_ptr<btDbvtBroadphase>(new btDbvtBroadphase());
	_bulletCollisionConfiguration	= std::shared_ptr<btDefaultCollisionConfiguration>(new btDefaultCollisionConfiguration());
	_bulletConstraintSolver			= std::shared_ptr<btSequentialImpulseConstraintSolver>(new btSequentialImpulseConstraintSolver());
	_bulletDispatcher				= std::shared_ptr<btCollisionDispatcher>(new btCollisionDispatcher(_bulletCollisionConfiguration.get()));

	_bulletDynamicsWorld			= std::shared_ptr<btDiscreteDynamicsWorld>(new btDiscreteDynamicsWorld(
		_bulletDispatcher.get(), 
		_bulletBroadphase.get(),
		_bulletConstraintSolver.get(),
		_bulletCollisionConfiguration.get()
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

	BulletCollider::Ptr bulletCollider = BulletCollider::create(collider);
	_colliderMap.insert(std::pair<Collider::Ptr, BulletCollider::Ptr>(collider, bulletCollider));

	std::dynamic_pointer_cast<btDiscreteDynamicsWorld>(_bulletDynamicsWorld)
		->addRigidBody(bulletCollider->rigidBody().get());
	//_bulletDynamicsWorld->addCollisionObject(bulletCollider->collisionObject().get());


#ifdef DEBUG_PHYSICS
	std::cout << "[" << collider->name() << "]\tadd physics body" << std::endl;

	btTransform bulletTransform;
	bulletCollider->motionState()->getWorldTransform(bulletTransform);
	print(std::cout << "motionstate.worldTransform =\n", bulletTransform) << std::endl;

	bulletCollider->rigidBody()->getMotionState()->getWorldTransform(bulletTransform);
	print(std::cout << "rigid.body.motionstate.worldTransform =\n", bulletTransform) << std::endl;

	print(std::cout << "rigidbody.worldTransform =\n", bulletCollider->rigidBody()->getWorldTransform()) << std::endl;
#endif // DEBUG_PHYSICS
}

void
bullet::PhysicsWorld::removeChild(Collider::Ptr collider)
{
	ColliderMap::const_iterator	it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		throw std::invalid_argument("collider");

	_bulletDynamicsWorld->removeCollisionObject(it->second->collisionObject().get());

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
	_bulletDynamicsWorld->setGravity(btVector3(gravity->x(), gravity->y(), gravity->z()));
}

void
bullet::PhysicsWorld::frameEndHandler(std::shared_ptr<SceneManager> sceneManager)
{
	update();
}


void
bullet::PhysicsWorld::update(float timeStep)
{
	_bulletDynamicsWorld->stepSimulation(timeStep);
	updateColliders();
}

void
bullet::PhysicsWorld::updateColliders()
{
	for (ColliderMap::iterator it = _colliderMap.begin(); it != _colliderMap.end(); ++it)
	{
		Collider::Ptr		collider(it->first);
		BulletCollider::Ptr	bulletCollider(it->second);

		if (collider->isStatic())
			continue;

		const btTransform& colliderWorldTrf(bulletCollider->collisionObject()->getWorldTransform());	

		//btTransform colliderWorldTrf;
		//bulletCollider->motionState()->getWorldTransform(colliderWorldTrf);


		//collider->updateColliderWorldTransform(fromBulletTransform(colliderWorldTrf));

		collider->updateGraphicsTransformFromPhysics(fromBulletTransform(colliderWorldTrf));
	}
}

void
bullet::PhysicsWorld::setPhysicsWorldMatrix(Collider::Ptr collider, 
											Matrix4x4::Ptr worldMatrix)
{
	auto it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		return;

#ifdef DEBUG_PHYSICS
	const float scaling = powf(fabsf(worldMatrix->determinant3x3()), 1.0f/3.0f);
	if (fabsf(scaling - 1.0f) > 1e-3)
		throw std::logic_error("Physics world matrices must be pure rotation + translation matrices.");
#endif // DEBUG_PHYSICS

	it->second->setWorldTransform(worldMatrix);

#ifdef DEBUG_PHYSICS
	std::cout << "[" << collider->name() << "]\tsynchro graphics->physics" << std::endl;

	btTransform bulletTransform;
	it->second->rigidBody()->getMotionState()->getWorldTransform(bulletTransform);
	print(std::cout << "- motionstate.worldTransform = ", bulletTransform) << std::endl;

	print(std::cout << "- rigidbody.worldTransform = ", it->second->rigidBody()->getWorldTransform()) << std::endl;
#endif // DEBUG_PHYSICS
}

/*
void
bullet::PhysicsWorld::setPhysicsTransformFromCollider(Collider::Ptr collider)
{
	auto it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		return;

	it->second->setWorldTransform(collider->worldTransform());
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
*/

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
bullet::PhysicsWorld::removeScalingShear(Matrix4x4::Ptr input, 
										 Matrix4x4::Ptr output, 
										 Matrix4x4::Ptr correction)
{
	auto decompQR = Matrix4x4::create()
		->copyFrom(input)
		->appendTranslation(-(*input->translationVector()))
		->decomposeQR();

	if (correction != nullptr)
		correction->copyFrom(decompQR.second);

	if (output == nullptr)
		output = Matrix4x4::create();
	output
		->copyFrom(decompQR.first)
		->appendTranslation(input->translationVector());
		
	return output;
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
		transform->rotationQuaternion(), 
		transform->translationVector(), 
		output
	);
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

/*static*/
std::ostream&
bullet::PhysicsWorld::print(std::ostream& out, const btTransform& bulletTransform)
{
	const btVector3& origin(bulletTransform.getOrigin());
	const btMatrix3x3& basis(bulletTransform.getBasis());

	out << "\t- origin\t= [" << origin[0] << "\t" << origin[1] << "\t" << origin[2] << "]\n\t- basis \t=\n" 
		<< "\t[" << basis[0][0] << "\t" << basis[0][1] << "\t" << basis[0][2] 
		<< "\n\t " << basis[1][0] << "\t" << basis[1][1] << "\t" << basis[1][2] 
		<< "\n\t " << basis[2][0] << "\t" << basis[2][1] << "\t" << basis[2][2] << "]";

	return out;
}