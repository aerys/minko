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

#include "minko/particle/modifier/StartSprite.hpp"
#include "minko/data/ParticlesProvider.hpp"
#include "minko/render/Texture.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::modifier;


StartSprite::StartSprite(SamplerPtr     spriteIndex,
                         unsigned int   numCols,
                         unsigned int   numRows):
    IParticleInitializer(),
    Modifier1<float> (spriteIndex),
    _numCols(numCols),
    _numRows(numRows)
{

}

void
StartSprite::initialize(ParticleData&     particle,
                        float            time) const
{
    particle.spriteIndex = _x->value();
}

unsigned int
StartSprite::getNeededComponents() const
{
    return VertexComponentFlags::SPRITE_INDEX;
}


void
StartSprite::setProperties(data::ParticlesProvider::Ptr provider) const
{
    provider->spritesheetSize(_numRows, _numCols);
}

void
StartSprite::unsetProperties(data::ParticlesProvider::Ptr provider) const
{
    provider->unsetSpritesheetSize();
}