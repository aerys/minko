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

#include "minko/particle/modifier/VelocityOverTime.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::modifier;

VelocityOverTime::VelocityOverTime(SamplerPtr vx,
			     				   SamplerPtr vy,
			     				   SamplerPtr vz)
	: Modifier3<float> (vx, vy, vz)
{}

void
VelocityOverTime::update(std::vector<ParticleData>& 	particles,
		 		   		 float							timeStep) const
{
	for (unsigned int particleIndex = 0; particleIndex < particles.size(); ++particleIndex)
	{
		ParticleData& particle = particles[particleIndex];

		if (particle.alive)
		{
			particle.x += _x->value(particle.timeLived / particle.lifetime) * timeStep;
			particle.y += _y->value(particle.timeLived / particle.lifetime) * timeStep;
			particle.z += _z->value(particle.timeLived / particle.lifetime) * timeStep;
		}
	}
}


unsigned int
VelocityOverTime::getNeededComponents() const
{
	return VertexComponentFlags::DEFAULT;
}

