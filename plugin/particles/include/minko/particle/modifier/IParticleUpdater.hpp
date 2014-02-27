#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/modifier/IParticleModifier.hpp"

namespace minko
{
	namespace particle
	{
		namespace modifier
		{
			class IParticleUpdater : public IParticleModifier
			{
			public:
				typedef std::shared_ptr<IParticleUpdater> Ptr;

			public:
				virtual
				void
				update(std::vector<ParticleData>& 	particles,
					  float							timeStep) const = 0;
			};
		}
	}
}