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

BasicMaterial::BasicMaterial(const std::string& name):
    Material(name)
{
    auto defaultStates = render::States();

    data()->copyFrom(defaultStates.data());
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

BasicMaterial::Ptr
BasicMaterial::blendingMode(Blending::Source src, Blending::Destination dst)
{
	data()->set<Blending::Mode>("blendingMode", src | dst);
    data()->set<render::Blending::Source>(render::States::PROPERTY_BLENDING_SOURCE, src);
    data()->set<render::Blending::Destination>(render::States::PROPERTY_BLENDING_DESTINATION, dst);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

BasicMaterial::Ptr
BasicMaterial::blendingMode(Blending::Mode value)
{
    auto srcBlendingMode = static_cast<render::Blending::Source>(static_cast<uint>(value) & 0x00ff);
    auto dstBlendingMode = static_cast<render::Blending::Destination>(static_cast<uint>(value) & 0xff00);

    data()->set<render::Blending::Mode>("blendingMode", value);
    data()->set<render::Blending::Source>(render::States::PROPERTY_BLENDING_SOURCE, srcBlendingMode);
    data()->set<render::Blending::Destination>(render::States::PROPERTY_BLENDING_DESTINATION, dstBlendingMode);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

Blending::Source
BasicMaterial::blendingSourceFactor() const
{
    return data()->get<render::Blending::Source>(render::States::PROPERTY_BLENDING_SOURCE);
}

Blending::Destination
BasicMaterial::blendingDestinationFactor() const
{
    return data()->get<render::Blending::Destination>(render::States::PROPERTY_BLENDING_DESTINATION);
}

BasicMaterial::Ptr
BasicMaterial::colorMask(bool value)
{
    data()->set(States::PROPERTY_COLOR_MASK, value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

bool
BasicMaterial::colorMask() const
{
    return data()->get<bool>(States::PROPERTY_COLOR_MASK);
}

BasicMaterial::Ptr
BasicMaterial::depthMask(bool value)
{
    data()->set(States::PROPERTY_DEPTH_MASK, value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

bool
BasicMaterial::depthMask() const
{
    return data()->get<bool>(States::PROPERTY_DEPTH_MASK);
}

BasicMaterial::Ptr
BasicMaterial::depthFunction(CompareMode value)
{
    data()->set(States::PROPERTY_DEPTH_FUNCTION, value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

CompareMode
BasicMaterial::depthFunction() const
{
    return data()->get<CompareMode>(States::PROPERTY_DEPTH_FUNCTION);
}

BasicMaterial::Ptr
BasicMaterial::triangleCulling(TriangleCulling value)
{
    data()->set(States::PROPERTY_TRIANGLE_CULLING, value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

TriangleCulling
BasicMaterial::triangleCulling() const
{
    return data()->get<TriangleCulling>(States::PROPERTY_TRIANGLE_CULLING);
}

BasicMaterial::Ptr
BasicMaterial::stencilFunction(CompareMode value)
{
	data()->set(States::PROPERTY_STENCIL_FUNCTION, value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

CompareMode
BasicMaterial::stencilFunction() const
{
    return data()->get<CompareMode>(States::PROPERTY_STENCIL_FUNCTION);
}

BasicMaterial::Ptr
BasicMaterial::stencilReference(int value)
{
    data()->set(States::PROPERTY_STENCIL_REFERENCE, value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

int
BasicMaterial::stencilReference() const
{
    return data()->get<int>(States::PROPERTY_STENCIL_REFERENCE);
}

BasicMaterial::Ptr
BasicMaterial::stencilMask(uint value)
{
    data()->set(States::PROPERTY_STENCIL_MASK, value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

uint
BasicMaterial::stencilMask() const
{
    return data()->get<uint>(States::PROPERTY_STENCIL_MASK);
}

BasicMaterial::Ptr
BasicMaterial::stencilFailOperation(StencilOperation value)
{
    data()->set(States::PROPERTY_STENCIL_FAIL_OPERATION, value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

StencilOperation
BasicMaterial::stencilFailOperation() const
{
    return data()->get<StencilOperation>(States::PROPERTY_STENCIL_FAIL_OPERATION);
}

BasicMaterial::Ptr
BasicMaterial::stencilZFailOperation(StencilOperation value)
{
    data()->set(States::PROPERTY_STENCIL_ZFAIL_OPERATION, value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

StencilOperation
BasicMaterial::stencilZFailOperation() const
{
    return data()->get<StencilOperation>(States::PROPERTY_STENCIL_ZFAIL_OPERATION);
}

BasicMaterial::Ptr
BasicMaterial::stencilZPassOperation(StencilOperation value)
{
    data()->set(States::PROPERTY_STENCIL_ZPASS_OPERATION, value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

StencilOperation
BasicMaterial::stencilZPassOperation() const
{
    return data()->get<StencilOperation>(States::PROPERTY_STENCIL_ZPASS_OPERATION);
}

BasicMaterial::Ptr
BasicMaterial::priority(float value)
{
    data()->set(States::PROPERTY_PRIORITY, value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}


float
BasicMaterial::priority() const
{
    return data()->get<float>(States::PROPERTY_PRIORITY);
}

BasicMaterial::Ptr
BasicMaterial::zSorted(bool value)
{
    data()->set(States::PROPERTY_ZSORTED, value);

	return std::static_pointer_cast<BasicMaterial>(shared_from_this());
}

bool
BasicMaterial::zSorted() const
{
    return data()->get<bool>(States::PROPERTY_ZSORTED);
}
