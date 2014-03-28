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
			class ConeShape:
				public AbstractPhysicsShape
			{
			public:
				typedef std::shared_ptr<ConeShape>					Ptr;

			private:
				typedef std::shared_ptr<geometry::LineGeometry>		LineGeometryPtr;
				typedef std::shared_ptr<render::AbstractContext>	AbsContextPtr;

			private:
				float												_radius;
				float												_height;

			public:
				inline static
				Ptr
				create(float radius, float height)
				{
					return std::shared_ptr<ConeShape>(new ConeShape(radius, height));
				}

				inline
				float
				radius() const
				{
					return _radius;
				}

				inline
				void
				radius(float radius)
				{
					const bool needsUpdate	= fabsf(radius - _radius) > 1e-6f;
					_radius	= radius;
					if (needsUpdate)
						shapeChanged()->execute(shared_from_this());
				}

				inline
				float
				height() const
				{
					return _height;
				}

				inline
				void
				height(float height)
				{
					const bool needsUpdate	= fabsf(height - _height) > 1e-6f;
					_height	= height;
					if (needsUpdate)
						shapeChanged()->execute(shared_from_this());
				}

				inline
				float
				volume() const
				{
					const float radius	= _radius + _margin; 
					const float volume	= ((float)PI / 3.0f) * radius * radius * (_height + _margin);

					return volume * _volumeScaling;
				}

				LineGeometryPtr
				getGeometry(AbsContextPtr) const;

			private:
				ConeShape(float radius, float height):
					AbstractPhysicsShape(CONE),
					_radius(radius),
					_height(height)
				{
				}
			};
		}
	}
}
