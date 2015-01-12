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

#include "minko/material/BasicMaterial.hpp"

#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"
#include "minko/render/Priority.hpp"
#include "minko/render/States.hpp"

using namespace minko;
using namespace minko::material;
using namespace minko::render;

/*static*/ const std::shared_ptr<render::States> BasicMaterial::_defaultStates;

BasicMaterial::BasicMaterial():
	Material("BasicMaterial")
{
}

void
BasicMaterial::initialize()
{
	diffuseColor(0xffffffff);
}

BasicMaterial&
BasicMaterial::uvScale(const math::vec2& value)
{
    data()->set("uvScale", value);

	return *this;
}

const math::vec2&
BasicMaterial::uvScale() const
{
    return data()->get<math::vec2>("uvScale");
}

BasicMaterial&
BasicMaterial::uvOffset(const math::vec2& value)
{
    data()->set("uvOffset", value);

	return *this;
}

const math::vec2&
BasicMaterial::uvOffset() const
{
    return data()->get<math::vec2>("uvOffset");
}

BasicMaterial&
BasicMaterial::diffuseColor(const math::vec4& value)
{
	data()->set("diffuseColor", value);

	return *this;
}

BasicMaterial&
BasicMaterial::diffuseColor(uint diffuseRGBA)
{
	return diffuseColor(math::rgba(diffuseRGBA));
}

math::vec4
BasicMaterial::diffuseColor() const
{
	return data()->get<math::vec4>("diffuseColor");
}

BasicMaterial&
BasicMaterial::diffuseMap(TexturePtr texture)
{
#ifdef DEBUG
	assert(texture == nullptr || texture->type() == TextureType::Texture2D);
#endif

    if (texture)
    	data()->set("diffuseMap", texture->sampler());
    else
        data()->unset("diffuseMap");

	return *this;
}

render::ResourceId
BasicMaterial::diffuseMap() const
{
	return data()->hasProperty("diffuseMap")
		? data()->get<render::ResourceId>("diffuseMap")
		: -1;
}

BasicMaterial&
BasicMaterial::diffuseMapWrapMode(WrapMode wrapMode)
{
    data()->set("diffuseMapWrapMode", wrapMode);

    return *this;
}

WrapMode
BasicMaterial::diffuseMapWrapMode() const
{
    return (data()->hasProperty("diffuseMapWrapMode"))
        ? data()->get<WrapMode>("diffuseMapWrapMode")
        : render::SamplerStates::DEFAULT_WRAP_MODE;
}

BasicMaterial&
BasicMaterial::diffuseMapTextureFilter(TextureFilter textureFilter)
{
    data()->set("diffuseMapTextureFilter", textureFilter);

    return *this;
}

TextureFilter
BasicMaterial::diffuseMapTextureFilter() const
{
    return (data()->hasProperty("diffuseMapTextureFilter"))
        ? data()->get<TextureFilter>("diffuseMapTextureFilter")
        : render::SamplerStates::DEFAULT_TEXTURE_FILTER;
}

BasicMaterial&
BasicMaterial::diffuseMapMipFilter(MipFilter mipFilter)
{
    data()->set("diffuseMapMipFilter", mipFilter);

    return *this;
}

MipFilter
BasicMaterial::diffuseMapMipFilter() const
{
    return (data()->hasProperty("diffuseMapMipFilter")) 
        ? data()->get<MipFilter>("diffuseMapMipFilter") 
        : render::SamplerStates::DEFAULT_MIP_FILTER;
}

BasicMaterial&
BasicMaterial::diffuseCubeMap(std::shared_ptr<render::AbstractTexture> texture)
{
	assert(texture->type() == TextureType::CubeTexture);

	data()->set("diffuseCubeMap", texture->id());

	return *this;
}

render::ResourceId
BasicMaterial::diffuseCubeMap() const
{
	return data()->hasProperty("diffuseCubeMap")
		? data()->get<render::ResourceId>("diffuseCubeMap")
		: -1;
}

WrapMode
BasicMaterial::diffuseCubeMapWrapMode() const
{
    return (data()->hasProperty("diffuseCubeMapWrapMode"))
        ? data()->get<WrapMode>("diffuseCubeMapWrapMode")
        : render::SamplerStates::DEFAULT_WRAP_MODE;
}

BasicMaterial&
BasicMaterial::diffuseCubeMapTextureFilter(TextureFilter textureFilter)
{
    data()->set("diffuseCubeMapTextureFilter", textureFilter);

    return *this;
}

TextureFilter
BasicMaterial::diffuseCubeMapTextureFilter() const
{
    return (data()->hasProperty("diffuseCubeMapTextureFilter"))
        ? data()->get<TextureFilter>("diffuseCubeMapTextureFilter")
        : render::SamplerStates::DEFAULT_TEXTURE_FILTER;
}

