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

#include "minko/component/bullet/Collider.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/bullet/AbstractPhysicsShape.hpp"
#include "minko/component/bullet/ColliderData.hpp"
#include "minko/component/bullet/PhysicsWorld.hpp"
#include "minko/component/bullet/PhysicsWorld.hpp"
#include "minko/file/AssetLibrary.hpp"

#include "minko/math/tools.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;

bullet::Collider::Collider(ColliderData::Ptr data):
    AbstractComponent(LayoutMask::COLLISIONS_DYNAMIC_DEFAULT),
    _colliderData(data),
    _canSleep(false),
    _triggerCollisions(false),
    _linearFactor(math::vec3(1.0f, 1.0f, 1.0f)),
    _linearDamping(0.0f),
    _linearSleepingThreshold(0.8f),
    _angularFactor(math::vec3(1.0f, 1.0f, 1.0f)),
    _angularDamping(0.0f),
    _angularSleepingThreshold(1.0f),
    _physicsWorld(nullptr),
    _correction(math::mat4()),
    _physicsTransform(math::mat4()),
    _graphicsTransform(nullptr),
    _propertiesChanged(Signal<Ptr>::create()),
    _collisionStarted(Signal<Ptr, Ptr>::create()),
    _collisionEnded(Signal<Ptr, Ptr>::create()),
    _physicsTransformChanged(Signal<Ptr, math::mat4>::create()),
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
bullet::Collider::targetAdded(Node::Ptr target)
{
    _addedSlot = target->added().connect(std::bind(
        &bullet::Collider::addedHandler,
        std::static_pointer_cast<Collider>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
        ));

    _removedSlot = target->removed().connect(std::bind(
        &bullet::Collider::removedHandler,
        std::static_pointer_cast<Collider>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
        ));
}

void
bullet::Collider::targetRemoved(Node::Ptr target)
{
    if (_physicsWorld != nullptr)
        _physicsWorld->removeChild(std::static_pointer_cast<Collider>(shared_from_this()));

    _physicsWorld = nullptr;
    _graphicsTransform = nullptr;

    _addedSlot = nullptr;
    _removedSlot = nullptr;
}

void
bullet::Collider::addedHandler(Node::Ptr node, Node::Ptr target, Node::Ptr ancestor)
{
    initializeFromNode(node);

    assert(_graphicsTransform);
}

void
bullet::Collider::removedHandler(Node::Ptr, Node::Ptr, Node::Ptr)
{
    //if (_physicsWorld != nullptr)
    //    _physicsWorld->removeChild(std::static_pointer_cast<Collider>(shared_from_this()));

    //_physicsWorld        = nullptr;
    //_graphicsTransform    = nullptr;
}

void
bullet::Collider::initializeFromNode(Node::Ptr node)
{
    if (_graphicsTransform != nullptr && _physicsWorld != nullptr)
        return;

    _physicsTransform = math::mat4(); // Matrix automatically updated by physicsWorldTransformChangedHandler

    // Get existing transform component or create one if necessary
    if (!node->hasComponent<Transform>())
        node->addComponent(Transform::create());

    _graphicsTransform = node->component<Transform>();

    if (fabsf(math::determinant(_graphicsTransform->modelToWorldMatrix(true))) < 1e-4f)
        throw std::logic_error("The node's model-to-world matrix cannot be inverted.");

    // Identify physics world
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

    auto graphicsTransform = _graphicsTransform->modelToWorldMatrix(forceTransformUpdate);
    static auto graphicsNoScale = math::mat4();
    static auto graphicsNoScaleInverse = math::mat4();
    static auto centerOfMassOffset = math::mat4();
    static auto physicsTransform = math::mat4();

    // remove the scaling/shear from the graphics transform, but record it to restitute it during rendering
    math::removeScalingShear(
        graphicsTransform,
        graphicsNoScale,
        _correction
    );

    graphicsNoScaleInverse = math::inverse(graphicsNoScale);
    centerOfMassOffset = graphicsNoScale * (_colliderData->shape()->deltaTransformInverse() * graphicsNoScaleInverse);
    physicsTransform = _colliderData->shape()->deltaTransform() * graphicsNoScale;

    setPhysicsTransform(physicsTransform, &_graphicsTransform->matrix());

    if (_physicsWorld)
        _physicsWorld->updateRigidBodyState(
            std::static_pointer_cast<Collider>(shared_from_this()),
            graphicsNoScale,
            centerOfMassOffset
        );
}

