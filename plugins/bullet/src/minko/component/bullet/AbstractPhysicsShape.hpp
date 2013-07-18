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
			class AbstractPhysicsShape:
				public std::enable_shared_from_this<AbstractPhysicsShape>
			{
			public:
				typedef std::shared_ptr<AbstractPhysicsShape> Ptr;

				enum Type
				{
					SPHERE,
					BOX,
					CONE,
					CYLINDER
				};

			private:
				typedef std::shared_ptr<math::Vector3>		Vector3Ptr;
				typedef std::shared_ptr<math::Quaternion>	QuaternionPtr;
				typedef std::shared_ptr<math::Matrix4x4>	Matrix4x4Ptr;

			protected:
				Type			_type;
				float			_margin;
				float			_localScaling;
				Matrix4x4Ptr	_centerOfMassOffset; // TODO: should disappear soon

				Vector3Ptr		_centerOfMassTranslation;
				QuaternionPtr	_centerOfMassRotation;

			private:
				std::shared_ptr<Signal<Ptr>> _shapeChanged;

			public:
				AbstractPhysicsShape(Type);

				virtual
				~AbstractPhysicsShape()
				{
				}

				inline 
				float 
				localScaling() const 
				{
					return _localScaling;
				}

				inline 
				void 
				setLocalScaling(float value)
				{
					_localScaling = value;
				}

				// testing stuff
				// TODO: should disappear soon
				virtual
				void
				apply(Matrix4x4Ptr) = 0;
				// TODO: should disappear soon
				inline
				Matrix4x4Ptr
				centerOfMassOffset() const
				{
					return _centerOfMassOffset;
				}
				// TODO: should disappear soon
				void
				setCenterOfMassOffset(Matrix4x4Ptr);
				// end of testing stuff...

				inline
				Type
				type() const
				{
					return _type;
				}

				inline
				float
				margin() const
				{
					return _margin;
				}

				inline
				void
				setMargin(float margin)
				{
					const bool needsUpdate	= fabsf(margin - _margin) > 1e-6f;
					_margin	= margin;
					if (needsUpdate)
						shapeChanged()->execute(shared_from_this());
				}

				inline
				Vector3Ptr
				centerOfMassTranslation() const
				{
					return _centerOfMassTranslation;
				}

				inline
				QuaternionPtr
				centerOfMassRotation() const
				{
					return _centerOfMassRotation;
				}

				inline
				Signal<Ptr>::Ptr
				shapeChanged()
				{
					return _shapeChanged;
				}
			};
		}
	}
}
