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

#include "minko/render/States.hpp"

#include "minko/render/Priority.hpp"

using namespace minko;
using namespace minko::render;

const std::array<std::string, 17>&
States::propertyNames()
{
    static const std::array<std::string, 17> propertyNames = {
        States::priorityPropertyName(),
        States::zSortedPropertyName(),
        States::blendingSourcePropertyName(),
        States::blendingDestinationPropertyName(),
        States::colorMaskPropertyName(),
        States::depthMaskPropertyName(),
        States::depthFunctionPropertyName(),
        States::triangleCullingPropertyName(),
        States::stencilFunctionPropertyName(),
        States::stencilReferencePropertyName(),
        States::stencilMaskPropertyName(),
        States::stencilFailOperationPropertyName(),
        States::stencilZFailOperationPropertyName(),
        States::stencilZPassOperationPropertyName(),
        States::scissorTestPropertyName(),
        States::scissorBoxPropertyName(),
        States::targetPropertyName(),
    };

    return propertyNames;
}

States::States() :
    _data(data::Provider::create())
{
    resetDefaultValues();
}

States::States(const States& states) :
    _data(data::Provider::create(states._data))
{
}

States::States(std::shared_ptr<data::Provider> data) :
    _data(data)
{
}

void
States::resetDefaultValues()
{
    this->priority(States::priorityDefaultValue());
    this->zSorted(States::zSortedDefaultValue());
    this->blendingSource(States::blendingSourceDefaultValue());
    this->blendingDestination(States::blendingDestinationDefaultValue());
    this->colorMask(States::colorMaskDefaultValue());
    this->depthMask(States::depthMaskDefaultValue());
    this->depthFunction(States::depthFunctionDefaultValue());
    this->triangleCulling(States::triangleCullingDefaultValue());
    this->stencilFunction(States::stencilFunctionDefaultValue());
    this->stencilReference(States::stencilReferenceDefaultValue());
    this->stencilMask(States::stencilMaskDefaultValue());
    this->stencilFailOperation(States::stencilFailOperationDefaultValue());
    this->stencilZFailOperation(States::stencilZFailOperationDefaultValue());
    this->stencilZPassOperation(States::stencilZPassOperationDefaultValue());
    this->scissorTest(States::scissorTestDefaultValue());
    this->scissorBox(States::scissorBoxDefaultValue());
    this->target(States::targetDefaultValue());
}
