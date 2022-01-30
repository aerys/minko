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

#include "minko/material/Material.hpp"

using namespace minko::render;
using namespace minko::material;

Material::Material(const std::string& name) :
    _provider(data::Provider::create())
{
    _provider->set("name", name);
    _provider->set("uuid", _provider->uuid());

    render::States defaultStates;

    _provider->copyFrom(defaultStates.data());
    _provider->unset(render::States::targetPropertyName());
}

Material::Ptr
Material::blendingMode(Blending::Source src, Blending::Destination dst)
{
    _provider->set<Blending::Mode>("blendingMode", src | dst);
    _provider->set<Blending::Source>(States::blendingSourcePropertyName(), src);
    _provider->set<Blending::Destination>(States::blendingDestinationPropertyName(), dst);

    return shared_from_this();
}

Material::Ptr
Material::blendingMode(Blending::Mode value)
{
    auto srcBlendingMode = static_cast<Blending::Source>(static_cast<uint>(value) & 0x00ff);
    auto dstBlendingMode = static_cast<Blending::Destination>(static_cast<uint>(value) & 0xff00);

    _provider->set<Blending::Mode>("blendingMode", value);
    _provider->set<Blending::Source>(States::blendingSourcePropertyName(), srcBlendingMode);
    _provider->set<Blending::Destination>(States::blendingDestinationPropertyName(), dstBlendingMode);

    return shared_from_this();
}

Blending::Source
Material::blendingSource() const
{
    return _provider->get<Blending::Source>(States::blendingSourcePropertyName());
}

Blending::Destination
Material::blendingDestination() const
{
    return _provider->get<Blending::Destination>(States::blendingDestinationPropertyName());
}

Material::Ptr
Material::colorMask(bool value)
{
    _provider->set(States::colorMaskPropertyName(), value);

    return shared_from_this();
}

bool
Material::colorMask() const
{
    return _provider->get<bool>(States::colorMaskPropertyName());
}

Material::Ptr
Material::depthMask(bool value)
{
    _provider->set(States::depthMaskPropertyName(), value);

    return shared_from_this();
}

bool
Material::depthMask() const
{
    return _provider->get<bool>(States::depthMaskPropertyName());
}

Material::Ptr
Material::depthFunction(CompareMode value)
{
    _provider->set(States::depthFunctionPropertyName(), value);

    return shared_from_this();
}

CompareMode
Material::depthFunction() const
{
    return _provider->get<CompareMode>(States::depthFunctionPropertyName());
}

Material::Ptr
Material::triangleCulling(TriangleCulling value)
{
    _provider->set(States::triangleCullingPropertyName(), value);

    return shared_from_this();
}

TriangleCulling
Material::triangleCulling() const
{
    return _provider->get<TriangleCulling>(States::triangleCullingPropertyName());
}

Material::Ptr
Material::stencilFunction(CompareMode value)
{
    _provider->set(States::stencilFunctionPropertyName(), value);

    return shared_from_this();
}

CompareMode
Material::stencilFunction() const
{
    return _provider->get<CompareMode>(States::stencilFunctionPropertyName());
}

Material::Ptr
Material::stencilReference(int value)
{
    _provider->set(States::stencilReferencePropertyName(), value);

    return shared_from_this();
}

int
Material::stencilReference() const
{
    return _provider->get<int>(States::stencilReferencePropertyName());
}

Material::Ptr
Material::stencilMask(uint value)
{
    _provider->set(States::stencilMaskPropertyName(), value);

    return shared_from_this();
}

uint
Material::stencilMask() const
{
    return _provider->get<uint>(States::stencilMaskPropertyName());
}

Material::Ptr
Material::stencilFailOperation(StencilOperation value)
{
    _provider->set(States::stencilFailOperationPropertyName(), value);

    return shared_from_this();
}

StencilOperation
Material::stencilFailOperation() const
{
    return _provider->get<StencilOperation>(States::stencilFailOperationPropertyName());
}

Material::Ptr
Material::stencilZFailOperation(StencilOperation value)
{
    _provider->set(States::stencilZFailOperationPropertyName(), value);

    return shared_from_this();
}

StencilOperation
Material::stencilZFailOperation() const
{
    return _provider->get<StencilOperation>(States::stencilZFailOperationPropertyName());
}

Material::Ptr
Material::stencilZPassOperation(StencilOperation value)
{
    _provider->set(States::stencilZPassOperationPropertyName(), value);

    return shared_from_this();
}

StencilOperation
Material::stencilZPassOperation() const
{
    return _provider->get<StencilOperation>(States::stencilZPassOperationPropertyName());
}

Material::Ptr
Material::priority(float value)
{
    _provider->set(States::priorityPropertyName(), value);

    return shared_from_this();
}

float
Material::priority() const
{
    return _provider->get<float>(States::priorityPropertyName());
}

Material::Ptr
Material::zSorted(bool value)
{
    _provider->set(States::zSortedPropertyName(), value);

    return shared_from_this();
}

bool
Material::zSorted() const
{
    return _provider->get<bool>(States::zSortedPropertyName());
}
