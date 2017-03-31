/*
Copyright (c) 2014 Aerys

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
#include <minko/scene/Node.hpp>
#include <minko/scene/NodeSet.hpp>
#include <minko/component/SceneManager.hpp>
#include <minko/component/Renderer.hpp>
#include <minko/component/bullet/LinearIdAllocator.hpp>
#include <minko/component/bullet/ColliderData.hpp>
#include <minko/component/bullet/Collider.hpp>
#include <minko/component/bullet/AbstractPhysicsShape.hpp>

#include "minko/math/tools.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;

/*static*/ const uint bullet::PhysicsWorld::_MAX_BODIES = 2048;


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
    _frameBeginSlot(nullptr),
    _frameEndSlot(nullptr),
    _componentAddedOrRemovedSlot(nullptr),
    _addedOrRemovedSlot(nullptr),
    _colliderNodeLayoutChangedSlot(),
    _colliderPropertiesChangedSlot(),
    _colliderLayoutMaskChangedSlot(),
    _paused(false),
    _maxNumSteps(0),
    _baseFramerate(60.0f)
{
}


void
bullet::PhysicsWorld::initialize()
{
    // straightforward physics world initialization for the time being
    _bulletBroadphase = std::shared_ptr<btDbvtBroadphase>(new btDbvtBroadphase());
    _bulletCollisionConfiguration = std::shared_ptr<btDefaultCollisionConfiguration>(new btDefaultCollisionConfiguration());
    _bulletConstraintSolver = std::shared_ptr<btSequentialImpulseConstraintSolver>(new btSequentialImpulseConstraintSolver());
    _bulletDispatcher = std::shared_ptr<btCollisionDispatcher>(new btCollisionDispatcher(_bulletCollisionConfiguration.get()));

    _bulletDynamicsWorld = std::shared_ptr<btDiscreteDynamicsWorld>(new btDiscreteDynamicsWorld(
        _bulletDispatcher.get(),
        _bulletBroadphase.get(),
        _bulletConstraintSolver.get(),
        _bulletCollisionConfiguration.get()
        ));
}

void
bullet::PhysicsWorld::targetAdded(Node::Ptr target)
{    
    setSceneManager(target->root()->component<SceneManager>());
}

void
bullet::PhysicsWorld::targetRemoved(Node::Ptr target)
{
    _sceneManager = nullptr;
    _frameBeginSlot = nullptr;
    _frameEndSlot = nullptr;
    _addedOrRemovedSlot = nullptr;
    _componentAddedOrRemovedSlot = nullptr;
    _exitFrameSlot = nullptr;

    _colliderMap.clear();
    _colliderReverseMap.clear();
    _uidToCollider.clear();
    _colliderNodeLayoutChangedSlot.clear();
    _colliderPropertiesChangedSlot.clear();
    _colliderLayoutMaskChangedSlot.clear();
}

void
bullet::PhysicsWorld::setSceneManager(std::shared_ptr<SceneManager> sceneManager)
{
    if (sceneManager != _sceneManager || (!_componentAddedOrRemovedSlot && !_addedOrRemovedSlot))
    {
        auto componentCallback = [&](Node::Ptr target, Node::Ptr node, AbstractComponent::Ptr cmp)
        {
            setSceneManager(target->root()->component<SceneManager>());
        };

        auto nodeCallback = [&](Node::Ptr target, Node::Ptr node, Node::Ptr ancestor)
        {
            setSceneManager(target->root()->component<SceneManager>());
        };

        if (sceneManager)
        {
            _sceneManager = sceneManager;

            _frameBeginSlot = sceneManager->frameBegin()->connect(std::bind(
                &PhysicsWorld::frameBeginHandler,
                std::static_pointer_cast<PhysicsWorld>(shared_from_this()),
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
            ));

            _frameEndSlot = sceneManager->frameEnd()->connect(std::bind(
                &PhysicsWorld::frameEndHandler,
                std::static_pointer_cast<PhysicsWorld>(shared_from_this()),
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
            ));

            _componentAddedOrRemovedSlot = target()->componentRemoved().connect(componentCallback);
            _addedOrRemovedSlot = target()->removed().connect(nodeCallback);
        }
        else
        {
            _sceneManager = nullptr;
            _frameBeginSlot = nullptr;
            _frameEndSlot = nullptr;

            _componentAddedOrRemovedSlot = target()->componentAdded().connect(componentCallback);
            _addedOrRemovedSlot = target()->added().connect(nodeCallback);
        }
    }
}

