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

#include "minko/component/bullet/PhysicsWorld.hpp"

#include <btBulletDynamicsCommon.h>
#include <minko/math/Vector3.hpp>
#include <minko/math/Quaternion.hpp>
#include <minko/math/Matrix4x4.hpp>
#include <minko/component/bullet/ColliderData.hpp>
#include <minko/component/bullet/AbstractPhysicsShape.hpp>
#include <minko/component/bullet/SphereShape.hpp>
#include <minko/component/bullet/BoxShape.hpp>
#include <minko/component/bullet/ConeShape.hpp>
#include <minko/component/bullet/CylinderShape.hpp>

using namespace minko;
using namespace minko::math;
using namespace minko::component;

bullet::PhysicsWorld::BulletCollider::BulletCollider():
	_bulletCollisionShape(nullptr),
	_bulletMotionState(nullptr),
	_bulletCollisionObject(nullptr)
{
}

std::shared_ptr<btRigidBody>
bullet::PhysicsWorld::BulletCollider::rigidBody() const
{
	return std::dynamic_pointer_cast<btRigidBody>(_bulletCollisionObject);
}

void
bullet::PhysicsWorld::BulletCollider::initialize(ColliderData::Ptr data)
{
	if (data == nullptr)
		throw std::invalid_argument("data");

	std::shared_ptr<btCollisionShape>	bulletCollisionShape	= initializeCollisionShape(data->shape());	
	std::shared_ptr<btMotionState>		bulletMotionState		= initializeMotionState(data);

#ifdef DEBUG_PHYSICS
	std::cout << "[" << data->name() << "]\tinit collision shape\n\t- local scaling = " << bulletCollisionShape->getLocalScaling()[0] 
	<< "\n\t- margin = " << bulletCollisionShape->getMargin() << std::endl;
#endif // DEBUG_PHYSICS

	initializeCollisionObject(
		data, 
		bulletCollisionShape, 
		bulletMotionState
	);
}

std::shared_ptr<btCollisionShape>
bullet::PhysicsWorld::BulletCollider::initializeCollisionShape(AbstractPhysicsShape::Ptr shape) const
{
	if (shape == nullptr)
		throw std::invalid_argument("shape");

	std::shared_ptr<btCollisionShape> bulletShape = nullptr;
	switch(shape->type())
	{
	case AbstractPhysicsShape::SPHERE:
		bulletShape = initializeSphereShape(std::dynamic_pointer_cast<SphereShape>(shape));
		break;

	case AbstractPhysicsShape::BOX:
		bulletShape = initializeBoxShape(std::dynamic_pointer_cast<BoxShape>(shape));
		break;

	case AbstractPhysicsShape::CONE:
		bulletShape = initializeConeShape(std::dynamic_pointer_cast<ConeShape>(shape));
		break;

	case AbstractPhysicsShape::CYLINDER:
		bulletShape = initializeCylinderShape(std::dynamic_pointer_cast<CylinderShape>(shape));
		break;

	default:
		throw std::logic_error("Unsupported physics shape");
	}

	bulletShape->setLocalScaling(btVector3(
		shape->localScaling()->x(), 
		shape->localScaling()->y(), 
		shape->localScaling()->z()
	));

	bulletShape->setMargin(shape->margin());

	return bulletShape;
}

std::shared_ptr<btCollisionShape>
bullet::PhysicsWorld::BulletCollider::initializeSphereShape(SphereShape::Ptr sphere) const
{
	return std::shared_ptr<btSphereShape>(new btSphereShape(sphere->radius()));
}

std::shared_ptr<btCollisionShape>
bullet::PhysicsWorld::BulletCollider::initializeBoxShape(BoxShape::Ptr box) const
{
	btVector3 halfExtents (box->halfExtentX(), box->halfExtentY(), box->halfExtentZ());

	return std::shared_ptr<btBoxShape>(new btBoxShape(halfExtents));
}

std::shared_ptr<btCollisionShape>
bullet::PhysicsWorld::BulletCollider::initializeConeShape(ConeShape::Ptr cone) const
{
	return std::shared_ptr<btConeShape>(new btConeShape(cone->radius(), cone->height()));
}

std::shared_ptr<btCollisionShape>
bullet::PhysicsWorld::BulletCollider::initializeCylinderShape(CylinderShape::Ptr cylinder) const
{
	btVector3 halfExtents (cylinder->halfExtentX(), cylinder->halfExtentY(), cylinder->halfExtentZ());

	return std::shared_ptr<btCylinderShape>(new btCylinderShape(halfExtents));
}

std::shared_ptr<btMotionState>
bullet::PhysicsWorld::BulletCollider::initializeMotionState(ColliderData::Ptr collider) const
{
	return std::shared_ptr<btMotionState>(new btDefaultMotionState(
		btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(0.0f, 0.0f, 0.0f)),
		btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(0.0f, 0.0f, 0.0f))
	));
}

