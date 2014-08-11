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
	Material("BasicMaterial")
{
}

/*virtual*/
void
BasicMaterial::initialize()
{
	diffuseColor(0xffffffff);
	data()->set("uvScale",	math::vec2(1.f));
	data()->set("uvOffset",	math::vec2(0.f));
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
	assert(texture->type() == TextureType::Texture2D);
#endif

	data()->set("diffuseMap", texture->id());

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
BasicMaterial::diffuseCubeMap(std::shared_ptr<render::AbstractTexture> texture)
{
	assert(texture->type() == TextureType::CubeTexture);

	data()->set("diffuseCubeMap", texture->id());

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

render::ResourceId
BasicMaterial::diffuseCubeMap() const
{
	return data()->hasProperty("diffuseCubeMap")
		? data()->get<render::ResourceId>("diffuseCubeMap")
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
BasicMaterial::fogDensity(float value)
{
    data()->set("fogDensity", value);

    return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

float
BasicMaterial::fogDensity() const
{
    return data()->get<float>("fogDensity");
}

BasicMaterial::Ptr
BasicMaterial::fogStart(float value)
{
    data()->set("fogStart", value);

    return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

float
BasicMaterial::fogStart() const
{
    return data()->get<float>("fogStart");
}

BasicMaterial::Ptr
BasicMaterial::fogEnd(float value)
{
    data()->set("fogEnd", value);

    return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

float
BasicMaterial::fogEnd() const
{
    return data()->get<float>("fogEnd");
}

BasicMaterial::Ptr
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

    return std::static_pointer_cast<BasicMaterial>(shared_from_this());
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

BasicMaterial::Ptr
BasicMaterial::blendingMode(Blending::Source src, Blending::Destination dst)
{
	data()->set<Blending::Mode>("blendMode", src | dst);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

BasicMaterial::Ptr
BasicMaterial::blendingMode(Blending::Mode value)
{
	data()->set("blendMode", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
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

BasicMaterial::Ptr
BasicMaterial::colorMask(bool value)
{
	data()->set("colorMask", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

bool
BasicMaterial::colorMask() const
{
	return data()->hasProperty("colorMask")
		? data()->get<bool>("colorMask")
		: _defaultStates->colorMask();
}

BasicMaterial::Ptr
BasicMaterial::depthMask(bool value)
{
	data()->set("depthMask", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

bool
BasicMaterial::depthMask() const
{
	return data()->hasProperty("depthMask")
		? data()->get<bool>("depthmask")
		: _defaultStates->depthMask();
}

BasicMaterial::Ptr
BasicMaterial::depthFunction(CompareMode value)
{
	data()->set("depthFunc", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

CompareMode
BasicMaterial::depthFunction() const
{
	return data()->hasProperty("depthFunc")
		? data()->get<CompareMode>("depthFunc")
		: _defaultStates->depthFunc();
}

BasicMaterial::Ptr
BasicMaterial::triangleCulling(TriangleCulling value)
{
	data()->set("triangleCulling", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

TriangleCulling
BasicMaterial::triangleCulling() const
{
	return data()->hasProperty("triangleCulling")
		? data()->get<TriangleCulling>("triangleCulling")
		: _defaultStates->triangleCulling();
}

BasicMaterial::Ptr
BasicMaterial::stencilFunction(CompareMode value)
{
	data()->set("stencilFunc", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

CompareMode
BasicMaterial::stencilFunction() const
{
	return data()->hasProperty("stencilFunc")
		? data()->get<CompareMode>("stencilFunc")
		: _defaultStates->stencilFunction();
}

BasicMaterial::Ptr
BasicMaterial::stencilReference(int value)
{
	data()->set("stencilRef", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

int
BasicMaterial::stencilReference() const
{
	return data()->hasProperty("stencilRef")
		? data()->get<int>("stencilRef")
		: _defaultStates->stencilReference();
}

BasicMaterial::Ptr
BasicMaterial::stencilMask(uint value)
{
	data()->set("stencilMask", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

uint
BasicMaterial::stencilMask() const
{
	return data()->hasProperty("stencilMask")
		? data()->get<uint>("stencilMask")
		: _defaultStates->stencilMask();
}

BasicMaterial::Ptr
BasicMaterial::stencilFailOperation(StencilOperation value)
{
	data()->set("stencilFailOp", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

StencilOperation
BasicMaterial::stencilFailOperation() const
{
	return data()->hasProperty("stencilFailOp")
		? data()->get<StencilOperation>("stencilFailOp")
		: _defaultStates->stencilFailOperation();
}

BasicMaterial::Ptr
BasicMaterial::stencilDepthFailOperation(StencilOperation value)
{
	data()->set("stencilZFailOp", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

StencilOperation
BasicMaterial::stencilDepthFailOperation() const
{
	return data()->hasProperty("stencilZFailOp")
		? data()->get<StencilOperation>("stencilZFailOp")
		: _defaultStates->stencilDepthFailOperation();
}

BasicMaterial::Ptr
BasicMaterial::stencilDepthPassOperation(StencilOperation value)
{
	data()->set("stencilZPassOp", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

StencilOperation
BasicMaterial::stencilDepthPassOperation() const
{
	return data()->hasProperty("stencilZPassOp")
		? data()->get<StencilOperation>("stencilZPassOp")
		: _defaultStates->stencilDepthPassOperation();
}

BasicMaterial::Ptr
BasicMaterial::priority(float value)
{
	data()->set("priority", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

float 
BasicMaterial::priority() const
{
	return data()->hasProperty("priority")
		? data()->get<float>("priority")
		: _defaultStates->priority();
}

BasicMaterial::Ptr
BasicMaterial::zSorted(bool value)
{
	data()->set("zSort", value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

bool
BasicMaterial::zSorted() const
{
	return data()->hasProperty("zSort")
		? data()->get<bool>("zSort")
		: _defaultStates->zSorted();
}