void
bullet::PhysicsWorld::addCollider(Collider::Ptr collider)
{
    if (collider == nullptr || collider->target() == nullptr)
        throw std::invalid_argument("collider");

    if (hasCollider(collider))
        throw std::logic_error("The same collider cannot be added twice to the physics world.");

    //std::cout << "PhysicsWorld::addChild\tnode '" << data->node()->name()
    //    << "'\tgroup = " << std::bitset<16>(data->collisionGroup())
    //    << "\tmask = " << std::bitset<16>(data->collisionMask())
    //    << std::endl;

    auto uid = _uidAllocator->allocate();
    auto bulletCollider = BulletCollider::create(collider);
    auto rigidBody = bulletCollider->rigidBody().get();

    collider->uid(uid);

    _uidToCollider[uid] = collider;
    _colliderMap[collider] = bulletCollider;
    _colliderReverseMap[rigidBody] = collider;

    _colliderNodeLayoutChangedSlot[collider] = collider->target()->layoutChanged().connect([=](Node::Ptr, Node::Ptr){ updateColliderNodeProperties(collider); });
    _colliderPropertiesChangedSlot[collider] = collider->propertiesChanged()->connect([=](Collider::Ptr){ updateColliderProperties(collider); });
    _colliderLayoutMaskChangedSlot[collider] = collider->layoutMaskChanged().connect([=](AbstractComponent::Ptr){ updateColliderLayoutMask(collider); });

    std::dynamic_pointer_cast<btDiscreteDynamicsWorld>(_bulletDynamicsWorld)
        ->addRigidBody(
            rigidBody,
            short(collider->target()->layout() & ((1<<16) - 1)),
            short(collider->layoutMask() & ((1<<16) - 1))
         );

    updateColliderProperties(collider);
    updateColliderNodeProperties(collider);

#ifdef DEBUG_PHYSICS
    std::cout << "[" << target()->name() << "]\tadd physics body" << std::endl;

    auto matrix = math::fromBulletTransform(bulletCollider->rigidBody()->getWorldTransform());

    std::cout << "rigidbody.worldTransform =\n" << std::to_string(matrix) << std::endl;
#endif // DEBUG_PHYSICS
}

//void
//bullet::PhysicsWorld::updateCollisionFilter(ColliderData::Ptr data)
//{
//    auto foundColliderIt = _colliderMap.find(data);
//    if (foundColliderIt != _colliderMap.end())
//    {
//        auto proxy = foundColliderIt->second->rigidBody()->getBroadphaseProxy();
//
//        //proxy->m_collisionFilterGroup   = short(data->node()->layouts() & ((1<<16) - 1)); // FIXME
//        proxy->m_collisionFilterGroup   = data->collisionGroup();
//        proxy->m_collisionFilterMask    = data->collisionMask();
//    }
//}

void
bullet::PhysicsWorld::updateColliderProperties(Collider::Ptr collider)
{
    if (collider == nullptr)
        return;

    auto foundColliderIt = _colliderMap.find(collider);
    if (foundColliderIt != _colliderMap.end())
    {
        auto rigidBody = foundColliderIt->second->rigidBody();
        assert(rigidBody && rigidBody->getBroadphaseProxy());

        rigidBody->setActivationState(collider->canSleep() ? ACTIVE_TAG : DISABLE_DEACTIVATION);
        rigidBody->setLinearFactor(math::convert(collider->linearFactor()));
        rigidBody->setAngularFactor(math::convert(collider->angularFactor()));
        rigidBody->setSleepingThresholds(collider->linearSleepingThreshold(), collider->angularSleepingThreshold());
        rigidBody->setDamping(collider->linearDamping(), collider->angularDamping());
    }
}

