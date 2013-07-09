#include "EmitterShape.hpp"
#include "minko/particle/ParticleData.hpp"

using namespace minko::particle;
using namespace minko::particle::shape;

inline
void
EmitterShape::initPositionAndDirection(ParticleData& particle) const
{
	initPosition(particle);
	initDirection(particle);
}

inline
void
EmitterShape::initDirection(ParticleData& particle) const
{
	particle.startvx 	= particle.x;
	particle.startvy 	= particle.y;
	particle.startvz 	= particle.z;
}
