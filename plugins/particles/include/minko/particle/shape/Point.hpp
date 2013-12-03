#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/shape/EmitterShape.hpp"

namespace minko
{
	namespace particle
	{
		namespace shape
		{
			class Point : public EmitterShape
			{
			public:
				typedef std::shared_ptr<Point>	Ptr;

			public:
				static
				Ptr
				create()
				{
					Ptr point = std::shared_ptr<Point>(new Point());

					return point;
				};

				virtual
				void
				initPosition(ParticleData& particle) const;

			protected:
				Point();
			};
		}
	}
}