void
bullet::PhysicsWorld::updateColliderLayoutMask(Collider::Ptr collider)
{
    if (collider == nullptr)
        return;

    auto foundColliderIt = _colliderMap.find(collider);
    if (foundColliderIt != _colliderMap.end())
    {
        auto rigidBody = foundColliderIt->second->rigidBody();
        assert(rigidBody && rigidBody->getBroadphaseProxy());

        rigidBody->getBroadphaseProxy()->m_collisionFilterMask = short(collider->layoutMask() & ((1<<16) - 1));
    }
}

void
bullet::PhysicsWorld::updateColliderNodeProperties(Collider::Ptr collider)
{
    if (collider == nullptr || collider->target() == nullptr)
        return;

    auto foundColliderIt = _colliderMap.find(collider);
    if (foundColliderIt != _colliderMap.end())
    {
        auto rigidBody = foundColliderIt->second->rigidBody();
        assert(rigidBody && rigidBody->getBroadphaseProxy());

        rigidBody->getBroadphaseProxy()->m_collisionFilterGroup = short(collider->target()->layout() & ((1<<16) - 1));
    }
}

void
bullet::PhysicsWorld::removeCollider(Collider::Ptr collider)
{
    if (collider == nullptr)
        return;

    if (_colliderNodeLayoutChangedSlot.count(collider))
        _colliderNodeLayoutChangedSlot.erase(collider);

    if (_colliderPropertiesChangedSlot.count(collider))
        _colliderPropertiesChangedSlot.erase(collider);

    if (_colliderLayoutMaskChangedSlot.count(collider))
        _colliderLayoutMaskChangedSlot.erase(collider);

    auto bulletColliderIt = _colliderMap.find(collider);
    if (bulletColliderIt != _colliderMap.end())
    {
        btCollisionObject*    bulletObject = bulletColliderIt->second->rigidBody().get();

        auto dataIt = _colliderReverseMap.find(bulletObject);

        if (dataIt != _colliderReverseMap.end())
            _colliderReverseMap.erase(dataIt);

        _bulletDynamicsWorld->removeCollisionObject(bulletObject);
        _colliderMap.erase(bulletColliderIt);
    }

    auto uid = collider->uid();
    auto uidIt = _uidToCollider.find(uid);

    if (uidIt != _uidToCollider.end())
    {
        _uidAllocator->free(uid);
        _uidToCollider.erase(uidIt);
    }

    // remove all current collision pairs the collider appears in (warning: it is an ordered set, remove_if won't work).
    for (auto collisionIt = _collisions.begin(); collisionIt != _collisions.end();)
    {
        if (collisionIt->first == uid || collisionIt->second == uid)
            collisionIt = _collisions.erase(collisionIt);
        else
            ++collisionIt;
    }
}

bool
bullet::PhysicsWorld::hasCollider(Collider::Ptr collider) const
{
    return collider && _colliderMap.find(collider) != _colliderMap.end();
}

