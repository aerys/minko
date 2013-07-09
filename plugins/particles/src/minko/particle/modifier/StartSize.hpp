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
			class StartSize : public IParticleInitializer, public Modifier1<float>
			{
			public:
				typedef std::shared_ptr<StartSize>	Ptr;

			public:
				static
				Ptr
				create(SamplerPtr x)
				{
					Ptr modifier = std::shared_ptr<StartSize>(new StartSize(x));

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
				StartSize(SamplerPtr size);
			};
		}
	}
}