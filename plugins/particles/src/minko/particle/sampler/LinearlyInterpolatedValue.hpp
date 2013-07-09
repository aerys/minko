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
			class LinearlyInterpolatedValue : public Sampler<T>
			{
			public:
				typedef std::shared_ptr<LinearlyInterpolatedValue<T> > Ptr;
				
			private:
				T _start;
				T _delta;

			public:
				static
				Ptr
				create(T start, T end)
				{
					Ptr sampler = std::shared_ptr<LinearlyInterpolatedValue>(new LinearlyInterpolatedValue(start, end));

					return sampler;
				};

				void
					start(T value)
				{
					_start = value;
				};

				void
					end(T value)
				{
					_delta = value - _start;
				};

			public:
				virtual
				T
				value(float time) const;

				virtual
				T*
				set(T* target, float time) const;

				virtual
				T
				max() const;

				virtual
				T
				min() const;

			protected:
				LinearlyInterpolatedValue(T start, T end);
			};
		}
	}
}