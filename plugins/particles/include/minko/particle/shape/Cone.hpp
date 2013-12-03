#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/shape/EmitterShape.hpp"

namespace minko
{
	namespace particle
	{
		namespace shape
		{
			class Cone : public EmitterShape
			{
			public:
				typedef std::shared_ptr<Cone>	Ptr;

			private:
				float _angle;
				float _baseRadius;
				float _length;
				float _innerRadius;

			public:
				static
				Ptr
				create(float	angle,
					   float 	baseRadius,
					   float	length = 0, 
					   float 	innerRadius = 0)
				{
					Ptr cone = std::shared_ptr<Cone>(new Cone(angle,
															  baseRadius,
															  length,
															  innerRadius));

					return cone;
				};
				
				inline
				void
				angle(float value)
				{
					_angle = value;
				};
				
				inline
				void
				baseRadius(float value)
				{
					_baseRadius = value;
				};
				
				inline
				void
				length(float value)
				{
					_length = value;
				};
				
				inline
				void
				innerRadius(float value)
				{
					_innerRadius = value;
				};

				virtual
				void
				initPositionAndDirection(ParticleData& particle) const;
				
				virtual
				void
				initPosition(ParticleData& particle) const;

			private:
				void
				initParticle(ParticleData& particle,
				   		     bool direction) const;
			
			protected:
				Cone(float	angle,
					 float 	baseRadius,
					 float	length = 0, 
					 float 	innerRadius = 0);
			};
		}
	}
}