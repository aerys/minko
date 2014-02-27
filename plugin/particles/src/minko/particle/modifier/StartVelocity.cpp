#include "minko//particle/modifier/StartVelocity.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::modifier;

StartVelocity::StartVelocity(SamplerPtr vx,
						     SamplerPtr vy,
						     SamplerPtr vz)
	: Modifier3<float> (vx, vy, vz)
	{
	}

void
StartVelocity::initialize(ParticleData& 	particle,
	    			  	  float				time) const 
{
	particle.startvx += _x->value();
	particle.startvy += _y->value();
	particle.startvz += _z->value();

	particle.x += particle.startvx * time;
	particle.y += particle.startvy * time;
	particle.z += particle.startvz * time;
}

unsigned int
StartVelocity::getNeededComponents() const 
{
	return VertexComponentFlags::DEFAULT;
}