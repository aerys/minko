#include "minko/particle/modifier/StartRotation.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::modifier;

StartRotation::StartRotation(SamplerPtr angle)
	: Modifier1<float> (angle)
{
}

void
StartRotation::initialize(ParticleData& 	particle,
	    			 	  float				time) const
{
	particle.rotation = _x->value();
}


unsigned int
StartRotation::getNeededComponents() const
{
	return VertexComponentFlags::ROTATION;
}