BasicMaterial&
BasicMaterial::diffuseCubeMapMipFilter(MipFilter mipFilter)
{
    data()->set("diffuseCubeMapMipFilter", mipFilter);

    return *this;
}

MipFilter
BasicMaterial::diffuseCubeMapMipFilter() const
{
    return (data()->hasProperty("diffuseCubeMapMipFilter"))
        ? data()->get<MipFilter>("diffuseCubeMapMipFilter")
        : render::SamplerStates::DEFAULT_MIP_FILTER;
}

BasicMaterial&
BasicMaterial::fogColor(const math::vec4& value)
{
	data()->set("fogColor", value);

	return *this;
}

BasicMaterial&
BasicMaterial::fogColor(uint fogRGBA)
{
	return fogColor(math::rgba(fogRGBA));
}

math::vec4
BasicMaterial::fogColor() const
{
	return data()->get<math::vec4>("fogColor");
}

BasicMaterial&
BasicMaterial::fogDensity(float value)
{
    data()->set("fogDensity", value);

    return *this;
}

float
BasicMaterial::fogDensity() const
{
    return data()->get<float>("fogDensity");
}

BasicMaterial&
BasicMaterial::fogStart(float value)
{
    data()->set("fogStart", value);

    return *this;
}

float
BasicMaterial::fogStart() const
{
    return data()->get<float>("fogStart");
}

BasicMaterial&
BasicMaterial::fogEnd(float value)
{
    data()->set("fogEnd", value);

    return *this;
}

float
BasicMaterial::fogEnd() const
{
    return data()->get<float>("fogEnd");
}

BasicMaterial&
BasicMaterial::fogType(render::FogType value)
{
    data()->unset("fogLinear");
    data()->unset("fogExponential");
    data()->unset("fogExponential2");

    switch (value)
    {
    case render::FogType::Linear:
        data()->set("fogLinear", true);
        break;
    case render::FogType::Exponential:
        data()->set("fogExponential", true);
        break;
    case render::FogType::Exponential2:
        data()->set("fogExponential2", true);
        break;
    default:
        break;
    }

    return *this;
}

render::FogType
BasicMaterial::fogType() const
{
    if (data()->hasProperty("fogLinear"))
        return render::FogType::Linear;

    if (data()->hasProperty("fogExponential"))
        return render::FogType::Exponential;

    if (data()->hasProperty("fogExponential2"))
        return render::FogType::Exponential2;

    return render::FogType::None;
}

BasicMaterial&
BasicMaterial::blendingMode(Blending::Source src, Blending::Destination dst)
{
	data()->set<Blending::Mode>("blendingMode", src | dst);
    data()->set<render::Blending::Source>(render::States::PROPERTY_BLENDING_SOURCE, src);
    data()->set<render::Blending::Destination>(render::States::PROPERTY_BLENDING_DESTINATION, dst);

	return *this;
}

BasicMaterial&
BasicMaterial::blendingMode(Blending::Mode value)
{
    auto srcBlendingMode = static_cast<render::Blending::Source>(static_cast<uint>(value) & 0x00ff);
    auto dstBlendingMode = static_cast<render::Blending::Destination>(static_cast<uint>(value) & 0xff00);

    data()->set<render::Blending::Mode>("blendingMode", value);
    data()->set<render::Blending::Source>(render::States::PROPERTY_BLENDING_SOURCE, srcBlendingMode);
    data()->set<render::Blending::Destination>(render::States::PROPERTY_BLENDING_DESTINATION, dstBlendingMode);

	return *this;
}


Blending::Source
BasicMaterial::blendingSourceFactor() const
{
	return data()->hasProperty("blendMode") 
		? Blending::Source(uint(data()->get<Blending::Mode>("blendMode")) & 0x00ff)
		: _defaultStates->blendingSourceFactor();
}

Blending::Destination
BasicMaterial::blendingDestinationFactor() const
{
	return data()->hasProperty("blendMode") 
		? Blending::Destination(uint(data()->get<Blending::Mode>("blendMode")) & 0xff00)
		: _defaultStates->blendingDestinationFactor();
}

BasicMaterial&
BasicMaterial::colorMask(bool value)
{
    data()->set(States::PROPERTY_COLOR_MASK, value);

	return *this;
}

bool
BasicMaterial::colorMask() const
{
    return data()->hasProperty(States::PROPERTY_COLOR_MASK)
        ? data()->get<bool>(States::PROPERTY_COLOR_MASK)
		: _defaultStates->colorMask();
}

BasicMaterial&
BasicMaterial::depthMask(bool value)
{
    data()->set(States::PROPERTY_DEPTH_MASK, value);

	return *this;
}