void
bullet::PhysicsWorld::BulletCollider::initializeCollisionObject(ColliderData::Ptr data,
																std::shared_ptr<btCollisionShape> bulletCollisionShape, 
																std::shared_ptr<btMotionState> bulletMotionState) 
{
	// only rigid objects are considerered for the moment

	btVector3 inertia (0.0, 0.0, 0.0);
	if (data->inertia() == nullptr)
	{
		if (data->mass() > 0.0f)
			bulletCollisionShape->calculateLocalInertia(data->mass(), inertia);
	}
	else
	{
		inertia.setX(data->inertia()->x());
		inertia.setY(data->inertia()->y());
		inertia.setZ(data->inertia()->z());
	}

	// construction of a new rigid collision object
	auto info = btRigidBody::btRigidBodyConstructionInfo(
		data->mass(),
		bulletMotionState.get(),
		bulletCollisionShape.get(),
		inertia
	);
	info.m_linearDamping			= data->linearDamping();
	info.m_angularDamping			= data->angularDamping();
	info.m_friction					= data->friction();
	info.m_rollingFriction			= data->rollingFriction();
	info.m_restitution				= data->restitution();
	info.m_linearSleepingThreshold	= data->linearSleepingThreshold();
	info.m_angularSleepingThreshold	= data->angularSleepingThreshold();

	auto bulletRigidBody = std::shared_ptr<btRigidBody>(new btRigidBody(info));

	// communicate several properties of the rigid object
	bulletRigidBody->setLinearVelocity(btVector3(
		data->linearVelocity()->x(), 
		data->linearVelocity()->y(), 
		data->linearVelocity()->z()
		));
	bulletRigidBody->setLinearFactor(btVector3(
		data->linearFactor()->x(), 
		data->linearFactor()->y(), 
		data->linearFactor()->z()
		));
	bulletRigidBody->setAngularVelocity(btVector3(
		data->angularVelocity()->x(), 
		data->angularVelocity()->y(), 
		data->angularVelocity()->z()
		));
	bulletRigidBody->setAngularFactor(btVector3(
		data->angularFactor()->x(), 
		data->angularFactor()->y(), 
		data->angularFactor()->z()
		));

	bulletRigidBody->setActivationState(data->deactivationDisabled() 
		? DISABLE_DEACTIVATION 
		: ACTIVE_TAG
	);

	_bulletCollisionShape	= bulletCollisionShape;
	_bulletMotionState		= bulletMotionState;
	_bulletCollisionObject	= bulletRigidBody;
}

void
bullet::PhysicsWorld::BulletCollider::setLinearVelocity(Vector3::Ptr velocity)
{
	std::shared_ptr<btRigidBody> bulletRigidBody = std::dynamic_pointer_cast<btRigidBody>(_bulletCollisionObject);
	bulletRigidBody->setLinearVelocity(btVector3(velocity->x(), velocity->y(), velocity->z()));
}

void 
bullet::PhysicsWorld::BulletCollider::setWorldTransform(Matrix4x4::Ptr physicsNoScaleTransform)
{
#ifdef DEBUG_PHYSICS
	const float scaling = powf(fabsf(physicsNoScaleTransform->determinant3x3()), 1.0f/3.0f);
	if (fabsf(scaling - 1.0f) > 1e-3f)
		throw std::logic_error("World transforms sent to Bullet must not contain any scaling.");
#endif // DEBUG_PHYSICS

	btTransform bulletTransform;
	toBulletTransform(physicsNoScaleTransform, bulletTransform);

	auto bulletRigidBody = rigidBody();
	if (bulletRigidBody != nullptr)
	{

#ifdef DEBUG_PHYSICS
	PhysicsWorld::print(std::cout << "BulletCollider::setWorldTransform\n-physicsTrf = \n", bulletTransform) << std::endl;
#endif // DEBUG_PHYSICS

		bulletRigidBody->getMotionState()->setWorldTransform(bulletTransform);

		bulletRigidBody->getMotionState()->getWorldTransform(bulletTransform);
#ifdef DEBUG_PHYSICS
	PhysicsWorld::print(std::cout << "BulletCollider::setWorldTransform\n-motionstate.worldTrf = \n", bulletTransform) << std::endl;
#endif // DEBUG_PHYSICS

		bulletRigidBody->setWorldTransform(bulletTransform);
	}
}

void
bullet::PhysicsWorld::BulletCollider::applyRelativeImpulse(Vector3::Ptr relativeImpulse)
{
	auto bulletRigidBody = rigidBody();

	btVector3 btRelImpulse(relativeImpulse->x(), relativeImpulse->y(), relativeImpulse->z());

	bulletRigidBody->applyImpulse(
		bulletRigidBody->getWorldTransform().getBasis() * btRelImpulse, 
		btVector3(0.0f, 0.0f, 0.0f)
	);
}

void
bullet::PhysicsWorld::BulletCollider::prependLocalTranslation(Vector3::Ptr relTranslation)
{
	auto bulletRigidBody = rigidBody();

	btVector3 btRelTranslation(relTranslation->x(), relTranslation->y(), relTranslation->z());
	btVector3 btTranslation = bulletRigidBody->getWorldTransform().getBasis() * btRelTranslation;

	btTransform btNewTransform;
	btNewTransform.setBasis(bulletRigidBody->getWorldTransform().getBasis());
	btNewTransform.setOrigin(bulletRigidBody->getWorldTransform().getOrigin() + btTranslation);

	bulletRigidBody->setWorldTransform(btNewTransform);
}

void
bullet::PhysicsWorld::BulletCollider::prependRotationY(float radians)
{
	btMatrix3x3	btRotation (btQuaternion(btVector3(0.0f, 1.0f, 0.0f), radians));

	auto bulletRigidBody = rigidBody();

	btTransform btNewTransform;
	btNewTransform.setBasis(bulletRigidBody->getWorldTransform().getBasis() * btRotation);
	btNewTransform.setOrigin(bulletRigidBody->getWorldTransform().getOrigin());

	bulletRigidBody->setWorldTransform(btNewTransform);
}

bullet::PhysicsWorld::BulletCollider::Ptr
bullet::PhysicsWorld::BulletCollider::create(ColliderData::Ptr collider)
{
	BulletColliderPtr bulletCollider(new BulletCollider());

	bulletCollider->initialize(collider);

	return bulletCollider;
}

