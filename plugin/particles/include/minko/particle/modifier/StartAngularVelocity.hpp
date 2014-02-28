#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/modifier/Modifier1.hpp"
#include "minko/particle/modifier/IParticleInitializer.hpp"

namespace minko
{
	namespace particle
	{
		namespace modifier
		{
			class StartAngularVelocity : public IParticleInitializer, public Modifier1<float>
			{
			public:
				typedef std::shared_ptr<StartAngularVelocity>	Ptr;

			public:
				static
				Ptr
				create(SamplerPtr w)
				{
					Ptr modifier = std::shared_ptr<StartAngularVelocity>(new StartAngularVelocity(w));

					return modifier;
				};

				virtual
				void
				initialize(ParticleData& 	particle,
						   float			time) const;

				virtual
				unsigned int
				getNeededComponents() const;
			
			protected:
				StartAngularVelocity(SamplerPtr w);
			};
		}
	}
}