bool
BasicMaterial::depthMask() const
{
    return data()->hasProperty(States::PROPERTY_DEPTH_MASK)
        ? data()->get<bool>(States::PROPERTY_DEPTH_MASK)
		: _defaultStates->depthMask();
}

BasicMaterial&
BasicMaterial::depthFunction(CompareMode value)
{
    data()->set(States::PROPERTY_DEPTH_FUNCTION, value);

	return *this;
}

CompareMode
BasicMaterial::depthFunction() const
{
    return data()->hasProperty(States::PROPERTY_DEPTH_FUNCTION)
        ? data()->get<CompareMode>(States::PROPERTY_DEPTH_FUNCTION)
		: _defaultStates->depthFunction();
}

BasicMaterial&
BasicMaterial::triangleCulling(TriangleCulling value)
{
    data()->set(States::PROPERTY_TRIANGLE_CULLING, value);

	return *this;
}

TriangleCulling
BasicMaterial::triangleCulling() const
{
    return data()->hasProperty(States::PROPERTY_TRIANGLE_CULLING)
        ? data()->get<TriangleCulling>(States::PROPERTY_TRIANGLE_CULLING)
		: _defaultStates->triangleCulling();
}

BasicMaterial&
BasicMaterial::stencilFunction(CompareMode value)
{
	data()->set(States::PROPERTY_STENCIL_FUNCTION, value);

	return *this;
}

CompareMode
BasicMaterial::stencilFunction() const
{
    return data()->hasProperty(States::PROPERTY_STENCIL_FUNCTION)
        ? data()->get<CompareMode>(States::PROPERTY_STENCIL_FUNCTION)
		: _defaultStates->stencilFunction();
}

BasicMaterial&
BasicMaterial::stencilReference(int value)
{
    data()->set(States::PROPERTY_STENCIL_REFERENCE, value);

	return *this;
}

int
BasicMaterial::stencilReference() const
{
    return data()->hasProperty(States::PROPERTY_STENCIL_REFERENCE)
        ? data()->get<int>(States::PROPERTY_STENCIL_REFERENCE)
		: _defaultStates->stencilReference();
}

BasicMaterial&
BasicMaterial::stencilMask(uint value)
{
    data()->set(States::PROPERTY_STENCIL_MASK, value);

	return *this;
}

uint
BasicMaterial::stencilMask() const
{
    return data()->hasProperty(States::PROPERTY_STENCIL_MASK)
        ? data()->get<uint>(States::PROPERTY_STENCIL_MASK)
		: _defaultStates->stencilMask();
}

BasicMaterial&
BasicMaterial::stencilFailOperation(StencilOperation value)
{
    data()->set(States::PROPERTY_STENCIL_FAIL_OP, value);

	return *this;
}

StencilOperation
BasicMaterial::stencilFailOperation() const
{
    return data()->hasProperty(States::PROPERTY_STENCIL_FAIL_OP)
        ? data()->get<StencilOperation>(States::PROPERTY_STENCIL_FAIL_OP)
		: _defaultStates->stencilFailOperation();
}

BasicMaterial&
BasicMaterial::stencilZFailOperation(StencilOperation value)
{
    data()->set(States::PROPERTY_STENCIL_ZFAIL_OP, value);

	return *this;
}

StencilOperation
BasicMaterial::stencilZFailOperation() const
{
    return data()->hasProperty(States::PROPERTY_STENCIL_ZFAIL_OP)
        ? data()->get<StencilOperation>(States::PROPERTY_STENCIL_ZFAIL_OP)
		: _defaultStates->stencilZFailOperation();
}

BasicMaterial&
BasicMaterial::stencilZPassOperation(StencilOperation value)
{
    data()->set(States::PROPERTY_STENCIL_ZPASS_OP, value);

	return *this;
}

StencilOperation
BasicMaterial::stencilZPassOperation() const
{
    return data()->hasProperty(States::PROPERTY_STENCIL_ZPASS_OP)
        ? data()->get<StencilOperation>(States::PROPERTY_STENCIL_ZPASS_OP)
		: _defaultStates->stencilZPassOperation();
}

BasicMaterial&
BasicMaterial::priority(float value)
{
    data()->set(States::PROPERTY_PRIORITY, value);

	return *this;
}


float 
BasicMaterial::priority() const
{
    return data()->hasProperty(States::PROPERTY_PRIORITY)
        ? data()->get<float>(States::PROPERTY_PRIORITY)
		: _defaultStates->priority();
}

BasicMaterial&
BasicMaterial::zSorted(bool value)
{
    data()->set(States::PROPERTY_ZSORTED, value);

	return *this;
}

bool
BasicMaterial::zSorted() const
{
    return data()->hasProperty(States::PROPERTY_ZSORTED)
        ? data()->get<bool>(States::PROPERTY_ZSORTED)
		: _defaultStates->zSorted();
}
