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

			class Collider:
				public std::enable_shared_from_this<Collider>
			{
			public:
				typedef std::shared_ptr<Collider> Ptr;

				typedef std::shared_ptr<AbstractPhysicsShape>	AbsShapePtr;
				typedef std::shared_ptr<math::Matrix4x4>		Matrix4x4Ptr;
				typedef std::shared_ptr<math::Vector3>			Vector3Ptr;

			private:
				float			_mass;
				Matrix4x4Ptr	_worldTransform;
				float			_scaleCorrection;
				AbsShapePtr		_shape;
				Vector3Ptr		_inertia;

				Vector3Ptr		_linearVelocity;
				Vector3Ptr		_linearFactor;
				float			_linearDamping;
				float			_linearSleepingThreshold;
				Vector3Ptr		_angularVelocity;
				Vector3Ptr		_angularFactor;
				float			_angularDamping;
				float			_angularSleepingThreshold;
				float			_restitution; // from bullet: best simulation results using zero restitution. 
				float			_friction; // from bullet: best simulation results when friction is non-zero 
				float			_rollingFriction;

				bool			_deactivationDisabled;

				std::shared_ptr<Signal<Ptr>>	_transformChanged;

			public:
				inline static
				Ptr
				create(float mass, AbsShapePtr shape, Vector3Ptr inertia = nullptr)
				{
					return std::shared_ptr<Collider>(new Collider(mass, shape, inertia));
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
				Vector3Ptr
				inertia() const
				{
					return _inertia;
				}

				inline
				bool
				isStatic() const
				{
					return _mass < std::numeric_limits<float>::epsilon();
				}

				inline
				Matrix4x4Ptr
				worldTransform() const
				{
					return _worldTransform;
				}

				void
				setWorldTransform(Matrix4x4Ptr);

				void
				updateColliderWorldTransform(Matrix4x4Ptr);

				inline
				Vector3Ptr
				linearVelocity() const
				{
					return _linearVelocity;
				}

				inline
				Vector3Ptr
				linearFactor() const
				{
					return _linearFactor;
				}

				inline
				float
				linearDamping() const
				{
					return _linearDamping;
				}

				inline
				Vector3Ptr
				angularVelocity() const
				{
					return _angularVelocity;
				}

				inline
				Vector3Ptr
				angularFactor() const
				{
					return _angularFactor;
				}

				inline
				float
				angularDamping() const
				{
					return _angularDamping;
				}

				inline
				float
				restitution() const
				{
					return _restitution;
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

				void
				setLinearVelocity(float, float, float);

				void
				setLinearFactor(float, float, float);

				inline
				void 
				setLinearDamping(float value)
				{
					_linearDamping	= value;
				}

				void
				setAngularVelocity(float, float, float);

				void
				setAngularFactor(float, float, float);

				inline
				void 
				setAngularDamping(float value)
				{
					_angularDamping	= value;
				}

				inline
				void
				setRestitution(float value)
				{
					_restitution	= value;
				}

				inline
				float
				angularSleepingThreshold() const
				{
					return _angularSleepingThreshold;
				}

				inline
				float
				linearSleepingThreshold()
				{
					return _linearSleepingThreshold;
				}

				inline
				void
				setLinearSleepingThreshold(float value)
				{
					_linearSleepingThreshold = value;
				}

				inline
				void
				setAngularSleepingThreshold(float value)
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
				setFriction(float value)
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
				setRollingFriction(float value)
				{
					_rollingFriction = value;
				}

				inline
				Signal<Ptr>::Ptr
				transformChanged()
				{
					return _transformChanged;
				}

			private:
				Collider(float, AbsShapePtr, Vector3Ptr inertia	= nullptr);

				static
				Vector3Ptr
				getScaleCorrection(Matrix4x4Ptr, Vector3Ptr output = nullptr);

				static
				Matrix4x4Ptr
				applyScaleCorrection(Vector3Ptr, Matrix4x4Ptr);
			};
		}
	}
}
