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
#include <minko/math/Matrix4x4.hpp>
#include <minko/scene/Node.hpp>
#include <minko/scene/NodeSet.hpp>
#include <minko/component/SceneManager.hpp>
#include <minko/component/Renderer.hpp>
#include <minko/component/bullet/LinearIdAllocator.hpp>
#include <minko/component/bullet/ColliderData.hpp>
#include <minko/component/bullet/AbstractPhysicsShape.hpp>

using namespace minko;
using namespace minko::math;
using namespace minko::scene;
using namespace minko::component;

/*static*/
const uint		bullet::PhysicsWorld::_MAX_BODIES	= 2048;
/*static*/
Matrix4x4::Ptr	bullet::PhysicsWorld::_TMP_MATRIX	= Matrix4x4::create();
/*static*/
btTransform		bullet::PhysicsWorld::_TMP_BTTRANSFORM;

bullet::PhysicsWorld::PhysicsWorld():
	AbstractComponent(),
	_uidAllocator(LinearIdAllocator::create(_MAX_BODIES)),
	_colliderMap(),
	_colliderReverseMap(),
	_uidToCollider(),
	_collisions(),
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
	_exitFrameSlot = nullptr;

	_colliderMap.clear();
	_colliderReverseMap.clear();
	_uidToCollider.clear();
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
bullet::PhysicsWorld::addChild(ColliderData::Ptr data)
{
	if (hasCollider(data))
		throw std::logic_error("The same data cannot be added twice.");

	data->uid(_uidAllocator->allocate());

	_uidToCollider.insert(std::pair<uint, ColliderData::Ptr>(data->uid(), data));

	BulletCollider::Ptr bulletCollider = BulletCollider::create(data);
	_colliderMap.insert(std::pair<ColliderData::Ptr, BulletCollider::Ptr>(data, bulletCollider));
	_colliderReverseMap.insert(std::pair<btCollisionObject*, ColliderData::Ptr>(bulletCollider->rigidBody().get(), data));

	std::dynamic_pointer_cast<btDiscreteDynamicsWorld>(_bulletDynamicsWorld)
		->addRigidBody(bulletCollider->rigidBody().get());

#ifdef DEBUG_PHYSICS
	std::cout << "[" << data->name() << "]\tadd physics body" << std::endl;

	print(std::cout << "rigidbody.worldTransform =\n", bulletCollider->rigidBody()->getWorldTransform()) << std::endl;
#endif // DEBUG_PHYSICS
}

void
bullet::PhysicsWorld::removeChild(ColliderData::Ptr data)
{
	ColliderMap::const_iterator	it	= _colliderMap.find(data);
	if (it == _colliderMap.end())
		throw std::invalid_argument("data");

	btCollisionObject*	bulletObject = it->second->rigidBody().get();

	ColliderReverseMap::const_iterator invIt = _colliderReverseMap.find(bulletObject);
	if (invIt == _colliderReverseMap.end())
		throw std::invalid_argument("data");

	_bulletDynamicsWorld->removeCollisionObject(bulletObject);

	std::map<unsigned int, ColliderData::Ptr>::const_iterator uidIt = _uidToCollider.find(data->uid());
	if (uidIt == _uidToCollider.end())
		throw std::invalid_argument("data");

	_uidAllocator->free(data->uid());

	_colliderMap.erase(it);
	_colliderReverseMap.erase(invIt);
	_uidToCollider.erase(uidIt);
}

bool
bullet::PhysicsWorld::hasCollider(ColliderData::Ptr data) const
{
	return _colliderMap.find(data) != _colliderMap.end();
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
	notifyCollisions();
}

void
bullet::PhysicsWorld::updateColliders()
{
	for (ColliderMap::iterator it = _colliderMap.begin(); it != _colliderMap.end(); ++it)
	{
		ColliderData::Ptr	collider(it->first);
		if (collider->isStatic())
			continue;

		fromBulletTransform(
			it->second->rigidBody()->getWorldTransform(),
			_TMP_MATRIX
		);
		// _TMP_MATRIX = physicsTransform

		_TMP_MATRIX
			->prepend(collider->shape()->deltaTransformInverse())
			->prepend(collider->correction());
		// _TMP_MATRIX = graphicsTransform = physicsTransform * deltaInverse * correction

		collider->graphicsWorldTransformChanged()
			->execute(collider, _TMP_MATRIX);
	}
}

