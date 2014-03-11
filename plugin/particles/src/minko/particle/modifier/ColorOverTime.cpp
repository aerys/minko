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

#include "minko/data/ParticlesProvider.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/particle/modifier/ColorOverTime.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/LinearlyInterpolatedValue.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::modifier;
using namespace minko::particle::sampler;

/*static*/ const std::string ColorOverTime::PROPERTY_NAMES[2] = 
{
    std::string("particles.colorOverTimeStart"),
    std::string("particles.colorOverTimeEnd")
};

ColorOverTime::ColorOverTime(LinearlyInterpolatedValue<math::Vector3>::Ptr color):
    IParticleUpdater(),
    Modifier1<math::Vector3>(color)
{
    if (_x == nullptr)
        throw new std::invalid_argument("size");
}

void
ColorOverTime::update(std::vector<ParticleData>&, float timeStep) const
{

}

unsigned int
ColorOverTime::getNeededComponents() const
{
	return VertexComponentFlags::TIME;
}

void
ColorOverTime::setProperties(data::ParticlesProvider::Ptr provider) const
{
    if (provider == nullptr)
        return;

    auto linearSampler = std::dynamic_pointer_cast<LinearlyInterpolatedValue<math::Vector3>>(_x);
    assert(linearSampler);

    provider->set<math::Vector4::Ptr>(PROPERTY_NAMES[0], math::Vector4::create(
        linearSampler->startValue().x(),
        linearSampler->startValue().y(),
        linearSampler->startValue().z(),
        linearSampler->startTime()
    ));
    provider->set<math::Vector4::Ptr>(PROPERTY_NAMES[1], math::Vector4::create(
        linearSampler->endValue().x(),
        linearSampler->endValue().y(),
        linearSampler->endValue().z(),
        linearSampler->endTime()
    ));   
}

void
ColorOverTime::unsetProperties(data::ParticlesProvider::Ptr provider) const
{
    if (provider && provider->hasProperty(PROPERTY_NAMES[0]))
    {
        provider->unset(PROPERTY_NAMES[0]);
        provider->unset(PROPERTY_NAMES[1]);
    }
}