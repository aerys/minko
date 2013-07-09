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
			class Constant : public Sampler<T>
			{
			public:
				typedef std::shared_ptr<Constant<T> > Ptr;
				
			private:
				T _value;

			public:
				static
				Ptr
				create(T value)
				{
					Ptr sampler = std::shared_ptr<Constant>(new Constant(value));

					return sampler;
				};
				
				inline
				void
				value(T value)
				{
					_value = value;
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
				Constant(T value);
			};
		}
	}
}
