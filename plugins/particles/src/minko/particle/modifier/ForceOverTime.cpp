#include "minko/particle/modifier/ForceOverTime.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::modifier;

ForceOverTime::ForceOverTime(SamplerPtr fx,
			     			 SamplerPtr fy,
			     			 SamplerPtr fz)
	: Modifier3<float> (fx, fy, fz)
{}

void
ForceOverTime::update(std::vector<ParticleData>& 	particles,
		 		   	  float							timeStep) const
{
	float sqTime	= timeStep * timeStep;

	for (unsigned int particleIndex = 0; particleIndex < particles.size(); ++particleIndex)
	{
		ParticleData& particle = particles[particleIndex];

		if (particle.alive)
		{
			particle.x += _x->value(particle.timeLived / particle.lifetime) * sqTime;
			particle.y += _y->value(particle.timeLived / particle.lifetime) * sqTime;
			particle.z += _z->value(particle.timeLived / particle.lifetime) * sqTime;
		}
	}
}


unsigned int
ForceOverTime::getNeededComponents() const
{
	return VertexComponentFlags::DEFAULT;
}

