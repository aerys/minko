#pragma once

#include "minko/ParticlesCommon.hpp"

namespace minko
{
	namespace particle
	{
		namespace modifier
		{
			class IParticleModifier
			{
			public:
				typedef std::shared_ptr<IParticleModifier> Ptr;

			public:
				virtual
				unsigned int
				getNeededComponents() const = 0;
			};
		}
	}
}