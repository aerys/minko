#include "StartSprite.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko::particle;
using namespace minko::particle::modifier;


StartSprite::StartSprite(SamplerPtr spriteIndex)
	: Modifier1<float> (spriteIndex)
{
}

void
StartSprite::initialize(ParticleData& 	particle,
	    			    float			time) const
{
	particle.spriteIndex = _x->value();
}

unsigned int
StartSprite::getNeededComponents() const
{
	return VertexComponentFlags::SPRITEINDEX;
}