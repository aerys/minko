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
				Matrix4x4Ptr	_centerOfMassOffset;
				Matrix4x4Ptr	_scaleCorrectionMatrix; 
				AbsShapePtr		_shape;
				Vector3Ptr		_inertia;

				Vector3Ptr		_linearVelocity;
				Vector3Ptr		_linearFactor;
				float			_linearDamping;
				Vector3Ptr		_angularVelocity;
				Vector3Ptr		_angularFactor;
				float			_angularDamping;
				float			_restitution;

				std::shared_ptr<Signal<Ptr>>	_transformChanged;

			public:
				inline static
					Ptr
					create(float	mass, 					
					AbsShapePtr		shape, 
					Vector3Ptr		inertia = nullptr,
					Matrix4x4Ptr	centerOfMassOffset = nullptr)
				{
					return std::shared_ptr<Collider>(new Collider(mass, shape, inertia, centerOfMassOffset));
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
					centerOfMassOffset() const
				{
					return _centerOfMassOffset;
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
					Signal<Ptr>::Ptr
					transformChanged()
				{
					return _transformChanged;
				}

			private:
				Collider(float,
					AbsShapePtr, 
					Vector3Ptr		inertia				= nullptr,
					Matrix4x4Ptr	centerOfMassOffset	= nullptr);
			};
		}
	}
}
