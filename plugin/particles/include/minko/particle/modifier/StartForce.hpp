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
#include "minko/particle/modifier/Modifier3.hpp"
#include "minko/particle/modifier/IParticleInitializer.hpp"

namespace minko
{
	namespace particle
	{
		namespace modifier
		{
			class StartForce: 
                public IParticleInitializer, 
                public Modifier3<float>
			{
			public:
				typedef std::shared_ptr<StartForce>	Ptr;

			public:
				static
				Ptr
				create(SamplerPtr fx,
					   SamplerPtr fy,
					   SamplerPtr fz)
				{
					Ptr modifier = std::shared_ptr<StartForce>(new StartForce(fx, fy, fz));

					return modifier;
				};

				virtual
				void
				initialize(ParticleData&, float) const;

				virtual
				unsigned int
				getNeededComponents() const;
			
			protected:
				StartForce(SamplerPtr,
						   SamplerPtr,
						   SamplerPtr);
			};
		}
	}
}