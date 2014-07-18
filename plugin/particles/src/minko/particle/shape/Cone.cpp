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

#include "minko/particle/shape/Cone.hpp"
#include "minko/particle/ParticleData.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::shape;

Cone::Cone(float angle,
		   float baseRadius,
		   float length,
		   float innerRadius) :
	EmitterShape(),
	_angle (angle),
	_baseRadius (baseRadius),
	_length (length),
	_innerRadius (innerRadius)
{}

void
Cone::initPosition(ParticleData& particle) const
{
	initParticle(particle, false);
}

void
Cone::initPositionAndDirection(ParticleData& particle) const
{
	initParticle(particle, true);
}

void
Cone::initParticle(ParticleData& particle,
				   bool direction) const
{
	float theta		= (tools::rand01() * 2.f - 1.f) * float(M_PI);
	
	float cosTheta	= cosf(theta);
	float sinTheta	= sinf(theta);
	
	float r			= _innerRadius + sqrt(tools::rand01()) * (_baseRadius - _innerRadius);
	
	r = tools::rand01() > .5 ? r : -r;

	particle.x = r * cosTheta;
	particle.y = 0;
	particle.z = r * sinTheta;

	float angle		= _angle * r / _baseRadius;
	float height	= tools::rand01() * _length * cos(angle);

	r += height * tanf(angle);

	if (direction)
	{
		particle.startvx = r * cosTheta - particle.x;
		particle.startvy = height;
		particle.startvz = r * sinTheta - particle.z;
	}

	particle.x = r * cosTheta;
	particle.y = height;
	particle.z = r * sinTheta;
}
