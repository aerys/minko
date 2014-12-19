/*
Copyright (c) 2014 Aerys

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
#include "Geometry.hpp"

namespace minko
{
	namespace geometry
	{
		class SphereGeometry :
			public Geometry
		{
		public:
			typedef std::shared_ptr<SphereGeometry>	Ptr;

		public:
			inline static
			Ptr
			create(std::shared_ptr<render::AbstractContext>	context,
					unsigned int							numParallels	= 10,
					unsigned int							numMeridians	= 0,
					bool									withNormals		= true)
			{
				numMeridians = numMeridians != 0 ? numMeridians : numParallels;

				auto geom = std::shared_ptr<SphereGeometry>(new SphereGeometry());

				geom->initializeVertices(context, numParallels, numMeridians, withNormals);
				geom->initializeIndices(context, numParallels, numMeridians);

				return geom;
			}

        protected:
            SphereGeometry() :
                Geometry("sphere")
            {
            }

		private:
			void
			initializeVertices(std::shared_ptr<render::AbstractContext>	context,
							   unsigned int								numParallels,
							   unsigned int								numMeridians,
							   bool										withNormals);

			void
			initializeIndices(std::shared_ptr<render::AbstractContext>	context,
								unsigned int							numParallels,
								unsigned int							numMeridians);
		};
	}
}
