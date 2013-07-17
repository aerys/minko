#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/modifier/Modifier3.hpp"
#include "minko/particle/modifier/IParticleUpdater.hpp"

namespace minko
{
	namespace particle
	{
		namespace modifier
		{
			class SizeOverTime : public IParticleUpdater
			{
			public:
				typedef std::shared_ptr<SizeOverTime>	Ptr;

			public:
				static
				Ptr
				create()
				{
					Ptr modifier = std::shared_ptr<SizeOverTime>(new SizeOverTime());

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
				SizeOverTime();
			};
		}
	}
}