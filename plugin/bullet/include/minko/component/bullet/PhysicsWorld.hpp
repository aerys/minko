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

#pragma once

#include "minko/Common.hpp"
#include "minko/BulletCommon.hpp"

#include "minko/component/AbstractComponent.hpp"

class btDynamicsWorld;
class btBroadphaseInterface;
class btCollisionConfiguration;
class btConstraintSolver;
class btDispatcher;
class btCollisionShape;
class btMotionState;
struct btDefaultMotionState;
class btCollisionObject;
class btTransform;
class btRigidBody;

namespace minko
{
    namespace component
    {
        namespace bullet
        {
            class LinearIdAllocator;

            class PhysicsWorld:
                public AbstractComponent
            {
                friend class Collider;

            public:
                typedef std::shared_ptr<PhysicsWorld>                               Ptr;

            private:
                typedef std::shared_ptr<LinearIdAllocator>                                  LinearIdAllocatorPtr;
                typedef std::shared_ptr<AbstractComponent>                                  AbsCmp;
                typedef std::shared_ptr<scene::Node>                                        NodePtr;
                typedef std::shared_ptr<Collider>                                           ColliderPtr;
                typedef std::shared_ptr<const Collider>                                     ConstColliderPtr;
                typedef std::shared_ptr<Renderer>                                           RendererPtr;
                typedef std::shared_ptr<math::vec3>                                         Vector3Ptr;
                typedef std::shared_ptr<math::mat4>                                         Matrix4x4Ptr;
                typedef std::shared_ptr<math::quat>                                         QuaternionPtr;

                typedef std::shared_ptr<btTransform>                                    btTransformPtr;
                typedef std::shared_ptr<btBroadphaseInterface>                            btBroadphasePtr;
                typedef std::shared_ptr<btCollisionConfiguration>                        btCollisionConfigurationPtr;
                typedef std::shared_ptr<btConstraintSolver>                                btConstraintSolverPtr;
                typedef std::shared_ptr<btDispatcher>                                    btDispatcherPtr;
                typedef std::shared_ptr<btDynamicsWorld>                                btDynamicsWorldPtr;

                class BulletCollider;
                typedef std::shared_ptr<BulletCollider>                             BulletColliderPtr;
                typedef std::unordered_map<ColliderPtr, BulletColliderPtr>          ColliderMap;
                typedef std::unordered_map<const btCollisionObject*, ColliderPtr>   ColliderReverseMap;

                typedef std::set<std::pair<uint, uint>>                             CollisionSet;
                typedef Signal<NodePtr, NodePtr>                                    NodeLayoutsChanged;
                typedef Signal<AbsCmp>                                              LayoutMaskChanged;
                typedef Signal<ColliderPtr>                                         ColliderChanged;

            private:
                static const uint                                                   _MAX_BODIES;

                LinearIdAllocatorPtr                                                _uidAllocator;
                ColliderMap                                                         _colliderMap;
                ColliderReverseMap                                                  _colliderReverseMap;
                std::unordered_map<uint, ColliderPtr>                               _uidToCollider;
                CollisionSet                                                        _collisions;

                btBroadphasePtr                                                     _bulletBroadphase;
                btCollisionConfigurationPtr                                         _bulletCollisionConfiguration;
                btConstraintSolverPtr                                               _bulletConstraintSolver;
                btDispatcherPtr                                                     _bulletDispatcher;
                btDynamicsWorldPtr                                                  _bulletDynamicsWorld;

                std::shared_ptr<SceneManager>                                       _sceneManager;

                Signal<AbsCmp, NodePtr>::Slot                                       _targetAddedSlot;
                Signal<AbsCmp, NodePtr>::Slot                                       _targetRemovedSlot;
                Signal<AbsCmp, NodePtr>::Slot                                       _exitFrameSlot;
                Signal<std::shared_ptr<SceneManager>, float, float>::Slot           _frameBeginSlot;
                Signal<std::shared_ptr<SceneManager>, float, float>::Slot           _frameEndSlot;
                Signal<NodePtr, NodePtr, NodePtr>::Slot                             _addedOrRemovedSlot;
                Signal<NodePtr, NodePtr, AbsCmp>::Slot                              _componentAddedOrRemovedSlot;
                std::unordered_map<ColliderPtr, ColliderChanged::Slot>              _colliderPropertiesChangedSlot;
                std::unordered_map<ColliderPtr, NodeLayoutsChanged::Slot>           _colliderNodeLayoutChangedSlot;
                std::unordered_map<ColliderPtr, LayoutMaskChanged::Slot>            _colliderLayoutMaskChangedSlot;

                int                                                                 _maxNumSteps;
                float                                                               _baseFramerate;
                
                bool                                                                _paused;

            public:
                static
                Ptr
                create()
                {
                    Ptr ptr(new PhysicsWorld());

                    ptr->initialize();

                    return ptr;
                }

                ~PhysicsWorld()
                {
                }
                    
                inline
                void
                paused(bool paused)
                {
                    _paused = paused;
                }

                inline
                bool
                paused()
                {
                    return _paused;
                }

                bool
                hasCollider(ColliderPtr) const;

                void
                addCollider(ColliderPtr);

                void
                removeCollider(ColliderPtr);

                void
                setGravity(const math::vec3&);
                
                inline
                void
                maxNumSteps(int value)
                {
                    _maxNumSteps = value;
                }

                inline
                int
                maxNumSteps()
                {
                    return _maxNumSteps;
                }

                inline
                void
                baseFramerate(float value)
                {
                    _baseFramerate = value;
                }

                inline
                float
                baseFramerate()
                {
                    return _baseFramerate;
                }

            private: // Only the Collider class should know of the following functions
                void
                synchronizePhysicsWithGraphics(ColliderPtr, const math::mat4&);

                void
                updateRigidBodyState(ColliderPtr, math::mat4&, const math::mat4&);

                math::vec3
                getColliderLinearVelocity(ConstColliderPtr) const;

                void
                setColliderLinearVelocity(ColliderPtr, const math::vec3&);

                math::vec3
                getColliderAngularVelocity(ConstColliderPtr) const;

                void
                setColliderAngularVelocity(ColliderPtr, const math::vec3&);

                void
                setColliderGravity(ColliderPtr, const math::vec3&);

                math::vec3
                getColliderGravity(ConstColliderPtr) const;
                
                bool
                raycast(const math::vec3& origin, const math::vec3& direction, float maxDist, math::vec3& hit) const;

                bool
                raycast(const math::vec3& origin, const math::vec3& direction, math::vec3& hit)
                {
                    return raycast(origin, direction, 10000.f, hit);
                }

                void
                applyImpulse(ColliderPtr, const math::vec3& impulse, bool isImpulseRelative, const math::vec3& relPosition);

            private:
                PhysicsWorld();

                void
                initialize();

                void
                targetAdded(NodePtr);

                void
                targetRemoved(NodePtr);

                void
                addedHandler(NodePtr, NodePtr, NodePtr);

                void
                frameBeginHandler(std::shared_ptr<SceneManager>, float time, float deltaTime);

                void
                frameEndHandler(std::shared_ptr<SceneManager>, float time, float deltaTime);

                void
                updateColliders();

                void
                notifyCollisions();

                void
                componentAddedHandler(NodePtr, NodePtr, AbsCmp);

                void
                setSceneManager(std::shared_ptr<SceneManager>);

                void
                updateCollisionFilter(ColliderPtr);

                void
                updateColliderProperties(ColliderPtr);

                void
                updateColliderLayoutMask(ColliderPtr);

                void
                updateColliderNodeProperties(ColliderPtr);

            private:
                class BulletCollider
                {
                public:
                    typedef std::shared_ptr<BulletCollider> Ptr;

                private:
                    typedef std::shared_ptr<AbstractPhysicsShape>       AbsShapePtr;
                    typedef std::shared_ptr<SphereShape>                SphereShapePtr;
                    typedef std::shared_ptr<BoxShape>                   BoxShapePtr;
                    typedef std::shared_ptr<ConeShape>                  ConeShapePtr;
                    typedef std::shared_ptr<CylinderShape>              CylinderShapePtr;
                    typedef std::shared_ptr<ConvexHullShape>            ConvexHullShapePtr;
					typedef std::shared_ptr<CapsuleShape>				CapsuleShapePtr;
					typedef std::shared_ptr<TriangleMeshShape>			TriangleMeshShapePtr;

                    typedef std::shared_ptr<btCollisionShape>           btCollisionShapePtr;
                    typedef std::shared_ptr<btMotionState>              btMotionStatePtr;
                    typedef std::shared_ptr<btDefaultMotionState>       btDefaultMotionStatePtr;
                    typedef std::shared_ptr<btCollisionObject>          btCollisionObjectPtr;
                    typedef std::shared_ptr<btRigidBody>                btRigidBodyPtr;

                private:
                    btCollisionShapePtr        _bulletCollisionShape;
                    btMotionStatePtr        _bulletMotionState;
                    btCollisionObjectPtr    _bulletCollisionObject;

                public:
                    inline static
                    Ptr
                    create(ColliderPtr collider)
                    {
                        Ptr ptr = std::shared_ptr<BulletCollider>(new BulletCollider());

                        ptr->initialize(collider);

                        return ptr;
                    }

                    btRigidBodyPtr
                    rigidBody() const;

                private:
                    BulletCollider();

                    void
                    initialize(ColliderPtr);

                    btCollisionShapePtr
                    initializeCollisionShape(AbsShapePtr) const;

                    btCollisionShapePtr
                    initializeSphereShape(SphereShapePtr) const;

                    btCollisionShapePtr
                    initializeBoxShape(BoxShapePtr) const;

                    btCollisionShapePtr
                    initializeConeShape(ConeShapePtr) const;

                    btCollisionShapePtr
                    initializeCylinderShape(CylinderShapePtr) const;

                    btCollisionShapePtr
                    initializeConvexHullShape(ConvexHullShapePtr) const;

					btCollisionShapePtr
					initializeCapsuleShape(CapsuleShapePtr) const;

					btCollisionShapePtr
					initializeTriangleMeshShape(TriangleMeshShapePtr) const;

                    btMotionStatePtr
                    initializeMotionState(ColliderPtr) const;

                    void
                    initializeCollisionObject(ColliderPtr, btCollisionShapePtr, btMotionStatePtr);
                };
            };
        }
    }
}
