#include "minko/particle/modifier/SizeBySpeed.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::modifier;

void
SizeBySpeed::update(std::vector<ParticleData>& 	particles,
 		   			float						timeStep) const
{
	return;
}

unsigned int
SizeBySpeed::getNeededComponents() const
{
	return VertexComponentFlags::OLD_POSITION;
}