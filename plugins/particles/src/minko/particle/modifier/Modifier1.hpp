#pragma once

#include "minko/ParticlesCommon.hpp"

namespace minko
{
	namespace particle
	{
		namespace modifier
		{
			template <class T>
			class Modifier1
			{
			public:
				typedef std::shared_ptr<Modifier1>	 Ptr;
				typedef std::shared_ptr<sampler::Sampler<T> > SamplerPtr;
				
			protected:
				SamplerPtr _x;

			public:
				inline
				SamplerPtr
				x() const
				{
					return _x;
				};

				inline
				void
				x(SamplerPtr value)
				{
					_x = value;
				};

			protected:
				Modifier1(SamplerPtr x)
					: _x (x)
				{};
			};
		}
	}
}