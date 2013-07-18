#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/modifier/IParticleModifier.hpp"

namespace minko
{
	namespace particle
	{
		namespace modifier
		{
			class IParticleInitializer : public IParticleModifier
			{
			public:
				typedef std::shared_ptr<IParticleInitializer> Ptr;

			public:	
				virtual
				void
				initialize(ParticleData& 	particle,
						   float			time) const = 0;
			};
		}
	}
}