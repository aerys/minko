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
			class VelocityOverTime : public IParticleUpdater, public Modifier3<float>
			{
			public:
				typedef std::shared_ptr<VelocityOverTime>	Ptr;

			public:
				static
				Ptr
				create(SamplerPtr vx,
					   SamplerPtr vy,
					   SamplerPtr vz)
				{
					Ptr modifier = std::shared_ptr<VelocityOverTime>(new VelocityOverTime(vx, vy, vz));

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
				VelocityOverTime(SamplerPtr vx,
								 SamplerPtr vy,
								 SamplerPtr vz);
			};
		}
	}
}