void
bullet::PhysicsWorld::setGravity(const math::vec3& gravity)
{
    _bulletDynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

void
bullet::PhysicsWorld::frameBeginHandler(std::shared_ptr<SceneManager> sceneManager, float time, float deltaTime)
{
    if (_paused)
        return;
    
    deltaTime = deltaTime / 1000.0f;

    auto baseStepLength = 1.0f / _baseFramerate;

    if (_maxNumSteps > 0 && deltaTime > _maxNumSteps * baseStepLength)
        deltaTime = baseStepLength;
            
    _bulletDynamicsWorld->stepSimulation(deltaTime, _maxNumSteps, baseStepLength);

    updateColliders();
}

void
bullet::PhysicsWorld::frameEndHandler(std::shared_ptr<SceneManager> sceneManager, float time, float deltaTime)
{
    notifyCollisions();
}

void
bullet::PhysicsWorld::updateColliders()
{
    static auto physicsModelToWorld = math::mat4();

    for (auto& dataAndBulletCollider : _colliderMap)
    {
        auto collider = dataAndBulletCollider.first;
        auto bulletCollider = dataAndBulletCollider.second;
        //auto data = collider->colliderData();

        if (collider->colliderData()->isStatic())
            continue;

        physicsModelToWorld = math::fromBulletTransform(bulletCollider->rigidBody()->getWorldTransform());

        collider->setPhysicsTransform(physicsModelToWorld);
    }
}

void
bullet::PhysicsWorld::updateRigidBodyState(Collider::Ptr        collider,
                                           math::mat4&          graphicsNoScaleTransform,
                                           const math::mat4&    centerOfMassOffset)
{
#ifdef DEBUG
    const float det3x3 = fabsf(math::determinant(math::mat3(graphicsNoScaleTransform)));

    if (fabsf(det3x3 - 1.0f) > 1e-3f)
        throw std::logic_error("Graphics world matrices used for updating rigid bodies' must be pure rotation + translation matrices.");
#endif // DEBUG

    auto foundDataIt = _colliderMap.find(collider);

    if (foundDataIt == _colliderMap.end())
        return;

    auto bulletCollider = foundDataIt->second;
    btDefaultMotionState* bulletMotionState = dynamic_cast<btDefaultMotionState*>(bulletCollider->rigidBody()->getMotionState());

    if (bulletMotionState == nullptr)
        return;

    // Update the motion state's center of mass offset transform
    math::toBulletTransform(centerOfMassOffset, bulletMotionState->m_centerOfMassOffset);

    // Update the motion state's world transform
    math::toBulletTransform(graphicsNoScaleTransform, bulletMotionState->m_graphicsWorldTrans);

    // Synchronize bullet
    static btTransform bulletTransform;

    bulletMotionState->getWorldTransform(bulletTransform);
    bulletCollider->rigidBody()->setWorldTransform(bulletTransform);
}

void
bullet::PhysicsWorld::notifyCollisions()
{
    CollisionSet currentCollisions;
    Collider::Ptr colliders[2] = { nullptr, nullptr };
    const int numManifolds = _bulletDynamicsWorld->getDispatcher()->getNumManifolds();

    for (int i = 0; i < numManifolds; ++i)
    {
        btPersistentManifold* manifold = _bulletDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

        auto foundColliderIt = _colliderReverseMap.find(manifold->getBody0());
        colliders[0] = foundColliderIt != _colliderReverseMap.end()
            ? foundColliderIt->second
            : nullptr;

        foundColliderIt = _colliderReverseMap.find(manifold->getBody1());
        colliders[1] = foundColliderIt != _colliderReverseMap.end()
            ? foundColliderIt->second
            : nullptr;

        if (colliders[0] == nullptr || colliders[1] == nullptr)
            continue;
        //if (!colliderData[0]->triggerCollisions() && !colliderData[1]->triggerCollisions())
        //    continue;

        // a collision exists between to valid colliders
        auto collision = std::make_pair(colliders[0]->uid(),  colliders[1]->uid());

        if (collision.first > collision.second)
            std::swap(collision.first, collision.second);

        if (_collisions.find(collision) == _collisions.end()) // inserted only once
        {
            if (colliders[0]->triggerCollisions())
                colliders[0]->collisionStarted()->execute(colliders[0], colliders[1]);
            if (colliders[1]->triggerCollisions())
                colliders[1]->collisionStarted()->execute(colliders[1], colliders[0]);
        }

        currentCollisions.insert(collision);
    }

    // FIXME: not optimal at all.
    // find and notify collisions that are not present anymore as the difference with the intersection
    // between the previous collision set and the current one.

    CollisionSet lostCollisions;
    
    std::set_difference(
        _collisions.begin(), _collisions.end(),
        currentCollisions.begin(), currentCollisions.end(),
        std::inserter(lostCollisions, lostCollisions.end())
    );

    for (auto& collision : lostCollisions)
    {
        auto foundColliderIt = _uidToCollider.find(collision.first);
        colliders[0] = foundColliderIt != _uidToCollider.end()
            ? foundColliderIt->second
            : nullptr;

        foundColliderIt = _uidToCollider.find(collision.second);
        colliders[1] = foundColliderIt != _uidToCollider.end()
            ? foundColliderIt->second
            : nullptr;

        if (colliders[0] == nullptr || colliders[1] == nullptr)
            continue;

        // colliders assured to trigger collisions at this point.
        colliders[0]->collisionEnded()->execute(colliders[0], colliders[1]);
        colliders[1]->collisionEnded()->execute(colliders[1], colliders[0]);
    }

    _collisions.swap(currentCollisions);
}

math::vec3
bullet::PhysicsWorld::getColliderLinearVelocity(Collider::ConstPtr collider) const
{
    auto foundColliderIt = _colliderMap.find(std::const_pointer_cast<Collider>(collider));
    
    if (foundColliderIt == _colliderMap.end())
        return math::vec3();

    auto rigidBody = foundColliderIt->second->rigidBody();
    auto vec = rigidBody->getLinearVelocity();

    return math::vec3(vec.x(), vec.y(), vec.z());
}

void
bullet::PhysicsWorld::setColliderLinearVelocity(Collider::Ptr collider, const math::vec3& value)
{
    auto foundColliderIt = _colliderMap.find(collider);

    if (foundColliderIt == _colliderMap.end())
        return;

    auto rigidBody = foundColliderIt->second->rigidBody();
    rigidBody->setLinearVelocity(math::convert(value));
}

math::vec3
bullet::PhysicsWorld::getColliderAngularVelocity(Collider::ConstPtr collider) const
{
    auto foundColliderIt = _colliderMap.find(std::const_pointer_cast<Collider>(collider));
    
    if (foundColliderIt == _colliderMap.end())
        return math::vec3();

    auto rigidBody = foundColliderIt->second->rigidBody();
    auto vec = rigidBody->getAngularVelocity();

    return math::vec3(vec.x(), vec.y(), vec.z());
}

void
bullet::PhysicsWorld::setColliderAngularVelocity(Collider::Ptr collider, const math::vec3& value)
{
    auto foundColliderIt = _colliderMap.find(collider);

    if (foundColliderIt == _colliderMap.end())
        return;

    auto rigidBody = foundColliderIt->second->rigidBody();
    rigidBody->setAngularVelocity(math::convert(value));
}

math::vec3
bullet::PhysicsWorld::getColliderGravity(Collider::ConstPtr collider) const
{
    auto foundColliderIt = _colliderMap.find(std::const_pointer_cast<Collider>(collider));
    
    if (foundColliderIt == _colliderMap.end())
        return math::vec3();

    auto rigidBody = foundColliderIt->second->rigidBody();
    auto vec = rigidBody->getGravity();
    
    return math::vec3(vec.x(), vec.y(), vec.z());
}

void
bullet::PhysicsWorld::setColliderGravity(Collider::Ptr collider, const math::vec3& value)
{
    auto foundColliderIt = _colliderMap.find(collider);

    if (foundColliderIt == _colliderMap.end())
        return;

    auto rigidBody = foundColliderIt->second->rigidBody();
    rigidBody->setGravity(math::convert(value));
}

void
bullet::PhysicsWorld::applyImpulse(Collider::Ptr        collider,
                                   const math::vec3&    impulse,
                                   bool                 isImpulseRelative,
                                   const math::vec3&    relPosition)
{
    auto foundColliderIt = _colliderMap.find(collider);

    if (foundColliderIt == _colliderMap.end())
        return;

    auto rigidBody = foundColliderIt->second->rigidBody();

    auto impulseVector = isImpulseRelative
        ? rigidBody->getWorldTransform().getBasis() * math::convert(impulse)
        : math::convert(impulse);

    rigidBody->applyImpulse(impulseVector, math::convert(relPosition));
}

bool
bullet::PhysicsWorld::raycast(const math::vec3& origin, const math::vec3& direction, float maxDist, math::vec3& hit) const
{
    btVector3 btFrom(origin.x, origin.y, origin.z);
    btVector3 btTo(origin.x + direction.x * maxDist, origin.y + direction.y * maxDist, origin.z + direction.z * maxDist);

    btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);

    _bulletDynamicsWorld->rayTest(btFrom, btTo, res);

    if(res.hasHit())
    {
        auto vec = res.m_hitPointWorld;
        
        hit.x = vec.x();
        hit.y = vec.y();
        hit.z = vec.z();

        return true;
    }

    return false;
}