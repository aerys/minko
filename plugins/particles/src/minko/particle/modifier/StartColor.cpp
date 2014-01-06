#include "minko/particle/modifier/StartColor.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"
#include "minko/particle/sampler/Sampler.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::modifier;

StartColor::StartColor(SamplerPtr color)
	: Modifier1<math::Vector3> (color)
{
}

void
StartColor::initialize(ParticleData& 	particle,
	    			   float			time) const
{
	/*static math::Vector3 c;
	_x->set(&c);

	particle.r = c.x();
	particle.g = c.y();
	particle.b = c.z();*/
}

unsigned int
StartColor::getNeededComponents() const
{
	return VertexComponentFlags::COLOR;
}