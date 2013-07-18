#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/shape/EmitterShape.hpp"

namespace minko
{
	namespace particle
	{
		namespace shape
		{
			class Sphere : public EmitterShape
			{
			public:
				typedef std::shared_ptr<Sphere>	Ptr;
				
			private:
				float _radius;
				float _innerRadius;

			public:
				static
				Ptr
				create(float	radius,
					  float 	innerRadius = 0)
				{
					Ptr sphere	= std::shared_ptr<Sphere> (new Sphere (radius, innerRadius));

					return sphere;
				};

				inline
				void
				setRadius(float value)
				{
					_radius = value;
				};
				
				inline
				void
				setInnerRadius(float value)
				{
					_innerRadius = value;
				};

				virtual
				void
				initPosition(ParticleData& particle) const;

			protected:
				Sphere(float	radius,
	  				   float 	innerRadius = 0);
			};
		}
	}
}