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
			class StartForce: public IParticleInitializer, public Modifier3<float>
			{
			public:
				typedef std::shared_ptr<StartForce>	Ptr;

			public:
				static
				Ptr
				create(SamplerPtr fx,
					   SamplerPtr fy,
					   SamplerPtr fz)
				{
					Ptr modifier = std::shared_ptr<StartForce>(new StartForce(fx, fy, fz));

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
				StartForce(SamplerPtr fx,
						   SamplerPtr fy,
						   SamplerPtr fz);
			};
		}
	}
}