void
bullet::PhysicsWorld::notifyCollisions()
{
	CollisionSet currentCollisions;

	ColliderData::Ptr colliderData[2]	= { nullptr, nullptr };
	const int numManifolds				= _bulletDynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; ++i)
	{
		btPersistentManifold* manifold	= _bulletDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

		ColliderReverseMap::const_iterator	it = _colliderReverseMap.find(manifold->getBody0());
		if (it == _colliderReverseMap.end())
			throw std::logic_error("collision started between untracked colliders.");
		colliderData[0]	= it->second;

		it = _colliderReverseMap.find(manifold->getBody1());
		if (it == _colliderReverseMap.end())
			throw std::logic_error("collision started between untracked colliders.");
		colliderData[1]	= it->second;

		if (!colliderData[0]->triggerCollisions() 
			&& !colliderData[1]->triggerCollisions())
			continue;
		
		const uint				uid0	= colliderData[0]->uid();
		const uint				uid1	= colliderData[1]->uid();
		std::pair<uint, uint>	collision(uid0, uid1);
		if (uid0 > uid1)
		{
			collision.first		= uid1;
			collision.second	= uid0;
		}

		if (_collisions.find(collision) == _collisions.end())
		{
			colliderData[0]->collisionStarted()->execute(colliderData[0], colliderData[1]);
			colliderData[1]->collisionStarted()->execute(colliderData[1], colliderData[0]);
		}
		currentCollisions.insert(collision);
	}

	// find and notify collisions that are not present anymore as the difference with the intersection
	// between the previous collision set and the current one
	CollisionSet lostCollisions;
	std::set_difference(
		_collisions.begin(), _collisions.end(), 
		currentCollisions.begin(), currentCollisions.end(), 
		std::inserter(lostCollisions, lostCollisions.end())
		);

	for (CollisionSet::const_iterator it = lostCollisions.begin(); it != lostCollisions.end(); ++it)
	{
		std::map<uint, ColliderData::Ptr>::const_iterator uidIt = _uidToCollider.find(it->first);
		if (uidIt == _uidToCollider.end())
			throw std::logic_error("collision ended between untracked colliders.");
		colliderData[0] = uidIt->second;

		uidIt = _uidToCollider.find(it->second);
		if (uidIt == _uidToCollider.end())
			throw std::logic_error("collision ended between untracked colliders.");
		colliderData[1] = uidIt->second;

		colliderData[0]->collisionEnded()->execute(colliderData[0], colliderData[1]);
		colliderData[1]->collisionEnded()->execute(colliderData[1], colliderData[0]);
	}
	
	_collisions.swap(currentCollisions);
}

void
bullet::PhysicsWorld::synchronizePhysicsWithGraphics(ColliderDataPtr collider, 
													 Matrix4x4::Ptr graphicsNoScaleTransform)
{
	auto it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		return;

#ifdef DEBUG_PHYSICS
	const float det3x3 = fabsf(graphicsNoScaleTransform->determinant3x3());
	if (fabsf(det3x3 - 1.0f) > 1e-3f)
		throw std::logic_error("Graphics world matrices used for physics synchronization must be pure rotation + translation matrices.");
#endif // DEBUG_PHYSICS

	// update the motion state's center of mass offset transform
	btDefaultMotionState* bulletMotionState = dynamic_cast<btDefaultMotionState*>(it->second->rigidBody()->getMotionState());
	if (bulletMotionState == nullptr)
		return;

	_TMP_MATRIX
		->copyFrom(graphicsNoScaleTransform)->invert()
		->append(collider->shape()->deltaTransform())
		->append(graphicsNoScaleTransform)
		->invert();

	toBulletTransform(
		_TMP_MATRIX, 
		bulletMotionState->m_centerOfMassOffset
	);

	// update the motion state's world transform
	toBulletTransform(
		graphicsNoScaleTransform,
		bulletMotionState->m_graphicsWorldTrans
	);

	// synchronize bullet
	bulletMotionState->getWorldTransform(_TMP_BTTRANSFORM);
	it->second->rigidBody()->setWorldTransform(_TMP_BTTRANSFORM);

#ifdef DEBUG_PHYSICS
	std::cout << "[" << it->first->name() << "] synchro graphics -> physics" << std::endl;

	print(std::cout << "- scalefree(graphics) = \n", graphicsNoScaleTransform) << std::endl;
	print(std::cout << "- motionstate.offset = \n", bulletMotionState->m_centerOfMassOffset) << std::endl;
	print(std::cout << "- rigidbody.worldtransform = \n", it->second->rigidBody()->getWorldTransform()) << std::endl;
#endif // DEBUG_PHYSICS
}

/*static*/
Matrix4x4::Ptr
bullet::PhysicsWorld::removeScalingShear(Matrix4x4::Ptr input, 
										 Matrix4x4::Ptr output, 
										 Matrix4x4::Ptr correction)
{
	auto translation	= input->translation();

	// remove translational component, then perform QR decomposition
	_TMP_MATRIX
		->copyFrom(input)
		->appendTranslation(-(*translation));

	if (output == nullptr)
		output = Matrix4x4::create();
	if (correction == nullptr)
		correction = Matrix4x4::create();

	_TMP_MATRIX->decomposeQR(output, correction);

	return output->appendTranslation(translation);
}

/*static*/
Matrix4x4::Ptr
bullet::PhysicsWorld::fromBulletTransform(const btTransform& transform,
										  Matrix4x4::Ptr output)
{
	auto basis			= transform.getBasis();
	auto translation	= transform.getOrigin();

	if (output == nullptr)
		output = Matrix4x4::create();

	return output->initialize(
		basis[0][0], basis[0][1], basis[0][2], translation[0],
		basis[1][0], basis[1][1], basis[1][2], translation[1],
		basis[2][0], basis[2][1], basis[2][2], translation[2],
		0.0f, 0.0f, 0.0f, 1.0f
		);
}

/*static*/
void
bullet::PhysicsWorld::toBulletTransform(Matrix4x4::Ptr transform,
										btTransform& output)
{
	toBulletTransform(
		transform->rotationQuaternion(), 
		transform->translation(), 
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

/*static*/
std::ostream&
bullet::PhysicsWorld::print(std::ostream& out, Matrix4x4Ptr matrix)
{
	const std::vector<float>& m(matrix->values());

	out << "\t- origin\t= [" << m[3] << "\t" << m[7] << "\t" << m[11] << "]\n\t- basis \t=\n" 
		<< "\t[" << m[0] << "\t" << m[1] << "\t" << m[2] 
		<< "\n\t " << m[4] << "\t" << m[5] << "\t" << m[6] 
		<< "\n\t " << m[8] << "\t" << m[9] << "\t" << m[10] << "]";

	return out;
}