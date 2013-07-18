#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/tools/random.hpp"

namespace minko
{
	namespace particle
	{
		namespace shape
		{
			class EmitterShape
			{
			public:
				typedef std::shared_ptr<EmitterShape>	Ptr;

			public:
				virtual
				void
				initPositionAndDirection(ParticleData& particle) const;
				
				virtual
				void
				initPosition(ParticleData& particle) const = 0;

			private:
				virtual
				void
				initDirection(ParticleData& particle) const;
			};
		}
	}
}