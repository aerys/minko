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
				public AbstractComponent,
				public std::enable_shared_from_this<PhysicsWorld>
			{
				friend class Collider;

			public:
				typedef std::shared_ptr<PhysicsWorld>                                   Ptr;

			private:
				typedef std::shared_ptr<LinearIdAllocator>			                    LinearIdAllocatorPtr;
				typedef std::shared_ptr<AbstractComponent>			                    AbsCtrlPtr;
				typedef std::shared_ptr<scene::Node>				                    NodePtr;
				typedef std::shared_ptr<Collider>										ColliderPtr; 
				typedef std::shared_ptr<const Collider>									ConstColliderPtr;
				typedef std::shared_ptr<Renderer>					                    RendererPtr;
				typedef std::shared_ptr<math::Vector3>				                    Vector3Ptr;
				typedef std::shared_ptr<math::Matrix4x4>			                    Matrix4x4Ptr;
				typedef std::shared_ptr<math::Quaternion>			                    QuaternionPtr;

				typedef std::shared_ptr<btTransform>				                    btTransformPtr;
				typedef std::shared_ptr<btBroadphaseInterface>		                    btBroadphasePtr;
				typedef std::shared_ptr<btCollisionConfiguration>	                    btCollisionConfigurationPtr;
				typedef std::shared_ptr<btConstraintSolver>			                    btConstraintSolverPtr;
				typedef std::shared_ptr<btDispatcher>				                    btDispatcherPtr;
				typedef std::shared_ptr<btDynamicsWorld>			                    btDynamicsWorldPtr;

				class BulletCollider;
				typedef std::shared_ptr<BulletCollider>									BulletColliderPtr;
				typedef std::unordered_map<ColliderPtr, BulletColliderPtr>				ColliderMap;
				typedef std::unordered_map<const btCollisionObject*, ColliderPtr>		ColliderReverseMap;

				typedef std::set<std::pair<uint, uint>>									CollisionSet;
                typedef Signal<NodePtr, NodePtr>                                        NodeLayoutsChanged;
                typedef Signal<ColliderPtr>												ColliderChanged;

			private:
				static const uint								                        _MAX_BODIES;

				LinearIdAllocatorPtr							                        _uidAllocator;
				ColliderMap										                        _colliderMap;
				ColliderReverseMap								                        _colliderReverseMap;
				std::unordered_map<uint, ColliderPtr>									_uidToCollider;
				CollisionSet									                        _collisions;

				btBroadphasePtr									                        _bulletBroadphase;
				btCollisionConfigurationPtr						                        _bulletCollisionConfiguration;
				btConstraintSolverPtr							                        _bulletConstraintSolver;
				btDispatcherPtr									                        _bulletDispatcher;
				btDynamicsWorldPtr								                        _bulletDynamicsWorld;

				std::shared_ptr<SceneManager>					                        _sceneManager;

				Signal<AbsCtrlPtr, NodePtr>::Slot				                        _targetAddedSlot;
				Signal<AbsCtrlPtr, NodePtr>::Slot				                        _targetRemovedSlot;
				Signal<AbsCtrlPtr, NodePtr>::Slot				                        _exitFrameSlot;
				Signal<std::shared_ptr<SceneManager>, float, float>::Slot               _frameBeginSlot;
				Signal<std::shared_ptr<SceneManager>, float, float>::Slot               _frameEndSlot;
				Signal<NodePtr, NodePtr, NodePtr>::Slot			                        _addedOrRemovedSlot;
				Signal<NodePtr, NodePtr, AbsCtrlPtr>::Slot		                        _componentAddedOrRemovedSlot;
				std::unordered_map<ColliderPtr, ColliderChanged::Slot>					_colliderPropertiesChangedSlot;
                std::unordered_map<ColliderPtr, NodeLayoutsChanged::Slot>				_colliderNodeLayoutChangedSlot;

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

				bool
				hasCollider(ColliderPtr) const;

				void
				addChild(ColliderPtr);

				void
				removeChild(ColliderPtr);

				void
				setGravity(Vector3Ptr);

			private: // only the Collider class should know of the following functions
				void
				synchronizePhysicsWithGraphics(ColliderPtr, Matrix4x4Ptr);

				void
				updateRigidBodyState(ColliderPtr, Matrix4x4Ptr, Matrix4x4Ptr);

				Vector3Ptr
				getColliderLinearVelocity(ConstColliderPtr, Vector3Ptr = nullptr) const;

				void
				setColliderLinearVelocity(ColliderPtr, Vector3Ptr);

				Vector3Ptr
				getColliderAngularVelocity(ConstColliderPtr, Vector3Ptr = nullptr) const;

				void
				setColliderAngularVelocity(ColliderPtr, Vector3Ptr);

				void
				applyImpulse(ColliderPtr, Vector3Ptr impulse, bool isImpulseRelative, Vector3Ptr relPosition = nullptr);

			private:
				PhysicsWorld();

				void 
				initialize();

				void 
				targetAddedHandler(AbsCtrlPtr, NodePtr);

				void 
				targetRemovedHandler(AbsCtrlPtr, NodePtr);

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
				componentAddedHandler(NodePtr, NodePtr, AbsCtrlPtr);

				void
				setSceneManager(std::shared_ptr<SceneManager>);

                void
                updateCollisionFilter(ColliderPtr);

				void
				updateColliderProperties(ColliderPtr);

				void
				updateColliderNodeProperties(ColliderPtr);

			private:
				class BulletCollider
				{
				public:
					typedef std::shared_ptr<BulletCollider> Ptr;

				private:
					typedef std::shared_ptr<AbstractPhysicsShape>	AbsShapePtr;
					typedef std::shared_ptr<SphereShape>			SphereShapePtr;
					typedef std::shared_ptr<BoxShape>				BoxShapePtr;
					typedef std::shared_ptr<ConeShape>				ConeShapePtr;
					typedef std::shared_ptr<CylinderShape>			CylinderShapePtr;

					typedef std::shared_ptr<btCollisionShape>		btCollisionShapePtr;
					typedef std::shared_ptr<btMotionState>			btMotionStatePtr;
					typedef std::shared_ptr<btDefaultMotionState>	btDefaultMotionStatePtr;
					typedef std::shared_ptr<btCollisionObject>		btCollisionObjectPtr;
					typedef std::shared_ptr<btRigidBody>			btRigidBodyPtr;

				private:
					btCollisionShapePtr		_bulletCollisionShape;
					btMotionStatePtr		_bulletMotionState;
					btCollisionObjectPtr	_bulletCollisionObject;

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

					btMotionStatePtr
					initializeMotionState(ColliderPtr) const;

					void
					initializeCollisionObject(ColliderPtr, btCollisionShapePtr, btMotionStatePtr);
				};
			};
		}
	}
}
