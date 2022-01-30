/*
Copyright (c) 2022 Aerys

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

#include "minko/material/BasicMaterial.hpp"

#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"
#include "minko/render/Priority.hpp"
#include "minko/render/States.hpp"

using namespace minko;
using namespace minko::material;
using namespace minko::render;

BasicMaterial::BasicMaterial(const std::string& name):
    Material(name)
{
}

BasicMaterial::Ptr
BasicMaterial::uvScale(const math::vec2& value)
{
    data()->set("uvScale", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

const math::vec2&
BasicMaterial::uvScale() const
{
    return data()->get<math::vec2>("uvScale");
}

BasicMaterial::Ptr
BasicMaterial::uvOffset(const math::vec2& value)
{
    data()->set("uvOffset", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

const math::vec2&
BasicMaterial::uvOffset() const
{
    return data()->get<math::vec2>("uvOffset");
}

BasicMaterial::Ptr
BasicMaterial::diffuseColor(const math::vec4& value)
{
	data()->set("diffuseColor", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

BasicMaterial::Ptr
BasicMaterial::diffuseColor(uint diffuseRGBA)
{
	return diffuseColor(math::rgba(diffuseRGBA));
}

math::vec4
BasicMaterial::diffuseColor() const
{
	return data()->get<math::vec4>("diffuseColor");
}

BasicMaterial::Ptr
BasicMaterial::diffuseMap(TexturePtr texture)
{
#ifdef DEBUG
	assert(texture == nullptr || texture->type() == TextureType::Texture2D);
#endif

    if (texture)
    	data()->set("diffuseMap", texture->sampler());
    else
        data()->unset("diffuseMap");

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

render::ResourceId
BasicMaterial::diffuseMap() const
{
	return data()->hasProperty("diffuseMap")
		? data()->get<render::ResourceId>("diffuseMap")
		: -1;
}

BasicMaterial::Ptr
BasicMaterial::fogColor(const math::vec4& value)
{
	data()->set("fogColor", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

BasicMaterial::Ptr
BasicMaterial::fogColor(uint fogRGBA)
{
	return fogColor(math::rgba(fogRGBA));
}

math::vec4
BasicMaterial::fogColor() const
{
	return data()->get<math::vec4>("fogColor");
}

BasicMaterial::Ptr
BasicMaterial::fogStart(float value)
{
    data()->getUnsafePointer<math::vec2>("fogBounds")->x = value;

    return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

float
BasicMaterial::fogStart() const
{
    return data()->get<math::vec2>("fogBounds").x;
}

BasicMaterial::Ptr
BasicMaterial::fogEnd(float value)
{
    data()->getUnsafePointer<math::vec2>("fogBounds")->y = value;

    return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

float
BasicMaterial::fogEnd() const
{
    return data()->get<math::vec2>("fogBounds").y;
}

BasicMaterial::Ptr
BasicMaterial::fogTechnique(material::FogTechnique value)
{
    data()->set("fogTechnique", value);

    return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

material::FogTechnique
BasicMaterial::fogTechnique() const
{
    return data()->get<material::FogTechnique>("fogTechnique");
}
