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
#include "minko/component/AbstractComponent.hpp"

class btDynamicsWorld;
class btBroadphaseInterface;
class btCollisionConfiguration;
class btConstraintSolver;
class btDispatcher;
class btCollisionShape;
class btMotionState;
class btCollisionObject;
class btTransform;

namespace minko
{
	namespace component
	{
		namespace bullet
		{
			class AbstractPhysicsShape;
			class SphereShape;
			class BoxShape;
			class ConeShape;
			class CylinderShape;
			class Collider;

			class PhysicsWorld:
				public AbstractComponent,
				public std::enable_shared_from_this<PhysicsWorld>
			{
			public:
				typedef std::shared_ptr<PhysicsWorld> Ptr;

			private:
				typedef std::shared_ptr<AbstractComponent>			AbsCtrlPtr;
				typedef std::shared_ptr<scene::Node>				NodePtr;
				typedef std::shared_ptr<Collider>					ColliderPtr; 
				typedef std::shared_ptr<Renderer>					RendererPtr;
				typedef std::shared_ptr<math::Vector3>				Vector3Ptr;
				typedef std::shared_ptr<math::Matrix4x4>			Matrix4x4Ptr;
				typedef std::shared_ptr<math::Quaternion>			QuaternionPtr;

				typedef std::shared_ptr<btBroadphaseInterface>		btBroadphasePtr;
				typedef std::shared_ptr<btCollisionConfiguration>	btCollisionConfigurationPtr;
				typedef std::shared_ptr<btConstraintSolver>			btConstraintSolverPtr;
				typedef std::shared_ptr<btDispatcher>				btDispatcherPtr;
				typedef std::shared_ptr<btDynamicsWorld>			btDynamicsWorldPtr;

				class BulletCollider;
				typedef std::shared_ptr<BulletCollider>				BulletColliderPtr;

				typedef std::unordered_map<ColliderPtr, BulletColliderPtr>	ColliderMap;

			private:
				ColliderMap										_colliderMap;

				btBroadphasePtr									_bulletBroadphase;
				btCollisionConfigurationPtr						_bulletCollisionConfiguration;
				btConstraintSolverPtr							_bulletConstraintSolver;
				btDispatcherPtr									_bulletDispatcher;
				btDynamicsWorldPtr								_bulletDynamicsWorld;

				std::shared_ptr<SceneManager>					_sceneManager;

				Signal<AbsCtrlPtr, NodePtr>::Slot				_targetAddedSlot;
				Signal<AbsCtrlPtr, NodePtr>::Slot				_targetRemovedSlot;
				Signal<std::shared_ptr<SceneManager>>::Slot		_frameEndSlot;
				Signal<NodePtr, NodePtr, NodePtr>::Slot			_addedOrRemovedSlot;
				Signal<NodePtr, NodePtr, AbsCtrlPtr>::Slot		_componentAddedOrRemovedSlot;

			public:
				static
				Ptr
				create()
				{
					Ptr physicsWorld(new PhysicsWorld());

					physicsWorld->initialize();

					return physicsWorld;
				}

				bool
				hasCollider(ColliderPtr) const;

				void
				addChild(ColliderPtr);

				void
				removeChild(ColliderPtr);

				void
				setGravity(Vector3Ptr);

				void 
				update(float timeStep = 1.0f/60.0f);

				void
				setPhysicsTransformFromCollider(ColliderPtr);

				void
				setWorldTransformFromCollider(ColliderPtr);

				void
				forceColliderWorldTransform(ColliderPtr, Matrix4x4Ptr);

				Matrix4x4Ptr
				getPhysicsWorldTransform(ColliderPtr) const;

				void
				setPhysicsWorldTransform(ColliderPtr, Matrix4x4Ptr) const;

				void
				setLinearVelocity(ColliderPtr, Vector3Ptr);

				void
				prependLocalTranslation(ColliderPtr, Vector3Ptr);

				void
				prependRotationY(ColliderPtr, float);

				void
				applyRelativeImpulse(ColliderPtr, Vector3Ptr);

				static
				Matrix4x4Ptr
				removeScalingShear(Matrix4x4Ptr, Matrix4x4Ptr output = nullptr, Matrix4x4Ptr correction = nullptr);

			private:
				PhysicsWorld();

				void 
				initialize();

				void 
				targetAddedHandler(AbsCtrlPtr, NodePtr);

				void 
				targetRemovedHandler(AbsCtrlPtr, NodePtr);

				void
				addedHandler(NodePtr node, NodePtr target, NodePtr ancestor);

				void
				frameEndHandler(std::shared_ptr<SceneManager> sceneManager);

				void
				updateColliders();

				static
				Matrix4x4Ptr
				fromBulletTransform(const btTransform&);

				static
				void
				toBulletTransform(Matrix4x4Ptr, btTransform&);

				static
				void
				toBulletTransform(QuaternionPtr, Vector3Ptr, btTransform&);

				void
				componentAddedHandler(NodePtr node, NodePtr target, AbsCtrlPtr component);

				void
				setSceneManager(std::shared_ptr<SceneManager> sceneManager);

				static
				std::ostream&
				print(std::ostream&, const btTransform&);

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
					typedef std::shared_ptr<btCollisionObject>		btCollisionObjectPtr;


				private:
					btCollisionShapePtr		_bulletCollisionShape;
					btMotionStatePtr		_bulletMotionState;
					btCollisionObjectPtr	_bulletCollisionObject;

				public:
					static
					BulletColliderPtr
					create(ColliderPtr);

					inline
					btCollisionShapePtr
					collisionShape() const
					{
						return _bulletCollisionShape;
					}

					inline
					btMotionStatePtr
					motionState() const
					{
						return _bulletMotionState;
					}

					inline 
					btCollisionObjectPtr
					collisionObject() const
					{
						return _bulletCollisionObject;
					}

					void 
					setWorldTransform(Matrix4x4Ptr);

					void
					setPhysicsWorldTransform(Matrix4x4Ptr) const;

					void
					setLinearVelocity(Vector3Ptr);

					void
					prependLocalTranslation(Vector3Ptr);

					void
					prependRotationY(float);

					void
					applyRelativeImpulse(Vector3Ptr);

				private:
					BulletCollider();

					void
					initialize(ColliderPtr);

					void
					initializeCollisionShape(AbsShapePtr);

					void
					initializeSphereShape(SphereShapePtr);

					void
					initializeBoxShape(BoxShapePtr);

					void
					initializeConeShape(ConeShapePtr);

					void
					initializeCylinderShape(CylinderShapePtr);

					void
					initializeMotionState(ColliderPtr);

					void
					initializeCollisionObject(ColliderPtr);
				};
			};
		}
	}
}
