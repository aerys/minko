#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/modifier/IParticleUpdater.hpp"

namespace minko
{
	namespace particle
	{
		namespace modifier
		{
			class ColorOverTime : public IParticleUpdater
			{
			public:
				typedef std::shared_ptr<ColorOverTime>	Ptr;

			public:
				static
				Ptr
				create()
				{
					Ptr modifier = std::shared_ptr<ColorOverTime>(new ColorOverTime());

					return modifier;
				};

				virtual
				void
				update(std::vector<ParticleData>& 	particles,
					   float						timeStep) const;

				virtual
				unsigned int
				getNeededComponents() const;

			protected:
				ColorOverTime();
			};
		}
	}
}