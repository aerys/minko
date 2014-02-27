#include "minko/particle/shape/Cone.hpp"
#include "minko/particle/ParticleData.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::shape;

Cone::Cone(float	angle,
		   float 	baseRadius,
		   float	length, 
		   float 	innerRadius)
	: _angle (angle),
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
	float theta		= (rand01() * 2.f - 1.f) * (float)PI;
	
	float cosTheta	= cos(theta);
	float sinTheta	= sin(theta);
	
	float r			= _innerRadius + sqrt(rand01()) * (_baseRadius - _innerRadius);
	
	r = rand01() > .5 ? r : -r;

	particle.x = r * cosTheta;
	particle.y = 0;
	particle.z = r * sinTheta;

	float angle		= _angle * r / _baseRadius;
	float height	= rand01() * _length * cos(angle);

	r += height * tan(angle);

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


