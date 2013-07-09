#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/shape/EmitterShape.hpp"

namespace minko
{
	namespace particle
	{
		namespace shape
		{
			class Box : public EmitterShape
			{
			public:
				typedef std::shared_ptr<Box>	Ptr;
				
			private:
				float _width;
				float _height;
				float _length;
				float _limitToSides;

			public:
				static
				Ptr
				create(float	width,
					   float 	height,
					   float	length,
					   bool 	limitToSides)
				{
					Ptr box = std::shared_ptr<Box>(new Box(width,
														   height,
														   length,
														   limitToSides));

					return box;
				};

				inline
				void
				width(float value)
				{
					_width = value;
				};
				
				inline
				void
				height(float value)
				{
					_height = value;
				};
				
				inline
				void
				length(float value)
				{
					_length = value;
				};
				
				inline
				void
				limitToSides(float value)
				{
					_limitToSides = value;
				};

				virtual
				void
				initPosition(ParticleData& particle) const;

			protected:
				Box(float	width,
					float 	height,
					float	length,
					bool 	limitToSides);
			};
		}
	}
}