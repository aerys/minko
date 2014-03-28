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

#include "minko/component/bullet/CylinderShape.hpp"

#include "minko/render/AbstractContext.hpp"
#include "minko/geometry/LineGeometry.hpp"
#include "minko/math/Vector3.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::geometry;

LineGeometry::Ptr
bullet::CylinderShape::getGeometry(render::AbstractContext::Ptr context) const
{
	static const unsigned int	numSteps	= 36;
	static const float			angStep		= 2.0f * float(PI) / float(numSteps); 
	static const float			cAngStep	= cosf(angStep);
	static const float			sAngStep	= sinf(angStep);
	
	const auto halfExtentX = _halfExtentX * localScaling()->x();
	const auto halfExtentY = _halfExtentY * localScaling()->y();
	const auto halfExtentZ = _halfExtentZ * localScaling()->z();

	auto lines = LineGeometry::create(context)

		->moveTo(-halfExtentX, -halfExtentY, 0.0f)
		->lineTo(-halfExtentX,  halfExtentY, 0.0f)
	
		->moveTo(halfExtentX, -halfExtentY, 0.0f)
		->lineTo(halfExtentX,  halfExtentY, 0.0f)

		->moveTo(0.0f, -halfExtentY, -halfExtentZ)
		->lineTo(0.0f,  halfExtentY, -halfExtentZ)

		->moveTo(0.0f, -halfExtentY, halfExtentZ)
		->lineTo(0.0f,  halfExtentY, halfExtentZ);

	for (unsigned int j = 0; j < 2; ++j)
	{
		const float y = (j & 0x1) == 0 ? -halfExtentY : halfExtentY;

		float cAng = 1.0f;
		float sAng = 0.0f;

		lines->moveTo(halfExtentX * cAng, y, halfExtentZ * sAng);

		for (unsigned int i = 0; i < numSteps; ++i)
		{
			float c	= cAng * cAngStep - sAng * sAngStep;
			float s	= cAng * sAngStep + sAng * cAngStep;
			
			cAng	= c;
			sAng	= s;
	
			lines->lineTo(halfExtentX * cAng, y, halfExtentZ * sAng);
		}
	}

	return lines;
}