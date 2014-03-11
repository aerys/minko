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

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/shape/EmitterShape.hpp"

namespace minko
{
	namespace particle
	{
		namespace shape
		{
			class Cylinder : public EmitterShape
			{
			public:
				typedef std::shared_ptr<Cylinder>	Ptr;
				
			private:
				float _height;
				float _radius;
				float _innerRadius;
			public:
				static
				Ptr
				create(float	height,
					   float	radius,
					   float 	innerRadius = 0)
				{
					Ptr cylinder = std::shared_ptr<Cylinder>(new Cylinder(height,
																		  radius,
																		  innerRadius));

					return cylinder;
				};
				
				inline
				void
				height(float value)
				{
					_height = value;
				};
				
				inline
				void
				radius(float value)
				{
					_radius = value;
				};
				
				inline
				void
				innerRadius(float value)
				{
					_innerRadius = value;
				};

				virtual
				void
				initPosition(ParticleData& particle) const;

			protected:
				Cylinder(float	height,
						 float	radius,
						 float 	innerRadius = 0);
			};
		}
	}
}