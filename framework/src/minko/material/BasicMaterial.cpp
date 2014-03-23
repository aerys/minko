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

/*static*/ const States::Ptr BasicMaterial::_defaultStates;

BasicMaterial::BasicMaterial():
	Material()
{
}

/*virtual*/
void
BasicMaterial::initialize()
{
	diffuseColor(0xffffffff);
}

BasicMaterial::Ptr
BasicMaterial::diffuseColor(const math::vec4& value)
{
	set("diffuseColor", value);

	return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
}

BasicMaterial::Ptr
BasicMaterial::diffuseColor(uint diffuseRGBA)
{
	return diffuseColor(math::rgba(diffuseRGBA));
}

math::vec4
BasicMaterial::diffuseColor() const
{
	return get<math::vec4>("diffuseColor");
}

BasicMaterial::Ptr
BasicMaterial::diffuseMap(render::AbstractTexture::Ptr texture)
{
#ifdef DEBUG
	assert(texture->type() == TextureType::Texture2D);
#endif

	set("diffuseMap", texture);

	return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
}

Texture::Ptr
BasicMaterial::diffuseMap() const
{
	return hasProperty("diffuseMap")
		? std::dynamic_pointer_cast<Texture>(get<AbstractTexture::Ptr>("diffuseMap"))
		: nullptr;
}

BasicMaterial::Ptr
BasicMaterial::diffuseCubeMap(std::shared_ptr<render::AbstractTexture> texture)
{
	assert(texture->type() == TextureType::CubeTexture);

	set("diffuseCubeMap", texture);

	return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
}

CubeTexture::Ptr
BasicMaterial::diffuseCubeMap() const
{
	return hasProperty("diffuseCubeMap")
		? std::dynamic_pointer_cast<CubeTexture>(get<AbstractTexture::Ptr>("diffuseCubeMap"))
		: nullptr;
}

BasicMaterial::Ptr
BasicMaterial::fogColor(const math::vec4& value)
{
	set("fogColor", value);

	return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
}

BasicMaterial::Ptr
BasicMaterial::fogColor(uint fogRGBA)
{
	return fogColor(math::rgba(fogRGBA));
}

math::vec4
BasicMaterial::fogColor() const
{
	return get<math::vec4>("fogColor");
}

BasicMaterial::Ptr
BasicMaterial::fogDensity(float value)
{
    set("fogDensity", value);

    return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
}

float
BasicMaterial::fogDensity() const
{
    return get<float>("fogDensity");
}

BasicMaterial::Ptr
BasicMaterial::fogStart(float value)
{
    set("fogStart", value);

    return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
}

float
BasicMaterial::fogStart() const
{
    return get<float>("fogStart");
}

BasicMaterial::Ptr
BasicMaterial::fogEnd(float value)
{
    set("fogEnd", value);

    return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
}

float
BasicMaterial::fogEnd() const
{
    return get<float>("fogEnd");
}

BasicMaterial::Ptr
BasicMaterial::fogType(render::FogType value)
{
    unset("fogLinear");
    unset("fogExponential");
    unset("fogExponential2");

    switch (value)
    {
    case render::FogType::Linear:
        set("fogLinear", true);
        break;
    case render::FogType::Exponential:
        set("fogExponential", true);
        break;
    case render::FogType::Exponential2:
        set("fogExponential2", true);
        break;
    default:
        break;
    }

    return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
}

render::FogType
BasicMaterial::fogType() const
{
    if (hasProperty("fogLinear"))
        return render::FogType::Linear;

    if (hasProperty("fogExponential"))
        return render::FogType::Exponential;

    if (hasProperty("fogExponential2"))
        return render::FogType::Exponential2;

    return render::FogType::None;
}

