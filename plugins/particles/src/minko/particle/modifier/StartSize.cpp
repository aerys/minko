#include "minko/particle/modifier/StartSize.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::modifier;

StartSize::StartSize(SamplerPtr size)
	: Modifier1<float> (size)
{
}

void
StartSize::initialize(ParticleData& 	particle,
	    			  float				time) const
{
	particle.size = _x->value();
}


unsigned int
StartSize::getNeededComponents() const
{
	return VertexComponentFlags::SIZE;
}