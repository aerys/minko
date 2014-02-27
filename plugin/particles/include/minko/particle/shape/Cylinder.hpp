#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/shape/EmitterShape.hpp"

namespace minko
{
	namespace particle
	{
		namespace shape
		{
			class Cylinder : public EmitterShape
			{
			public:
				typedef std::shared_ptr<Cylinder>	Ptr;
				
			private:
				float _height;
				float _radius;
				float _innerRadius;
			public:
				static
				Ptr
				create(float	height,
					   float	radius,
					   float 	innerRadius = 0)
				{
					Ptr cylinder = std::shared_ptr<Cylinder>(new Cylinder(height,
																		  radius,
																		  innerRadius));

					return cylinder;
				};
				
				inline
				void
				height(float value)
				{
					_height = value;
				};
				
				inline
				void
				radius(float value)
				{
					_radius = value;
				};
				
				inline
				void
				innerRadius(float value)
				{
					_innerRadius = value;
				};

				virtual
				void
				initPosition(ParticleData& particle) const;

			protected:
				Cylinder(float	height,
						 float	radius,
						 float 	innerRadius = 0);
			};
		}
	}
}