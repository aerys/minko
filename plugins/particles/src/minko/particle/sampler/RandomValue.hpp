#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/sampler/Sampler.hpp"

namespace minko
{
	namespace particle
	{
		namespace sampler
		{
			template <class T>
			class RandomValue : public Sampler<T>
			{
			public:
				typedef std::shared_ptr<RandomValue<T> > Ptr;
				
			private:
				T _min;
				T _delta;

			public:
				static
				Ptr
				create(T min, T max)
				{
					Ptr sampler = std::shared_ptr<RandomValue>(new RandomValue(min, max));

					return sampler;
				};
				
				inline
				void 
				min(T value)
				{
					_min = value;
				};
				
				inline
				void
				max(T value)
				{
					_delta = value - _min;
				};

			public:
				virtual
				T value(float time) const;
				
				virtual
				T*
				set(T* value, float time) const;

				virtual
				T
				max() const;
				
				virtual
				T
				min() const;

			protected:
				RandomValue(T min, T max);
			};
		}
	}
}