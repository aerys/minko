#pragma once

#include "minko/ParticlesCommon.hpp"

namespace minko
{
	namespace particle
	{
		namespace sampler
		{
			template <class T>
			class Sampler
			{
			public:
				typedef std::shared_ptr<Sampler<T> > Ptr;

			public:
				virtual
				T
				value(float time = 0) const	= 0;
				
				virtual
				void
				set(T& target, float time = 0) const	= 0;

				virtual
				T
				max() const = 0;
				
				virtual
				T
				min() const = 0;
			};
		}
	}
}