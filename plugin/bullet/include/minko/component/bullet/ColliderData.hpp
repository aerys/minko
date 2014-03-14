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
#include "minko/Signal.hpp"

namespace minko
{
	namespace component
	{
		namespace bullet
		{
			class AbstractPhysicsShape;
			class DynamicsProperties;
            class Collider;

			class ColliderData:
				public std::enable_shared_from_this<ColliderData>
			{
                friend class component::bullet::Collider;

			public:
				typedef std::shared_ptr<ColliderData> Ptr;

                typedef std::shared_ptr<scene::Node>            NodePtr;
				typedef std::shared_ptr<AbstractPhysicsShape>	AbsShapePtr;
				typedef std::shared_ptr<math::Matrix4x4>		Matrix4x4Ptr;
				typedef std::shared_ptr<math::Quaternion>		QuaternionPtr;
				typedef std::shared_ptr<math::Vector3>			Vector3Ptr;

            private:
                static const short  DEFAULT_DYNAMIC_FILTER;
                static const short  DEFAULT_STATIC_FILTER;
                static const short  DEFAULT_DYNAMIC_MASK;
                static const short  DEFAULT_STATIC_MASK;

			private:
				uint  	                                    _uid;

                NodePtr                                     _node;
				const float                                 _mass;
				Matrix4x4Ptr	                            _correctionMatrix;
				AbsShapePtr		                            _shape;
				Vector3Ptr		                            _inertia;

                short                                       _collisionGroup;
                short                                       _collisionMask;

				Vector3Ptr		                            _linearVelocity;
				Vector3Ptr		                            _linearFactor;
				float			                            _linearDamping;
				float			                            _linearSleepingThreshold;
				Vector3Ptr		                            _angularVelocity;
				Vector3Ptr		                            _angularFactor;
				float			                            _angularDamping;
				float			                            _angularSleepingThreshold;
				float			                            _restitution;       // from bullet: best simulation results using zero restitution. 
				float			                            _friction;          // from bullet: best simulation results when friction is non-zero 
				float			                            _rollingFriction;

				bool			                            _deactivationDisabled;
				bool			                            _triggerCollisions;

				std::shared_ptr<Signal<Ptr, Matrix4x4Ptr>>	_physicsWorldTransformChanged;
				std::shared_ptr<Signal<Ptr, Matrix4x4Ptr>>	_graphicsWorldTransformChanged;
				std::shared_ptr<Signal<Ptr, Ptr>>			_collisionStarted;
				std::shared_ptr<Signal<Ptr, Ptr>>			_collisionEnded;
                std::shared_ptr<Signal<Ptr>>                _collisionFilterChanged;

			public:
				inline static
				Ptr
				create(float mass, AbsShapePtr shape, Vector3Ptr inertia = nullptr)
				{
					return std::shared_ptr<ColliderData>(new ColliderData(mass, shape, inertia));
				}

				inline
				AbsShapePtr
				shape() const
				{
					return _shape;
				}

				inline
				float
				mass() const
				{
					return _mass;
				}

				inline
				bool
				isStatic() const
				{
					return _mass < 1e-6f;
				}

                inline
                short
                collisionGroup() const
                {
                    return _collisionGroup;
                }

                void
                collisionGroup(short);

                inline
                short
                collisionMask() const
                {
                    return _collisionMask;
                }

                void
                collisionMask(short);

				inline
				Vector3Ptr
				inertia() const
				{
					return _inertia;
				}

				inline
				uint 
                uid() const
				{
					return _uid;
				}

				inline
				void 
                uid(uint value)
				{
					_uid = value;
				}

                inline
                NodePtr
                node() const
                {
                    return _node;
                }

				inline
				Vector3Ptr
				linearVelocity() const
				{
					return _linearVelocity;
				}

				void
				linearVelocity(float, float, float);

				inline
				Vector3Ptr
				linearFactor() const
				{
					return _linearFactor;
				}

				void
				linearFactor(float, float, float);

				inline
				float
				linearDamping() const
				{
					return _linearDamping;
				}

				inline
				void 
				linearDamping(float value)
				{
					_linearDamping	= value;
				}

				inline
				Vector3Ptr
				angularVelocity() const
				{
					return _angularVelocity;
				}

				void
				angularVelocity(float, float, float);

				inline
				Vector3Ptr
				angularFactor() const
				{
					return _angularFactor;
				}

				void
				angularFactor(float, float, float);

				inline
				float
				angularDamping() const
				{
					return _angularDamping;
				}

				inline
				void 
				angularDamping(float value)
				{
					_angularDamping	= value;
				}

				inline
				float
				restitution() const
				{
					return _restitution;
				}

				inline
				void
				restitution(float value)
				{
					_restitution = value;
				}

				inline 
				bool
				deactivationDisabled() const
				{
					return _deactivationDisabled;
				}

				inline
				void
				disableDeactivation(bool value)
				{
					_deactivationDisabled = value;
				}

				inline
				bool 
				triggerCollisions() const
				{
					return _triggerCollisions;
				}

				inline
				void
				triggerCollisions(bool value)
				{
					_triggerCollisions = value;
				}

				void
				correction(Matrix4x4Ptr);

				inline
				Matrix4x4Ptr
				correction() const
				{
					return _correctionMatrix;
				}

				inline
				float
				linearSleepingThreshold()
				{
					return _linearSleepingThreshold;
				}

				inline
				void
				linearSleepingThreshold(float value)
				{
					_linearSleepingThreshold = value;
				}

				inline
				float
				angularSleepingThreshold() const
				{
					return _angularSleepingThreshold;
				}

				inline
				void
				angularSleepingThreshold(float value)
				{
					_angularSleepingThreshold = value;
				}

				inline
				float
				friction() const 
				{
					return _friction;
				}

				inline
				void
				friction(float value)
				{
					_friction = value;
				}

				inline
				float 
				rollingFriction() const
				{
					return _rollingFriction;
				}

				inline
				void
				rollingFriction(float value)
				{
					_rollingFriction = value;
				}

				inline
				Signal<Ptr, Matrix4x4Ptr>::Ptr
				physicsWorldTransformChanged() const
				{
					return _physicsWorldTransformChanged;
				}

				inline
				Signal<Ptr, Matrix4x4Ptr>::Ptr
				graphicsWorldTransformChanged() const
				{
					return _graphicsWorldTransformChanged;
				}

				inline
				Signal<Ptr, Ptr>::Ptr
				collisionStarted() const
				{
					return _collisionStarted;
				}

				inline
				Signal<Ptr, Ptr>::Ptr
				collisionEnded() const
				{
					return _collisionEnded;
				}

                inline
                Signal<Ptr>::Ptr
                collisionFilterChanged() const
                {
                    return _collisionFilterChanged;
                }

			private:
				ColliderData(float, AbsShapePtr, Vector3Ptr = nullptr);
                ColliderData(float, short, short, AbsShapePtr, Vector3Ptr = nullptr);
			};
		}
	}
}
