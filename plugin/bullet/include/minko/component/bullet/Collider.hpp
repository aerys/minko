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

namespace minko
{
	namespace Component
	{
		class ColliderData;
	}
}

namespace minko
{
	namespace component
	{
		namespace bullet
		{
			class ColliderData;
			class PhysicsWorld;

			class Collider:
				public AbstractComponent,
				public std::enable_shared_from_this<Collider>
			{
			public:
				typedef std::shared_ptr<Collider>					Ptr;
				typedef std::shared_ptr<const Collider>				ConstPtr;

			private:
				typedef std::shared_ptr<file::AssetLibrary>			AssetLibraryPtr;
				typedef std::shared_ptr<AbstractComponent>			AbsCtrlPtr;
				typedef std::shared_ptr<scene::Node>				NodePtr;
				typedef std::shared_ptr<scene::NodeSet>				NodeSetPtr;
				typedef std::shared_ptr<math::Vector3>				Vector3Ptr;
				typedef std::shared_ptr<math::Matrix4x4>			Matrix4x4Ptr;
				typedef std::shared_ptr<ColliderData>				ColliderDataPtr;
				typedef std::shared_ptr<Transform>					TransformPtr;
				typedef std::shared_ptr<PhysicsWorld>				PhysicsWorldPtr;
				typedef std::shared_ptr<Surface>					SurfacePtr;
																	
			private:
				int													_uid;
				ColliderDataPtr										_colliderData;
				short												_collisionGroup;
                short												_collisionMask;
				bool												_canSleep;
				bool												_triggerCollisions;
				Vector3Ptr											_linearFactor;
				float												_linearDamping;
				float												_linearSleepingThreshold;
				Vector3Ptr											_angularFactor;
				float												_angularDamping;
				float												_angularSleepingThreshold;

				PhysicsWorldPtr										_physicsWorld;
				Matrix4x4Ptr										_correction;
				Matrix4x4Ptr										_physicsTransform;
				TransformPtr										_graphicsTransform;

				NodePtr												_colliderDisplayNode;
				
				std::shared_ptr<Signal<Ptr>>						_propertiesChanged;
				std::shared_ptr<Signal<Ptr, Ptr>>					_collisionStarted;
				std::shared_ptr<Signal<Ptr, Ptr>>					_collisionEnded;

				Signal<AbsCtrlPtr, NodePtr>::Slot					_targetAddedSlot;
				Signal<AbsCtrlPtr, NodePtr>::Slot					_targetRemovedSlot;
				Signal<NodePtr, NodePtr, NodePtr>::Slot				_addedSlot;
				Signal<NodePtr, NodePtr, NodePtr>::Slot				_removedSlot;
																	
			public:
				inline static
				Ptr
				create(ColliderDataPtr data)
				{
					Ptr collider(new Collider(data));

					collider->initialize();

					return collider;
				}

				~Collider()
				{
				}

				inline
				ColliderDataPtr
				colliderData()
				{
					return _colliderData;
				}

				void
				synchronizePhysicsWithGraphics();

				void
				updatePhysicsTransform(Matrix4x4Ptr);

				Ptr
				show(AssetLibraryPtr);

				Ptr
				hide();

				inline
				NodePtr
				target() const
				{
					return targets().empty() ? nullptr : targets().front();
				}

				inline
				uint
				uid() const
				{
					if (_uid < 0)
						throw;

					return uint(_uid);
				}

				inline
				Ptr
				uid(uint value)
				{
					_uid = value;

					return shared_from_this();
				}

				inline
                short
                collisionGroup() const
                {
                    return _collisionGroup;
                }

                Ptr
                collisionGroup(short);

                inline
                short
                collisionMask() const
                {
                    return _collisionMask;
                }

                Ptr
                collisionMask(short);

				Vector3Ptr
				linearVelocity(Vector3Ptr = nullptr) const;

				Ptr
				linearVelocity(Vector3Ptr);

				inline
				Vector3Ptr
				linearFactor() const
				{
					return _linearFactor;
				}

				Ptr
				linearFactor(Vector3Ptr);

				Vector3Ptr
				angularVelocity(Vector3Ptr = nullptr) const;

				Ptr
				angularVelocity(Vector3Ptr);

				inline
				Vector3Ptr
				angularFactor() const
				{
					return _angularFactor;
				}

				Ptr
				angularFactor(Vector3Ptr);

				Ptr
				applyImpulse(Vector3Ptr impulse, Vector3Ptr relPosition = nullptr);

				Ptr
				applyRelativeImpulse(Vector3Ptr impulse, Vector3Ptr relPosition = nullptr);

				inline
				bool
				canSleep() const
				{
					return _canSleep;
				}

				Ptr
				canSleep(bool);

				inline
				bool
				triggerCollisions() const
				{
					return _triggerCollisions;
				}

				inline
				Ptr
				triggerCollisions(bool value)
				{
					_triggerCollisions = value;

					return shared_from_this();
				}

				inline
				float 
				linearDamping() const
				{
					return _linearDamping;
				}

				inline
				float 
				angularDamping() const
				{
					return _angularDamping;
				}

				Ptr
				damping(float linearDamping, float angularDamping);

				inline
				float
				linearSleepingThreshold() const
				{
					return _linearSleepingThreshold;
				}

				inline
				float
				angularSleepingThreshold() const
				{
					return _angularSleepingThreshold;
				}

				Ptr
				sleepingThresholds(float linearSleepingThreshold, float angularSleepingThreshold);

				inline
				std::shared_ptr<Signal<Ptr>>
				propertiesChanged() const
				{
					return _propertiesChanged;
				}

				inline
				std::shared_ptr<Signal<Ptr, Ptr>>
				collisionStarted() const
				{
					return _collisionStarted;
				}

				inline
				std::shared_ptr<Signal<Ptr, Ptr>>
				collisionEnded() const
				{
					return _collisionEnded;
				}

			private:
				Collider(ColliderDataPtr);

				void
				initialize();

				void
				initializeFromNode(NodePtr);

				void
				targetAddedHandler(AbsCtrlPtr, NodePtr);

				void
				targetRemovedHandler(AbsCtrlPtr, NodePtr);

				void 
				addedHandler(NodePtr, NodePtr, NodePtr);

				void
				removedHandler(NodePtr, NodePtr, NodePtr);
			};
		}
	}
}
