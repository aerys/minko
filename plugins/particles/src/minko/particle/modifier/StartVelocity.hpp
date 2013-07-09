#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/modifier/Modifier3.hpp"
#include "minko/particle/modifier/IParticleInitializer.hpp"

namespace minko
{
	namespace particle
	{
		namespace modifier
		{
			class StartVelocity: public IParticleInitializer, public Modifier3<float>
			{
			public:
				typedef std::shared_ptr<StartVelocity>	Ptr;

			public:
				static
				Ptr
				create(SamplerPtr vx,
					   SamplerPtr vy,
					   SamplerPtr vz)
				{
					Ptr modifier = std::shared_ptr<StartVelocity>(new StartVelocity(vx, vy, vz));

					return modifier;
				};

				virtual void initialize(ParticleData& 	particle,
										float			time) const;

				virtual unsigned int getNeededComponents() const;
			
			protected:
				StartVelocity(SamplerPtr vx,
							  SamplerPtr vy,
							  SamplerPtr vz);
			};
		}
	}
}