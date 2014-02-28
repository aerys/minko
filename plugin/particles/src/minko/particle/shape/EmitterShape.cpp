#include "minko/particle/shape/EmitterShape.hpp"
#include "minko/particle/ParticleData.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::shape;

void
EmitterShape::initPositionAndDirection(ParticleData& particle) const
{
	initPosition(particle);
	initDirection(particle);
}

void
EmitterShape::initDirection(ParticleData& particle) const
{
	particle.startvx 	= particle.x;
	particle.startvy 	= particle.y;
	particle.startvz 	= particle.z;
}
