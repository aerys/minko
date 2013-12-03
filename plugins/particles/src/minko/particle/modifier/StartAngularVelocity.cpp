#include "minko/particle/modifier/StartAngularVelocity.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::modifier;

StartAngularVelocity::StartAngularVelocity(SamplerPtr w)
	: Modifier1<float> (w)
{
}

void
StartAngularVelocity::initialize(ParticleData& 	particle,
	    			  	 		 float			time) const 
{
	particle.startAngularVelocity = _x->value();

	particle.rotation += particle.startAngularVelocity * time;
}

unsigned int
StartAngularVelocity::getNeededComponents() const 
{
	return VertexComponentFlags::ROTATION;
}