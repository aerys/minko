/*
Copyright (c) 2014 Aerys

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

#include "minko//particle/modifier/StartVelocity.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::modifier;

StartVelocity::StartVelocity(SamplerPtr vx,
                             SamplerPtr vy,
                             SamplerPtr vz):
    IParticleInitializer(),
    Modifier3<float> (vx, vy, vz)
{
}

void
StartVelocity::initialize(ParticleData&     particle,
                            float                time) const
{
    particle.startvx    += _x->value();
    particle.startvy    += _y->value();
    particle.startvz    += _z->value();

    particle.x          += particle.startvx * time;
    particle.y          += particle.startvy * time;
    particle.z          += particle.startvz * time;
}

unsigned int
StartVelocity::getNeededComponents() const
{
    return VertexComponentFlags::DEFAULT;
}