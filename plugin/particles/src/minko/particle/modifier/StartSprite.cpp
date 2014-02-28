#include "minko/particle/modifier/StartSprite.hpp"
#include "minko/data/Provider.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko;
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


void
StartSprite::setProperties(ProviderPtr provider)
{
	provider->set("particles.spritesheet", true);
	provider->set("particles.spriteSheetRows", 2.f);
	provider->set("particles.spriteSheetColumns", 2.f);
}
				
void
StartSprite::unsetProperties(ProviderPtr provider)
{
	provider->unset("particles.spritesheet");
	provider->unset("particles.spriteSheetRows");
	provider->unset("particles.spriteSheetColumns");
}