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
//
//BasicMaterial&
//BasicMaterial::blendingMode(Blending::Source src, Blending::Destination dst)
//{
//	data()->set<Blending::Mode>("blendMode", src | dst);
//
//	return *this;
//}
//
//BasicMaterial&
//BasicMaterial::blendingMode(Blending::Mode value)
//{
//	data()->set("blendMode", value);
//
//	return *this;
//}
//
//Blending::Source
//BasicMaterial::blendingSourceFactor() const
//{
//	return data()->hasProperty("blendMode") 
//		? Blending::Source(uint(data()->get<Blending::Mode>("blendMode")) & 0x00ff)
//		: _defaultStates->blendingSourceFactor();
//}
//
//Blending::Destination
//BasicMaterial::blendingDestinationFactor() const
//{
//	return data()->hasProperty("blendMode") 
//		? Blending::Destination(uint(data()->get<Blending::Mode>("blendMode")) & 0xff00)
//		: _defaultStates->blendingDestinationFactor();
//}
//
//BasicMaterial&
//BasicMaterial::colorMask(bool value)
//{
//	data()->set("colorMask", value);
//
//	return *this;
//}
//
//bool
//BasicMaterial::colorMask() const
//{
//	return data()->hasProperty("colorMask")
//		? data()->get<bool>("colorMask")
//		: _defaultStates->colorMask();
//}
//
//BasicMaterial&
//BasicMaterial::depthMask(bool value)
//{
//	data()->set("depthMask", value);
//
//	return *this;
//}
//
//bool
//BasicMaterial::depthMask() const
//{
//	return data()->hasProperty("depthMask")
//		? data()->get<bool>("depthmask")
//		: _defaultStates->depthMask();
//}
//
//BasicMaterial&
//BasicMaterial::depthFunction(CompareMode value)
//{
//	data()->set("depthFunc", value);
//
//	return *this;
//}
//
//CompareMode
//BasicMaterial::depthFunction() const
//{
//	return data()->hasProperty("depthFunc")
//		? data()->get<CompareMode>("depthFunc")
//		: _defaultStates->depthFunc();
//}
//
//BasicMaterial&
//BasicMaterial::triangleCulling(TriangleCulling value)
//{
//	data()->set("triangleCulling", value);
//
//	return *this;
//}
//
//TriangleCulling
//BasicMaterial::triangleCulling() const
//{
//	return data()->hasProperty("triangleCulling")
//		? data()->get<TriangleCulling>("triangleCulling")
//		: _defaultStates->triangleCulling();
//}
//
//BasicMaterial&
//BasicMaterial::stencilFunction(CompareMode value)
//{
//	data()->set("stencilFunc", value);
//
//	return *this;
//}
//
//CompareMode
//BasicMaterial::stencilFunction() const
//{
//	return data()->hasProperty("stencilFunc")
//		? data()->get<CompareMode>("stencilFunc")
//		: _defaultStates->stencilFunction();
//}
//
//BasicMaterial&
//BasicMaterial::stencilReference(int value)
//{
//	data()->set("stencilRef", value);
//
//	return *this;
//}
//
//int
//BasicMaterial::stencilReference() const
//{
//	return data()->hasProperty("stencilRef")
//		? data()->get<int>("stencilRef")
//		: _defaultStates->stencilReference();
//}
//
//BasicMaterial&
//BasicMaterial::stencilMask(uint value)
//{
//	data()->set("stencilMask", value);
//
//	return *this;
//}
//
//uint
//BasicMaterial::stencilMask() const
//{
//	return data()->hasProperty("stencilMask")
//		? data()->get<uint>("stencilMask")
//		: _defaultStates->stencilMask();
//}
//
//BasicMaterial&
//BasicMaterial::stencilFailOperation(StencilOperation value)
//{
//	data()->set("stencilFailOp", value);
//
//	return *this;
//}
//
//StencilOperation
//BasicMaterial::stencilFailOperation() const
//{
//	return data()->hasProperty("stencilFailOp")
//		? data()->get<StencilOperation>("stencilFailOp")
//		: _defaultStates->stencilFailOperation();
//}
//
//BasicMaterial&
//BasicMaterial::stencilDepthFailOperation(StencilOperation value)
//{
//	data()->set("stencilZFailOp", value);
//
//	return *this;
//}
//
//StencilOperation
//BasicMaterial::stencilDepthFailOperation() const
//{
//	return data()->hasProperty("stencilZFailOp")
//		? data()->get<StencilOperation>("stencilZFailOp")
//		: _defaultStates->stencilDepthFailOperation();
//}
//
//BasicMaterial&
//BasicMaterial::stencilDepthPassOperation(StencilOperation value)
//{
//	data()->set("stencilZPassOp", value);
//
//	return *this;
//}
//
//StencilOperation
//BasicMaterial::stencilDepthPassOperation() const
//{
//	return data()->hasProperty("stencilZPassOp")
//		? data()->get<StencilOperation>("stencilZPassOp")
//		: _defaultStates->stencilDepthPassOperation();
//}
//
//BasicMaterial&
//BasicMaterial::priority(float value)
//{
//	data()->set("priority", value);
//
//	return *this;
//}
//
//float 
//BasicMaterial::priority() const
//{
//	return data()->hasProperty("priority")
//		? data()->get<float>("priority")
//		: _defaultStates->priority();
//}
//
//BasicMaterial&
//BasicMaterial::zSorted(bool value)
//{
//	data()->set("zSort", value);
//
//	return *this;
//}
//
//bool
//BasicMaterial::zSorted() const
//{
//	return data()->hasProperty("zSort")
//		? data()->get<bool>("zSort")
//		: _defaultStates->zSorted();
//}
