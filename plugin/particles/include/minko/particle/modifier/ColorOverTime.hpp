/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/particle/modifier/Modifier1.hpp"
#include "minko/particle/modifier/IParticleUpdater.hpp"

namespace minko
{
	namespace particle
	{
		namespace modifier
		{
			class ColorOverTime : 
                public IParticleUpdater,
                public Modifier1<math::Vector3>
			{
			public:
				typedef std::shared_ptr<ColorOverTime>	                                    Ptr;

            private:
                typedef std::shared_ptr<sampler::LinearlyInterpolatedValue<math::Vector3>>  LinearSamplerPtr;
                typedef std::shared_ptr<data::ParticlesProvider>                            ParticlesProviderPtr;

            private:
                static const std::string PROPERTY_NAMES[2];

			public:
				static
				Ptr
				create(LinearSamplerPtr sampler)
				{
					Ptr ptr = std::shared_ptr<ColorOverTime>(new ColorOverTime(sampler));

					return ptr;
				};

				void
				update(std::vector<ParticleData>&, float timeStep) const;

				unsigned int
				getNeededComponents() const;

                void
                setProperties(ParticlesProviderPtr) const;

                void
                unsetProperties(ParticlesProviderPtr) const;

			protected:
				ColorOverTime(LinearSamplerPtr);
			};
		}
	}
}