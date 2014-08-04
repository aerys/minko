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

#include "minko/material/PhongMaterial.hpp"

#include "minko/Color.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"

using namespace minko;
using namespace minko::material;
using namespace minko::render;
using namespace minko::math;

PhongMaterial::PhongMaterial():
    BasicMaterial()
{

}

void
PhongMaterial::initialize()
{
    BasicMaterial::initialize();

    specularColor(0xffffffff);
    shininess(8.0f);
    environmentAlpha(1.0f);
    alphaThreshold(1.0f);

    set("environmentMap2dType", int(EnvironmentMap2dType::Unset));
}

PhongMaterial::Ptr
PhongMaterial::specularColor(Vector4::Ptr color)
{
    set("specularColor", color);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

PhongMaterial::Ptr
PhongMaterial::specularColor(uint color)
{
    return specularColor(Color::uintToVec4(color));
}

Vector4::Ptr
PhongMaterial::specularColor() const
{
    return get<Vector4::Ptr>("specularColor");
}

PhongMaterial::Ptr
PhongMaterial::shininess(float value)
{
    set("shininess", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

float
PhongMaterial::shininess() const
{
    return get<float>("shininess");
}

PhongMaterial::Ptr
PhongMaterial::environmentMap(AbstractTexture::Ptr value,
                              EnvironmentMap2dType type)
{
    if (value->type() == TextureType::Texture2D)
    {
        if (type != EnvironmentMap2dType::Probe && type != EnvironmentMap2dType::BlinnNewell)
            throw std::invalid_argument("type");

        set("environmentMap2d", std::static_pointer_cast<AbstractTexture>(value));
        set("environmentMap2dType", int(type));
    }
    else
    {
        set("environmentCubemap", std::static_pointer_cast<AbstractTexture>(value));
        set("environmentMap2dType", int(EnvironmentMap2dType::Unset));
    }

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

CubeTexture::Ptr
PhongMaterial::environmentCubemap() const
{
    return hasProperty("environmentCubemap")
        ? std::dynamic_pointer_cast<CubeTexture>(get<AbstractTexture::Ptr>("environmentCubemap"))
        : nullptr;
}

Texture::Ptr
PhongMaterial::environmentMap2d() const
{
    return hasProperty("environmentMap2d")
        ? std::dynamic_pointer_cast<Texture>(get<AbstractTexture::Ptr>("environmentMap2d"))
        : nullptr;
}

EnvironmentMap2dType
PhongMaterial::environmentMap2dType() const
{
    return static_cast<EnvironmentMap2dType>(get<int>("environmentMap2dType"));
}

PhongMaterial::Ptr
PhongMaterial::environmentAlpha(float value)
{
    set("environmentAlpha", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

float
PhongMaterial::environmentAlpha() const
{
    return get<float>("environmentAlpha");
}

PhongMaterial::Ptr
PhongMaterial::normalMap(AbstractTexture::Ptr value)
{
    if (value->type() == TextureType::CubeTexture)
        throw std::logic_error("Only 2d normal maps are currently supported.");

    set("normalMap", std::static_pointer_cast<AbstractTexture>(value));

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

Texture::Ptr
PhongMaterial::normalMap() const
{
    return hasProperty("normalMap")
        ? std::dynamic_pointer_cast<Texture>(get<AbstractTexture::Ptr>("normalMap"))
        : nullptr;
}

PhongMaterial::Ptr
PhongMaterial::specularMap(AbstractTexture::Ptr value)
{
    if (value->type() == TextureType::CubeTexture)
        throw std::logic_error("Only 2d specular maps are currently supported.");

    set("specularMap", std::static_pointer_cast<AbstractTexture>(value));

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

Texture::Ptr
PhongMaterial::specularMap() const
{
    return hasProperty("specularMap")
        ? std::dynamic_pointer_cast<Texture>(get<AbstractTexture::Ptr>("specularMap"))
        : nullptr;
}

PhongMaterial::Ptr
PhongMaterial::alphaMap(AbstractTexture::Ptr value)
{
    if (value->type() == TextureType::CubeTexture)
        throw std::logic_error("Only 2d transparency maps are currently supported.");

    set("alphaMap", std::static_pointer_cast<AbstractTexture>(value));

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

Texture::Ptr
PhongMaterial::alphaMap() const
{
    return hasProperty("alphaMap")
        ? std::dynamic_pointer_cast<Texture>(get<AbstractTexture::Ptr>("alphaMap"))
        : nullptr;
}

PhongMaterial::Ptr
PhongMaterial::alphaThreshold(float value)
{
    set("alphaThreshold", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

float
PhongMaterial::alphaThreshold() const
{
    return get<float>("alphaThreshold");
}