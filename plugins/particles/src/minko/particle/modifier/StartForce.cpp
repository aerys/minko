#include "minko/particle/modifier/StartForce.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::modifier;

StartForce::StartForce(SamplerPtr fx,
					   SamplerPtr fy,
					   SamplerPtr fz)
	: Modifier3<float> (fx, fy, fz)
{
}

void
StartForce::initialize(ParticleData& 	particle,
	    			   float				time) const
{
	particle.startfx = _x->value();
	particle.startfy = _y->value();
	particle.startfz = _z->value();

	particle.x += particle.startfx * time * time;
	particle.y += particle.startfy * time * time;
	particle.z += particle.startfz * time * time;
}

unsigned int
StartForce::getNeededComponents() const
{
	return VertexComponentFlags::DEFAULT;
}