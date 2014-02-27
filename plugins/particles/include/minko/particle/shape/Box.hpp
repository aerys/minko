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
			class Box : public EmitterShape
			{
			public:
				typedef std::shared_ptr<Box>	Ptr;
				
			private:
				float _width;
				float _height;
				float _length;
				float _limitToSides;

			public:
				static
				Ptr
				create(float	width,
					   float 	height,
					   float	length,
					   bool 	limitToSides)
				{
					Ptr box = std::shared_ptr<Box>(new Box(width,
														   height,
														   length,
														   limitToSides));

					return box;
				};

				inline
				void
				width(float value)
				{
					_width = value;
				};
				
				inline
				void
				height(float value)
				{
					_height = value;
				};
				
				inline
				void
				length(float value)
				{
					_length = value;
				};
				
				inline
				void
				limitToSides(float value)
				{
					_limitToSides = value;
				};

				virtual
				void
				initPosition(ParticleData& particle) const;

			protected:
				Box(float	width,
					float 	height,
					float	length,
					bool 	limitToSides);
			};
		}
	}
}