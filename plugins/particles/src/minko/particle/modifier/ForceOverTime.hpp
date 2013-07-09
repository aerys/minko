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
			class ForceOverTime : public IParticleUpdater, public Modifier3<float>
			{
			public:
				typedef std::shared_ptr<ForceOverTime>	Ptr;

			public:
				static
				Ptr
				create(SamplerPtr fx,
					   SamplerPtr fy,
					   SamplerPtr fz)
				{
					Ptr modifier = std::shared_ptr<ForceOverTime>(new ForceOverTime(fx, fy, fz));

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
				ForceOverTime(SamplerPtr fx,
							  SamplerPtr fy,
							  SamplerPtr fz);
			};
		}
	}
}