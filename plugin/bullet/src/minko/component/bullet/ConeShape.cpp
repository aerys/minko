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

#include "minko/component/bullet/ConeShape.hpp"

#include "minko/render/AbstractContext.hpp"
#include "minko/geometry/LineGeometry.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::geometry;

LineGeometry::Ptr
bullet::ConeShape::getGeometry(render::AbstractContext::Ptr context) const
{
	static const unsigned int	numSteps	= 18;
	static const float			angStep		= 2.0f * float(PI) / float(numSteps); 
	static const float			cAngStep	= cosf(angStep);
	static const float			sAngStep	= sinf(angStep);

	const float upperY	= 0.5f * _height;
	const float lowerY	= -0.5f * _height;
	auto lines			= LineGeometry::create(context)

		->moveTo(0.0f,		upperY,	0.0f)
		->lineTo(-_radius,	lowerY,	0.0f)

		->moveTo(0.0f,		upperY,	0.0f)
		->lineTo(_radius,	lowerY,	0.0f)

		->moveTo(0.0f,		upperY,	0.0f)
		->lineTo(0.0f,		lowerY,	-_radius)

		->moveTo(0.0f,		upperY,	0.0f)
		->lineTo(0.0f,		lowerY,	_radius);

	float cAng = 1.0f;
	float sAng = 0.0f;

	lines->moveTo(_radius * cAng, lowerY, _radius * sAng);

	for (unsigned int i = 0; i < numSteps; ++i)
	{
		float c	= cAng * cAngStep - sAng * sAngStep;
		float s	= cAng * sAngStep + sAng * cAngStep;
		
		cAng	= c;
		sAng	= s;

		lines->lineTo(_radius * cAng, lowerY, _radius * sAng);
	}

	return lines;
}