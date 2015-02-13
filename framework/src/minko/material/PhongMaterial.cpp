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

#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"

using namespace minko;
using namespace minko::material;
using namespace minko::render;

PhongMaterial::PhongMaterial():
	BasicMaterial()
{
}

void
PhongMaterial::initialize()
{
    BasicMaterial::initialize();

    specularColor(0xffffffff);
    // shininess(64.0f);
    // environmentAlpha(1.0f);

    data()->set("environmentMap2dType", int(EnvironmentMap2dType::Unset));
}

PhongMaterial::Ptr
PhongMaterial::specularColor(const math::vec4& color)
{
	data()->set("specularColor", color);

	return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

PhongMaterial::Ptr
PhongMaterial::specularColor(uint color)
{
	return specularColor(math::rgba(color));
}

math::vec4
PhongMaterial::specularColor() const
{
	return data()->get<math::vec4>("specularColor");
}

PhongMaterial::Ptr
PhongMaterial::shininess(float value)
{
	data()->set("shininess", value);

	return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

float
PhongMaterial::shininess() const
{
	return data()->get<float>("shininess");
}

PhongMaterial::Ptr
PhongMaterial::environmentMap(AbstractTexture::Ptr value, EnvironmentMap2dType type)
{
	if (value->type() == TextureType::Texture2D)
	{
		if (type != EnvironmentMap2dType::Probe && type != EnvironmentMap2dType::BlinnNewell)
			throw std::invalid_argument("type");

		data()->set("environmentMap2d", std::static_pointer_cast<AbstractTexture>(value)->id());
		data()->set("environmentMap2dType", int(type));
	}
	else
	{
		data()->set("environmentCubemap", std::static_pointer_cast<AbstractTexture>(value)->id());
		data()->set("environmentMap2dType", int(EnvironmentMap2dType::Unset));
	}

	return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

PhongMaterial::Ptr
PhongMaterial::environmentMapWrapMode(render::WrapMode value)
{
    data()->set("environmentMapWrapMode", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::WrapMode
PhongMaterial::environmentMapWrapMode() const
{
    return (data()->hasProperty("environmentMapWrapMode"))
        ? data()->get<WrapMode>("environmentMapWrapMode")
        : render::SamplerStates::DEFAULT_WRAP_MODE;
}

PhongMaterial::Ptr
PhongMaterial::environmentMapTextureFilter(render::TextureFilter value)
{
    data()->set("environmentMapTextureFilter", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::TextureFilter
PhongMaterial::environmentMapTextureFilter() const
{
    return (data()->hasProperty("environmentMapTextureFilter"))
        ? data()->get<TextureFilter>("environmentMapTextureFilter")
        : render::SamplerStates::DEFAULT_TEXTURE_FILTER;
}

PhongMaterial::Ptr
PhongMaterial::environmentMapMipFilter(render::MipFilter value)
{
    data()->set("environmentMapMipFilter", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::MipFilter
PhongMaterial::environmentMapMipFilter() const
{
    return (data()->hasProperty("environmentMapMipFilter"))
        ? data()->get<MipFilter>("environmentMapMipFilter")
        : render::SamplerStates::DEFAULT_MIP_FILTER;
}

render::ResourceId
PhongMaterial::environmentCubemap() const
{
	return data()->hasProperty("environmentCubemap")
		? data()->get<render::ResourceId>("environmentCubemap")
		: -1;
}

PhongMaterial::Ptr
PhongMaterial::environmentCubemapWrapMode(render::WrapMode value)
{
    data()->set("environmentCubemapWrapMode", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::WrapMode
PhongMaterial::environmentCubemapWrapMode() const
{
    return (data()->hasProperty("environmentCubemapWrapMode"))
        ? data()->get<WrapMode>("environmentCubemapWrapMode")
        : render::SamplerStates::DEFAULT_WRAP_MODE;
}

PhongMaterial::Ptr
PhongMaterial::environmentCubemapTextureFilter(render::TextureFilter value)
{
    data()->set("environmentCubemapTextureFilter", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::TextureFilter
PhongMaterial::environmentCubemapTextureFilter() const
{
    return (data()->hasProperty("environmentCubemapTextureFilter"))
        ? data()->get<TextureFilter>("environmentCubemapTextureFilter")
        : render::SamplerStates::DEFAULT_TEXTURE_FILTER;
}

PhongMaterial::Ptr
PhongMaterial::environmentCubemapMipFilter(render::MipFilter value)
{
    data()->set("environmentCubemapMipFilter", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::MipFilter
PhongMaterial::environmentCubemapMipFilter() const
{
    return (data()->hasProperty("environmentCubemapMipFilter"))
        ? data()->get<MipFilter>("environmentCubemapMipFilter")
        : render::SamplerStates::DEFAULT_MIP_FILTER;
}

render::ResourceId
PhongMaterial::environmentMap2d() const
{
	return data()->hasProperty("environmentMap2d")
		? data()->get<render::ResourceId>("environmentMap2d")
		: -1;
}

PhongMaterial::Ptr
PhongMaterial::environmentMap2dWrapMode(render::WrapMode value)
{
    data()->set("environmentMap2dWrapMode", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::WrapMode
PhongMaterial::environmentMap2dWrapMode() const
{
    return (data()->hasProperty("environmentMap2dWrapMode"))
        ? data()->get<WrapMode>("environmentMap2dWrapMode")
        : render::SamplerStates::DEFAULT_WRAP_MODE;
}

PhongMaterial::Ptr
PhongMaterial::environmentMap2dTextureFilter(render::TextureFilter value)
{
    data()->set("environmentMap2dTextureFilter", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::TextureFilter
PhongMaterial::environmentMap2dTextureFilter() const
{
    return (data()->hasProperty("environmentMap2dTextureFilter"))
        ? data()->get<TextureFilter>("environmentMap2dTextureFilter")
        : render::SamplerStates::DEFAULT_TEXTURE_FILTER;
}

PhongMaterial::Ptr
PhongMaterial::environmentMap2dMipFilter(render::MipFilter value)
{
    data()->set("environmentMap2dMipFilter", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::MipFilter
PhongMaterial::environmentMap2dMipFilter() const
{
    return (data()->hasProperty("environmentMap2dMipFilter"))
        ? data()->get<MipFilter>("environmentMap2dMipFilter")
        : render::SamplerStates::DEFAULT_MIP_FILTER;
}

EnvironmentMap2dType
PhongMaterial::environmentMap2dType() const
{
	return static_cast<EnvironmentMap2dType>(data()->get<int>("environmentMap2dType"));
}

PhongMaterial::Ptr
PhongMaterial::environmentAlpha(float value)
{
	data()->set("environmentAlpha", value);

	return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

float
PhongMaterial::environmentAlpha() const
{
	return data()->get<float>("environmentAlpha");
}

PhongMaterial::Ptr
PhongMaterial::normalMap(AbstractTexture::Ptr value)
{
	if (value->type() == TextureType::CubeTexture)
		throw std::logic_error("Only 2d normal maps are currently supported.");

	data()->set("normalMap", std::static_pointer_cast<AbstractTexture>(value)->id());

	return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::ResourceId
PhongMaterial::normalMap() const
{
	return data()->hasProperty("normalMap")
		? data()->get<render::ResourceId>("normalMap")
		: -1;
}

PhongMaterial::Ptr
PhongMaterial::normalMapWrapMode(render::WrapMode value)
{
    data()->set("normalMapWrapMode", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::WrapMode
PhongMaterial::normalMapWrapMode() const
{
    return (data()->hasProperty("normalMapWrapMode"))
        ? data()->get<WrapMode>("normalMapWrapMode")
        : render::SamplerStates::DEFAULT_WRAP_MODE;
}

PhongMaterial::Ptr
PhongMaterial::normalMapTextureFilter(render::TextureFilter value)
{
    data()->set("normalMapTextureFilter", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::TextureFilter
PhongMaterial::normalMapTextureFilter() const
{
    return (data()->hasProperty("normalMapTextureFilter"))
        ? data()->get<TextureFilter>("normalMapTextureFilter")
        : render::SamplerStates::DEFAULT_TEXTURE_FILTER;
}

PhongMaterial::Ptr
PhongMaterial::normalMapMipFilter(render::MipFilter value)
{
    data()->set("normalMapMipFilter", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::MipFilter
PhongMaterial::normalMapMipFilter() const
{
    return (data()->hasProperty("normalMapMipFilter"))
        ? data()->get<MipFilter>("normalMapMipFilter")
        : render::SamplerStates::DEFAULT_MIP_FILTER;
}

PhongMaterial::Ptr
PhongMaterial::specularMap(AbstractTexture::Ptr value)
{
	if (value->type() == TextureType::CubeTexture)
		throw std::logic_error("Only 2d specular maps are currently supported.");

	data()->set("specularMap", std::static_pointer_cast<AbstractTexture>(value)->id());

	return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::ResourceId
PhongMaterial::specularMap() const
{
	return data()->hasProperty("specularMap")
		? data()->get<render::ResourceId>("specularMap")
		: -1;
}

PhongMaterial::Ptr
PhongMaterial::specularMapWrapMode(render::WrapMode value)
{
    data()->set("specularMapWrapMode", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::WrapMode
PhongMaterial::specularMapWrapMode() const
{
    return (data()->hasProperty("specularMapWrapMode"))
        ? data()->get<WrapMode>("specularMapWrapMode")
        : render::SamplerStates::DEFAULT_WRAP_MODE;
}

PhongMaterial::Ptr
PhongMaterial::specularMapTextureFilter(render::TextureFilter value)
{
    data()->set("specularMapTextureFilter", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::TextureFilter
PhongMaterial::specularMapTextureFilter() const
{
    return (data()->hasProperty("specularMapTextureFilter"))
        ? data()->get<TextureFilter>("specularMapTextureFilter")
        : render::SamplerStates::DEFAULT_TEXTURE_FILTER;
}

PhongMaterial::Ptr
PhongMaterial::specularMapMipFilter(render::MipFilter value)
{
    data()->set("specularMapMipFilter", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::MipFilter
PhongMaterial::specularMapMipFilter() const
{
    return (data()->hasProperty("specularMapMipFilter"))
        ? data()->get<MipFilter>("specularMapMipFilter")
        : render::SamplerStates::DEFAULT_MIP_FILTER;
}

PhongMaterial::Ptr
PhongMaterial::alphaMap(AbstractTexture::Ptr value)
{
	if (value->type() == TextureType::CubeTexture)
		throw std::logic_error("Only 2d transparency maps are currently supported.");

	data()->set("alphaMap", std::static_pointer_cast<AbstractTexture>(value)->id());

	return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::ResourceId
PhongMaterial::alphaMap() const
{
	return data()->hasProperty("alphaMap")
		? data()->get<render::ResourceId>("alphaMap")
		: -1;
}

PhongMaterial::Ptr
PhongMaterial::alphaMapWrapMode(render::WrapMode value)
{
    data()->set("alphaMapWrapMode", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::WrapMode
PhongMaterial::alphaMapWrapMode() const
{
    return (data()->hasProperty("alphaMapWrapMode"))
        ? data()->get<WrapMode>("alphaMapWrapMode")
        : render::SamplerStates::DEFAULT_WRAP_MODE;
}

PhongMaterial::Ptr
PhongMaterial::alphaMapTextureFilter(render::TextureFilter value)
{
    data()->set("alphaMapTextureFilter", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::TextureFilter
PhongMaterial::alphaMapTextureFilter() const
{
    return (data()->hasProperty("alphaMapTextureFilter"))
        ? data()->get<TextureFilter>("alphaMapTextureFilter")
        : render::SamplerStates::DEFAULT_TEXTURE_FILTER;
}

PhongMaterial::Ptr
PhongMaterial::alphaMapMipFilter(render::MipFilter value)
{
    data()->set("alphaMapMipFilter", value);

    return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

render::MipFilter
PhongMaterial::alphaMapMipFilter() const
{
    return (data()->hasProperty("alphaMapMipFilter"))
        ? data()->get<MipFilter>("alphaMapMipFilter")
        : render::SamplerStates::DEFAULT_MIP_FILTER;
}

PhongMaterial::Ptr
PhongMaterial::alphaThreshold(float value)
{
	data()->set("alphaThreshold", value);

	return std::static_pointer_cast<PhongMaterial>(shared_from_this());
}

float
PhongMaterial::alphaThreshold() const
{
	return data()->get<float>("alphaThreshold");
}