bullet::Collider::Ptr
bullet::Collider::setPhysicsTransform(const math::mat4& physicsTransform,
                                      const math::mat4* graphicsModelToParent,
                                      bool              forceTransformUpdate)
{
    assert(_graphicsTransform);

    // Update the physics world transform
    _physicsTransform = physicsTransform;

    if (graphicsModelToParent)
        _graphicsTransform->matrix(*graphicsModelToParent);
    else
    {
        // Recompute graphics transform from the physics transform

        // Update the graphics local transform
        static auto worldToParent    = math::mat4();

        worldToParent = _graphicsTransform->matrix() * math::inverse(_graphicsTransform->modelToWorldMatrix(forceTransformUpdate));

        _graphicsTransform->matrix(worldToParent * (physicsTransform * (_colliderData->shape()->deltaTransformInverse() * _correction)));
    }

    // Fire update signals
    _physicsTransformChanged->execute(std::static_pointer_cast<Collider>(shared_from_this()), _physicsTransform);
    _graphicsTransformChanged->execute(std::static_pointer_cast<Collider>(shared_from_this()), _graphicsTransform);

    return std::static_pointer_cast<Collider>(shared_from_this());
}

math::mat4
bullet::Collider::getPhysicsTransform() const
{
    return _physicsTransform;
}

math::vec3
bullet::Collider::linearVelocity() const
{
    return _physicsWorld
        ? _physicsWorld->getColliderLinearVelocity(
            std::static_pointer_cast<const Collider>(shared_from_this())
        )
        : math::vec3();
}

bullet::Collider::Ptr
bullet::Collider::linearVelocity(const math::vec3& value)
{
    if (_physicsWorld)
        _physicsWorld->setColliderLinearVelocity(
            std::static_pointer_cast<Collider>(shared_from_this()),
            value
        );

    return std::static_pointer_cast<Collider>(shared_from_this());
}

math::vec3
bullet::Collider::angularVelocity(const math::vec3& output) const
{
    return _physicsWorld
        ? _physicsWorld->getColliderAngularVelocity(
            std::static_pointer_cast<const Collider>(shared_from_this())
        )
        : math::vec3();
}

bullet::Collider::Ptr
bullet::Collider::angularVelocity(const math::vec3& value)
{
    if (_physicsWorld)
        _physicsWorld->setColliderAngularVelocity(
            std::static_pointer_cast<Collider>(shared_from_this()),
            value
        );

    return std::static_pointer_cast<Collider>(shared_from_this());
}

bullet::Collider::Ptr
bullet::Collider::applyImpulse(const math::vec3& impulse, const math::vec3& relPosition)
{
    _physicsWorld->applyImpulse(
        std::static_pointer_cast<Collider>(shared_from_this()),
        impulse,
        false,
        relPosition
    );

    return std::static_pointer_cast<Collider>(shared_from_this());
}

bullet::Collider::Ptr
bullet::Collider::applyRelativeImpulse(const math::vec3& impulse, const math::vec3& relPosition)
{
    _physicsWorld->applyImpulse(
        std::static_pointer_cast<Collider>(shared_from_this()),
        impulse,
        true,
        math::vec3()
    );

    return std::static_pointer_cast<Collider>(shared_from_this());
}

bullet::Collider::Ptr
bullet::Collider::linearFactor(const math::vec3& values)
{
    const bool changed = fabsf(values.x - _linearFactor.x) > 1e-3f
        || fabsf(values.y - _linearFactor.y) > 1e-3f
        || fabsf(values.z - _linearFactor.z) > 1e-3f;

    _linearFactor = values;

    if (changed)
        _propertiesChanged->execute(std::static_pointer_cast<Collider>(shared_from_this()));

    return std::static_pointer_cast<Collider>(shared_from_this());
}


bullet::Collider::Ptr
bullet::Collider::angularFactor(const math::vec3& values)
{
    const bool changed = fabsf(values.x - _angularFactor.x) > 1e-3f
        || fabsf(values.y - _angularFactor.y) > 1e-3f
        || fabsf(values.z - _angularFactor.z) > 1e-3f;

    _angularFactor = values;

    if (changed)
        _propertiesChanged->execute(std::static_pointer_cast<Collider>(shared_from_this()));

    return std::static_pointer_cast<Collider>(shared_from_this());
}


bullet::Collider::Ptr
bullet::Collider::damping(float linearDamping, float angularDamping)
{
    const bool changed = fabsf(_linearDamping - linearDamping) > 1e-3f
        || fabsf(_angularDamping - angularDamping) > 1e-3f;

    _linearDamping = linearDamping;
    _angularDamping = angularDamping;

    if (changed)
        _propertiesChanged->execute(std::static_pointer_cast<Collider>(shared_from_this()));

    return std::static_pointer_cast<Collider>(shared_from_this());
}

bullet::Collider::Ptr
bullet::Collider::sleepingThresholds(float linearSleepingThreshold, float angularSleepingThreshold)
{
    const bool changed = fabsf(_linearSleepingThreshold - linearSleepingThreshold) > 1e-3f
        || fabsf(_angularSleepingThreshold - angularSleepingThreshold) > 1e-3f;

    _linearSleepingThreshold = linearSleepingThreshold;
    _angularSleepingThreshold = angularSleepingThreshold;

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