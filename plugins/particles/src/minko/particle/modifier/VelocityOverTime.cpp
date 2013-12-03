#include "minko/particle/modifier/VelocityOverTime.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::modifier;

VelocityOverTime::VelocityOverTime(SamplerPtr vx,
			     				   SamplerPtr vy,
			     				   SamplerPtr vz)
	: Modifier3<float> (vx, vy, vz)
{}

void
VelocityOverTime::update(std::vector<ParticleData>& 	particles,
		 		   		 float							timeStep) const
{
	for (unsigned int particleIndex = 0; particleIndex < particles.size(); ++particleIndex)
	{
		ParticleData& particle = particles[particleIndex];

		if (particle.alive)
		{
			particle.x += _x->value(particle.timeLived / particle.lifetime) * timeStep;
			particle.y += _y->value(particle.timeLived / particle.lifetime) * timeStep;
			particle.z += _z->value(particle.timeLived / particle.lifetime) * timeStep;
		}
	}
}


unsigned int
VelocityOverTime::getNeededComponents() const
{
	return VertexComponentFlags::DEFAULT;
}

