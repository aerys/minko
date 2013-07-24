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
#include <minko/math/Vector3.hpp>
#include <minko/math/Quaternion.hpp>
#include <minko/math/Matrix4x4.hpp>
#include <minko/component/bullet/Collider.hpp>
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
bullet::PhysicsWorld::BulletCollider::initialize(Collider::Ptr collider)
{
	if (collider == nullptr)
		throw std::invalid_argument("collider");

	initializeCollisionShape(collider->shape());

#ifdef DEBUG_PHYSICS
	std::cout << "[" << collider->name() << "]\tinit collision shape\n\t- local scaling = " << _bulletCollisionShape->getLocalScaling()[0] 
	<< "\n\t- margin = " << _bulletCollisionShape->getMargin() << std::endl;
#endif // DEBUG_PHYSICS

	initializeMotionState(collider);
	initializeCollisionObject(collider);
}

std::shared_ptr<btCollisionShape>
bullet::PhysicsWorld::BulletCollider::initializeCollisionShape(AbstractPhysicsShape::Ptr shape) const
{
	if (shape == nullptr)
		throw std::invalid_argument("shape");

	std::shared_ptr<btCollisionShape> shape = nullptr;
	switch(shape->type())
	{
	case AbstractPhysicsShape::SPHERE:
		shape = initializeSphereShape(std::dynamic_pointer_cast<SphereShape>(shape));
		break;

	case AbstractPhysicsShape::BOX:
		shape = initializeBoxShape(std::dynamic_pointer_cast<BoxShape>(shape));
		break;

	case AbstractPhysicsShape::CONE:
		shape = initializeConeShape(std::dynamic_pointer_cast<ConeShape>(shape));
		break;

	case AbstractPhysicsShape::CYLINDER:
		shape = initializeCylinderShape(std::dynamic_pointer_cast<CylinderShape>(shape));
		break;

	default:
		throw std::logic_error("Unsupported physics shape");
	}

	shape->setLocalScaling(btVector3(
		shape->localScaling(), 
		shape->localScaling(), 
		shape->localScaling()
	));
	shape->setMargin(shape->margin());

	return shape;
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
bullet::PhysicsWorld::BulletCollider::initializeMotionState(Collider::Ptr collider) const
{
	// collider's starting world transform
	//Matrix4x4Ptr startWorld = Matrix4x4::create()
	//	->copyFrom(collider->worldTransform());
		//->appendTranslation(collider->shape()->centerOfMassTranslation()->x(),
		//					collider->shape()->centerOfMassTranslation()->y(),
		//					collider->shape()->centerOfMassTranslation()->z());

//	btTransform bulletStartTransform;
	//toBulletTransform(startWorld, bulletStartTransform);

	// collider's center-of-mass offset transform
	btTransform bulletOffsetTransform;

	Quaternion::Ptr offRotation = collider->shape()->centerOfMassInverseTransform()->rotationQuaternion();
	Vector3::Ptr	offTransl = collider->shape()->centerOfMassInverseTransform()->translationVector();

	/*
	Matrix4x4::Ptr	offTransf = Matrix4x4::create()
		->copyFrom(startWorld)->invert()
		->append(offRotation)
		->append(startWorld)
		->appendTranslation(offTransl)
		->invert();
		*/

	toBulletTransform(
		collider->shape()->centerOfMassOffset(),
		bulletOffsetTransform
	);

	/*
	auto	offsetTranslation	= Vector3::create(0.f, 0.f, 0.f);
	auto	offsetRotation		= collider->shape()->centerOfMassRotation();

	btTransform bulletOffsetTransform;
	toBulletTransform(
		collider->shape()->centerOfMassRotation(),
		collider->shape()->centerOfMassTranslation(),
		bulletOffsetTransform
	);
	*/

#ifdef DEBUG_PHYSICS
	std::cout << "[" << collider->name() << "]\tinit motion state" << std::endl;
	//print(std::cout << "- startTrans =\n", bulletStartTransform) << std::endl;
	print(std::cout << "- centerOfMassOffset =\n", bulletOffsetTransform) << std::endl;
#endif // DEBUG_PHYSICS


	return std::shared_ptr<btMotionState>(new btDefaultMotionState(
		btTransform(),
		bulletOffsetTransform
		));

	/*
	auto worldTransform = Matrix4x4::create()
		->copyFrom(collider->shape()->centerOfMassOffsetInverse())
		->append(collider->worldTransform());

	setWorldTransform(worldTransform);
	*/
//	toBulletTransform(worldTransform, bulletStartTransform);
//	_bulletMotionState->setWorldTransform(bulletStartTransform);
}

void
bullet::PhysicsWorld::BulletCollider::initializeCollisionObject(Collider::Ptr collider)
{
	// only rigid objects are considerered for the moment

	btVector3 inertia (0.0, 0.0, 0.0);
	if (collider->inertia() == nullptr)
	{
		if (collider->mass() > 0.0f)
			_bulletCollisionShape->calculateLocalInertia(collider->mass(), inertia);
	}
	else
	{
		inertia.setX(collider->inertia()->x());
		inertia.setY(collider->inertia()->y());
		inertia.setZ(collider->inertia()->z());
	}

	// construction of a new rigid collision object
	auto info = btRigidBody::btRigidBodyConstructionInfo(
		collider->mass(),
		_bulletMotionState.get(),
		_bulletCollisionShape.get(),
		inertia
	);
	info.m_linearDamping			= collider->linearDamping();
	info.m_angularDamping			= collider->angularDamping();
	info.m_friction					= collider->friction();
	info.m_rollingFriction			= collider->rollingFriction();
	info.m_restitution				= collider->restitution();
	info.m_linearSleepingThreshold	= collider->linearSleepingThreshold();
	info.m_angularSleepingThreshold	= collider->angularSleepingThreshold();

	auto bulletRigidBody = std::shared_ptr<btRigidBody>(new btRigidBody(info));

	// communicate several properties of the rigid object
	bulletRigidBody->setLinearVelocity(btVector3(
		collider->linearVelocity()->x(), 
		collider->linearVelocity()->y(), 
		collider->linearVelocity()->z()
		));
	bulletRigidBody->setLinearFactor(btVector3(
		collider->linearFactor()->x(), 
		collider->linearFactor()->y(), 
		collider->linearFactor()->z()
		));
	bulletRigidBody->setAngularVelocity(btVector3(
		collider->angularVelocity()->x(), 
		collider->angularVelocity()->y(), 
		collider->angularVelocity()->z()
		));
	bulletRigidBody->setAngularFactor(btVector3(
		collider->angularFactor()->x(), 
		collider->angularFactor()->y(), 
		collider->angularFactor()->z()
		));

	bulletRigidBody->setActivationState(collider->deactivationDisabled() 
		? DISABLE_DEACTIVATION 
		: ACTIVE_TAG
	);

	_bulletCollisionObject	= bulletRigidBody;
}

void
bullet::PhysicsWorld::BulletCollider::setLinearVelocity(Vector3::Ptr velocity)
{
	std::shared_ptr<btRigidBody> bulletRigidBody = std::dynamic_pointer_cast<btRigidBody>(_bulletCollisionObject);
	bulletRigidBody->setLinearVelocity(btVector3(velocity->x(), velocity->y(), velocity->z()));
}

void 
bullet::PhysicsWorld::BulletCollider::setWorldTransform(Matrix4x4::Ptr worldTransform)
{
	btTransform bulletTransform;
	toBulletTransform(worldTransform, bulletTransform);

#ifdef DEBUG_PHYSICS
	std::cout << "BulletCollider::setWorldTransform\n" << std::to_string(worldTransform) << std::endl;
#endif // DEBUG_PHYSICS

	auto bulletRigidBody = rigidBody();
	if (bulletRigidBody == nullptr)
		_bulletMotionState->setWorldTransform(bulletTransform);
	else
	{
		bulletRigidBody->getMotionState()->setWorldTransform(bulletTransform);

		bulletRigidBody->getMotionState()->getWorldTransform(bulletTransform);
		bulletRigidBody->setWorldTransform(bulletTransform);
	}
}

void
bullet::PhysicsWorld::BulletCollider::applyRelativeImpulse(Vector3::Ptr relativeImpulse)
{
	std::shared_ptr<btRigidBody> bulletRigidBody = std::dynamic_pointer_cast<btRigidBody>(_bulletCollisionObject);

	btVector3 btRelImpulse(relativeImpulse->x(), relativeImpulse->y(), relativeImpulse->z());

	bulletRigidBody->applyImpulse(
		bulletRigidBody->getWorldTransform().getBasis() * btRelImpulse, 
		btVector3(0.0f, 0.0f, 0.0f)
	);
}

void
bullet::PhysicsWorld::BulletCollider::prependLocalTranslation(Vector3::Ptr relTranslation)
{
	std::shared_ptr<btRigidBody> bulletRigidBody = std::dynamic_pointer_cast<btRigidBody>(_bulletCollisionObject);

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

	std::shared_ptr<btRigidBody> bulletRigidBody = std::dynamic_pointer_cast<btRigidBody>(_bulletCollisionObject);

	btTransform btNewTransform;
	btNewTransform.setBasis(bulletRigidBody->getWorldTransform().getBasis() * btRotation);
	btNewTransform.setOrigin(bulletRigidBody->getWorldTransform().getOrigin());

	bulletRigidBody->setWorldTransform(btNewTransform);
}

bullet::PhysicsWorld::BulletCollider::Ptr
bullet::PhysicsWorld::BulletCollider::create(Collider::Ptr collider)
{
	BulletColliderPtr bulletCollider(new BulletCollider());

	bulletCollider->initialize(collider);

	return bulletCollider;
}

