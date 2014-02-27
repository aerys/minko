#include "minko/particle/shape/Cylinder.hpp"
#include "minko/particle/ParticleData.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::shape;

Cylinder::Cylinder(float	height,
				   float	radius,
		   		   float 	innerRadius)
	: _height (height),
	  _radius (radius),
	  _innerRadius (innerRadius)
{}

void
Cylinder::initPosition(ParticleData& particle) const
{
	float theta		= (rand01() * 2.f - 1.f) * (float)PI;
	
	float cosTheta	= cos(theta);
	float sinTheta	= sin(theta);
	
	float r			= _innerRadius + sqrt(rand01()) * (_radius - _innerRadius);
	
	r = rand01() > .5 ? r : -r;

	particle.x = r * cosTheta;
	particle.y = rand01() * _height;
	particle.z = r * sinTheta;
}


