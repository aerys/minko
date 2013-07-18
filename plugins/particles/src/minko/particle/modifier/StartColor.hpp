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
			class StartColor : public IParticleInitializer, public Modifier1<minko::math::Vector3>
			{
			public:
				typedef std::shared_ptr<StartColor>	Ptr;

			public:
				static
				Ptr
				create(SamplerPtr x)
				{
					Ptr modifier = std::shared_ptr<StartColor>(new StartColor(x));

					return modifier;
				};

				virtual
				void
				initialize(ParticleData& 	particle,
						  float				time) const;

				virtual
				unsigned int
				getNeededComponents() const;

			protected:
				StartColor(SamplerPtr color);
			};
		}
	}
}