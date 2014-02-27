#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/modifier/IParticleUpdater.hpp"

namespace minko
{
	namespace particle
	{
		namespace modifier
		{
			class ColorBySpeed : public IParticleUpdater
			{
			public:
				virtual
				void
				update(std::vector<ParticleData>& 	particles,
					   float						timeStep) const;

				virtual
				unsigned int
				getNeededComponents() const;
			};
		}
	}
}