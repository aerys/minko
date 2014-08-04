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

#include "minko/data/ParticlesProvider.hpp"

#include "minko/math/Vector4.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/Color.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::data;
using namespace minko::render;

ParticlesProvider::ParticlesProvider():
    Provider()
{
}

void
ParticlesProvider::initialize()
{
    set<float>("particles.timeStep", 0.0f);
    diffuseColor(0xffffffff);
}

ParticlesProvider::Ptr
ParticlesProvider::diffuseColor(uint color)
{
    return diffuseColor(Color::uintToVec4(color));
}

ParticlesProvider::Ptr
ParticlesProvider::diffuseColor(Vector4::Ptr color)
{
    set<Vector4::Ptr>("particles.diffuseColor", color);

    return std::static_pointer_cast<ParticlesProvider>(shared_from_this());
}

ParticlesProvider::Ptr
ParticlesProvider::diffuseMap(AbstractTexture::Ptr  texture)
{
    if (texture)
        set<AbstractTexture::Ptr>("particles.spritesheet", texture);

    return std::static_pointer_cast<ParticlesProvider>(shared_from_this());
}

ParticlesProvider::Ptr
ParticlesProvider::unsetDiffuseMap()
{
    if (hasProperty("particles.spritesheet"))
        unset("particles.spritesheet");

    return std::static_pointer_cast<ParticlesProvider>(shared_from_this());
}


ParticlesProvider::Ptr
ParticlesProvider::spritesheetSize(unsigned int numCols,
                                   unsigned int numRows)
{
    set<Vector2::Ptr>("particles.spritesheetSize", Vector2::create(float(numCols), float(numRows)));

    return std::static_pointer_cast<ParticlesProvider>(shared_from_this());
}

ParticlesProvider::Ptr
ParticlesProvider::unsetSpritesheetSize()
{
    if (hasProperty("particles.spritesheetSize"))
    {
        unset("particles.spritesheetSize");
        unset("particles.spritesheet");
    }

    return std::static_pointer_cast<ParticlesProvider>(shared_from_this());
}

ParticlesProvider::Ptr
ParticlesProvider::isInWorldSpace(bool value)
{
    if (value)
        set<bool>("particles.worldspace", true);
    else if (hasProperty("particles.worldspace"))
        unset("particles.worldspace");

    return std::static_pointer_cast<ParticlesProvider>(shared_from_this());
}

bool
ParticlesProvider::isInWorldSpace() const
{
    return hasProperty("particles.worldspace");
}

Vector4::Ptr
ParticlesProvider::diffuseColor() const
{
    return get<Vector4::Ptr>("particles.diffuseColor");
}