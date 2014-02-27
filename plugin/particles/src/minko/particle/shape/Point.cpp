#include "minko/particle/shape/Point.hpp"
#include "minko/particle/ParticleData.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::shape;

Point::Point()
{}
	
void
Point::initPosition(ParticleData& particle) const
{
	particle.x = 0;
	particle.y = 0;
	particle.z = 0;
}