BasicMaterial::Ptr
BasicMaterial::blendingMode(Blending::Source src, Blending::Destination dst)
{
	set<Blending::Mode>("blendMode", src | dst);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

BasicMaterial::Ptr
BasicMaterial::blendingMode(Blending::Mode value)
{
	set("blendMode", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

Blending::Source
BasicMaterial::blendingSourceFactor() const
{
	return hasProperty("blendMode") 
		? Blending::Source(uint(get<Blending::Mode>("blendMode")) & 0x00ff)
		: _defaultStates->blendingSourceFactor();
}

Blending::Destination
BasicMaterial::blendingDestinationFactor() const
{
	return hasProperty("blendMode") 
		? Blending::Destination(uint(get<Blending::Mode>("blendMode")) & 0xff00)
		: _defaultStates->blendingDestinationFactor();
}

BasicMaterial::Ptr
BasicMaterial::colorMask(bool value)
{
	set("colorMask", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

bool
BasicMaterial::colorMask() const
{
	return hasProperty("colorMask")
		? get<bool>("colorMask")
		: _defaultStates->colorMask();
}

BasicMaterial::Ptr
BasicMaterial::depthMask(bool value)
{
	set("depthMask", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

bool
BasicMaterial::depthMask() const
{
	return hasProperty("depthMask")
		? get<bool>("depthmask")
		: _defaultStates->depthMask();
}

BasicMaterial::Ptr
BasicMaterial::depthFunction(CompareMode value)
{
	set("depthFunc", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

CompareMode
BasicMaterial::depthFunction() const
{
	return hasProperty("depthFunc")
		? get<CompareMode>("depthFunc")
		: _defaultStates->depthFunc();
}

BasicMaterial::Ptr
BasicMaterial::triangleCulling(TriangleCulling value)
{
	set("triangleCulling", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

TriangleCulling
BasicMaterial::triangleCulling() const
{
	return hasProperty("triangleCulling")
		? get<TriangleCulling>("triangleCulling")
		: _defaultStates->triangleCulling();
}

BasicMaterial::Ptr
BasicMaterial::stencilFunction(CompareMode value)
{
	set("stencilFunc", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

CompareMode
BasicMaterial::stencilFunction() const
{
	return hasProperty("stencilFunc")
		? get<CompareMode>("stencilFunc")
		: _defaultStates->stencilFunction();
}

BasicMaterial::Ptr
BasicMaterial::stencilReference(int value)
{
	set("stencilRef", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

int
BasicMaterial::stencilReference() const
{
	return hasProperty("stencilRef")
		? get<int>("stencilRef")
		: _defaultStates->stencilReference();
}

BasicMaterial::Ptr
BasicMaterial::stencilMask(uint value)
{
	set("stencilMask", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

uint
BasicMaterial::stencilMask() const
{
	return hasProperty("stencilMask")
		? get<uint>("stencilMask")
		: _defaultStates->stencilMask();
}

BasicMaterial::Ptr
BasicMaterial::stencilFailOperation(StencilOperation value)
{
	set("stencilFailOp", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

StencilOperation
BasicMaterial::stencilFailOperation() const
{
	return hasProperty("stencilFailOp")
		? get<StencilOperation>("stencilFailOp")
		: _defaultStates->stencilFailOperation();
}

BasicMaterial::Ptr
BasicMaterial::stencilDepthFailOperation(StencilOperation value)
{
	set("stencilZFailOp", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

StencilOperation
BasicMaterial::stencilDepthFailOperation() const
{
	return hasProperty("stencilZFailOp")
		? get<StencilOperation>("stencilZFailOp")
		: _defaultStates->stencilDepthFailOperation();
}

BasicMaterial::Ptr
BasicMaterial::stencilDepthPassOperation(StencilOperation value)
{
	set("stencilZPassOp", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

StencilOperation
BasicMaterial::stencilDepthPassOperation() const
{
	return hasProperty("stencilZPassOp")
		? get<StencilOperation>("stencilZPassOp")
		: _defaultStates->stencilDepthPassOperation();
}

BasicMaterial::Ptr
BasicMaterial::priority(float value)
{
	set("priority", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

float 
BasicMaterial::priority() const
{
	return hasProperty("priority")
		? get<float>("priority")
		: _defaultStates->priority();
}

BasicMaterial::Ptr
BasicMaterial::zSorted(bool value)
{
	set("zSort", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

bool
BasicMaterial::zSorted() const
{
	return hasProperty("zSort")
		? get<bool>("zSort")
		: _defaultStates->zSorted();
}

BasicMaterial::Ptr
BasicMaterial::target(AbstractTexture::Ptr value)
{
	set("target", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

AbstractTexture::Ptr
BasicMaterial::target() const
{
	return hasProperty("target")
		? get<AbstractTexture::Ptr>("zSort")
		: nullptr;
}

BasicMaterial::Ptr
BasicMaterial::isTransparent(bool transparent, bool zSort)
{
	return priority(transparent ? priority::TRANSPARENT : priority::OPAQUE)
		->zSorted(zSort)
		->blendingMode(transparent ? Blending::Mode::ALPHA : Blending::Mode::DEFAULT);
}

