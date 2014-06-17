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
#include <minko/component/bullet/AbstractPhysicsShape.hpp>

namespace minko
{
	namespace component
	{
		namespace bullet
		{
			class CylinderShape:
				public AbstractPhysicsShape
			{
			public:
				typedef std::shared_ptr<CylinderShape>				Ptr;

			private:
				typedef std::shared_ptr<geometry::LineGeometry>		LineGeometryPtr;
				typedef std::shared_ptr<render::AbstractContext>	AbsContextPtr;

			private:
				float												_halfExtentX;
				float												_halfExtentY;
				float												_halfExtentZ;

			public:
				inline static
					Ptr
					create(float halfExtentX, float halfExtentY, float halfExtentZ)
				{
					return std::shared_ptr<CylinderShape>(new CylinderShape(halfExtentX, halfExtentY, halfExtentZ));
				}

				inline 
				float 
				halfExtentX() const
				{
					return _halfExtentX;
				}

				inline
				void
				halfExtentX(float halfExtentX)
				{
					const bool needsUpdate	= fabsf(halfExtentX - _halfExtentX) > 1e-6f;
					_halfExtentX	= halfExtentX;
					if (needsUpdate)
						shapeChanged()->execute(shared_from_this());
				}

				inline 
				float 
				halfExtentY() const
				{
					return _halfExtentY;
				}

				inline
				void
				halfExtentY(float halfExtentY)
				{
					const bool needsUpdate	= fabsf(halfExtentY - _halfExtentY) > 1e-6f;
					_halfExtentY	= halfExtentY;
					if (needsUpdate)
						shapeChanged()->execute(shared_from_this());
				}

				inline 
				float 
				halfExtentZ() const
				{
					return _halfExtentZ;
				}

				inline
				void
				halfExtentZ(float halfExtentZ)
				{
					const bool needsUpdate	= fabsf(halfExtentZ - _halfExtentZ) > 1e-6f;
					_halfExtentZ	= halfExtentZ;
					if (needsUpdate)
						shapeChanged()->execute(shared_from_this());
				}

				inline
				float
				volume() const
				{
					const float volume	= (float)PI * (_halfExtentX + _margin) * (_halfExtentZ + _margin) // elliptical basis area approx
						* (_halfExtentY + _margin);

					return volume * _volumeScaling;
				}

				LineGeometryPtr
				getGeometry(AbsContextPtr) const;

			private:
				CylinderShape(float halfExtentX, float halfExtentY, float halfExtentZ):
					AbstractPhysicsShape(CYLINDER),
					_halfExtentX(halfExtentX),
					_halfExtentY(halfExtentY),
					_halfExtentZ(halfExtentZ)
				{
				}
			};
		}
	}
}
