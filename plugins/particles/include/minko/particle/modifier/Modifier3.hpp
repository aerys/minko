#pragma once

#include "minko/ParticlesCommon.hpp"

namespace minko
{
	namespace particle
	{
		namespace modifier
		{
			template <class T>
			class Modifier3
			{
			public:
				typedef std::shared_ptr<Modifier3>	 Ptr;
				typedef std::shared_ptr<sampler::Sampler<T> > SamplerPtr;

			protected:
				SamplerPtr _x;
				SamplerPtr _y;
				SamplerPtr _z;

			public:
				inline
				SamplerPtr
				x() const
				{
					return _x;
				};

				inline
				SamplerPtr
				y() const 
				{
					return _y;
				};

				inline
				SamplerPtr 
				z() const 
				{
					return _z;
				};

				inline
				void
				x(SamplerPtr value)
				{
					_x = value;
				};

				inline
				void 
				y(SamplerPtr value) 
				{
					_y = value; 
				};

				inline
				void
				z(SamplerPtr value) 
				{
					_z = value; 
				};

			protected:
				Modifier3(SamplerPtr x,
						  SamplerPtr y,
						  SamplerPtr z)
					: _x (x),
					_y (y),
					_z (z)
				{};
			};
		}
	}
}