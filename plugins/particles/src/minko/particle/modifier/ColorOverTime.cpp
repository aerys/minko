#include "minko/particle/modifier/ColorOverTime.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::modifier;

ColorOverTime::ColorOverTime()
{}

void
ColorOverTime::update(std::vector<ParticleData>& 	particles,
 					  float							timeStep) const
{
	return;
}

unsigned int
ColorOverTime::getNeededComponents() const
{
	return VertexComponentFlags::TIME;
}