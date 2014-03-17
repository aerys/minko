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

#include "minko/component/bullet/SphereShape.hpp"

#include "minko/render/AbstractContext.hpp"
#include "minko/geometry/LineGeometry.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::geometry;

LineGeometry::Ptr
bullet::SphereShape::getGeometry(render::AbstractContext::Ptr context) const
{
	static const unsigned int	numSteps	= 36;
	static const float			angStep		= 2.0f * float(PI) / float(numSteps); 
	static const float			cAngStep	= cosf(angStep);
	static const float			sAngStep	= sinf(angStep);

	auto lines = LineGeometry::create(context);
	
	for (unsigned int j = 0; j < 3; ++j)
	{
		float cAng = 1.0f;
		float sAng = 0.0f;
		
		if (j == 0)
			lines->moveTo(0.0f,				_radius * cAng,	_radius * sAng);
		else if (j == 1)
			lines->moveTo(_radius * cAng,	0.0f,			_radius * sAng);
		else
			lines->moveTo(_radius * cAng,	_radius * sAng,	0.0f);
		
		for (unsigned int i = 0; i < numSteps; ++i)
		{
			float c	= cAng * cAngStep - sAng * sAngStep;
			float s	= cAng * sAngStep + sAng * cAngStep;
			
			cAng	= c;
			sAng	= s;
		
			if (j == 0)
				lines->lineTo(0.0f,				_radius * cAng,	_radius * sAng);
			else if (j == 1)
				lines->lineTo(_radius * cAng,	0.0f,			_radius * sAng);
			else
				lines->lineTo(_radius * cAng,	_radius * sAng,	0.0f);
		}
	}

	return lines;
}