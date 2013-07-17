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

			protected:
				Type	_type;
				float	_margin;
				float	_localScaleX;
				float	_localScaleY;
				float	_localScaleZ;

			private:
				std::shared_ptr<Signal<Ptr>> _shapeChanged;

			public:
				AbstractPhysicsShape(Type type):
					_type(type),
					_margin(0.0f),
					_localScaleX(1.f),
					_localScaleY(1.f),
					_localScaleZ(1.f),
					_shapeChanged(Signal<Ptr>::create())
				{
				}

				virtual
					~AbstractPhysicsShape()
				{
				}

				inline float localScaleZ() const 
				{
					return _localScaleZ;
				}

				inline void localScaleZ(float value)
				{
					_localScaleZ = value;
				}


				inline float localScaleY() const 
				{
					return _localScaleY;
				}

				inline void localScaleY(float value)
				{
					_localScaleY = value;
				}

				inline float localScaleX() const 
				{
					return _localScaleX;
				}

				inline void localScaleX(float value)
				{
					_localScaleX = value;
				}

				virtual
				void
				apply(std::shared_ptr<math::Matrix4x4> matrix) = 0;

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
				Signal<Ptr>::Ptr
				shapeChanged()
				{
					return _shapeChanged;
				}
			};
		}
	}
}
