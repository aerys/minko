#include "minko/particle/shape/Sphere.hpp"
#include "minko/particle/ParticleData.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::shape;

Sphere::Sphere(float	radius,
		   	   float 	innerRadius)
	: _radius (radius),
	  _innerRadius (innerRadius)
{}
	
void
Sphere::initPosition(ParticleData& particle) const
{
	float u			= rand01();
	float sqrt1mu2	= sqrt(1 - u * u);
	
	float theta		= (rand01() * 2.f - 1.f) * (float)PI;
	
	float cosTheta	= cos(theta);
	float sinTheta	= sin(theta);
	
	float r			= _innerRadius + sqrt(rand01()) * (_radius - _innerRadius);
	
	r = rand01() > .5f ? r : -r;

	particle.x = r * sqrt1mu2 * cosTheta;
	particle.y = r * sqrt1mu2 * sinTheta;
	particle.z = r * u;
}



