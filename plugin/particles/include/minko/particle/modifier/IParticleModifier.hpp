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

				typedef std::shared_ptr<data::Provider> ProviderPtr;

			public:
				virtual
				unsigned int
				getNeededComponents() const = 0;

				virtual
				void
				setProperties(ProviderPtr provider)
				{};
				
				virtual
				void
				unsetProperties(ProviderPtr provider)
				{};
			};
		}
	}
}