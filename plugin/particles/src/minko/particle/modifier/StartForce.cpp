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

#include "minko/particle/modifier/StartForce.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::sampler;
using namespace minko::particle::modifier;

StartForce::StartForce(Sampler<float>::Ptr fx,
                       Sampler<float>::Ptr fy,
                       Sampler<float>::Ptr fz):
    IParticleInitializer(),
    Modifier3<float> (fx, fy, fz)
{
}

void
StartForce::initialize(ParticleData&    particle,
                       float            time) const
{
    particle.startfx    = _x->value();
    particle.startfy    = _y->value();
    particle.startfz    = _z->value();

    const float tt      = time * time;

    particle.x          += particle.startfx * tt;
    particle.y          += particle.startfy * tt;
    particle.z          += particle.startfz * tt;
}

unsigned int
StartForce::getNeededComponents() const
{
    return VertexComponentFlags::DEFAULT;
}