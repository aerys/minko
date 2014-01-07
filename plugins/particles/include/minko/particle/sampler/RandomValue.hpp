#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/random.hpp"

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
				T value(float time) const
				{
					return _min + _delta * rand01();
				};

				virtual
				void
				set(T& value, float time) const
				{
					value = _min + _delta * rand01();
				};

				virtual
				T
				max() const
				{
					return _min + _delta;
				};

				virtual
				T
				min() const
				{
					return _min;
				};

			protected:
				RandomValue(T min, T max)
					: _min (min),
					_delta(max - min)
				{}
;
			};
		}
	}
}