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

#include "minko/render/DrawCallZSorter.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/data/Store.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/data/Provider.hpp"

using namespace minko;
using namespace minko::data;
using namespace minko::render;
using namespace minko::math;

// Names of the properties that may cause a Z-sort change between drawcalls
const DrawCallZSorter::PropertyInfos DrawCallZSorter::_rawProperties = initializeRawProperties();

DrawCallZSorter::PropertyInfos
DrawCallZSorter::initializeRawProperties()
{
    PropertyInfos props;

    props["material[${materialUuid}].priority"] = data::Binding::Source::TARGET;
    props["material[${materialUuid}].zSorted"] = data::Binding::Source::TARGET;
    props["geometry[${geometryUuid}].position"] = data::Binding::Source::TARGET;
    props["modelToWorldMatrix"] = data::Binding::Source::TARGET;
    props["worldToScreenMatrix"] = data::Binding::Source::RENDERER;

    return props;
}

DrawCallZSorter::DrawCallZSorter(DrawCall* drawcall):
    _drawcall(drawcall),
    _targetPropAddedSlot(nullptr),
    _targetPropRemovedSlot(nullptr),
    _rendererPropAddedSlot(nullptr),
    _rendererPropRemovedSlot(nullptr),
    _centerPosition(),
    _modelToWorldMatrix(nullptr),
    _worldToScreenMatrix(nullptr)
{
    
}

void
DrawCallZSorter::initialize(data::Store& targetData,
                            data::Store& rendererData,
                            data::Store& rootData)
{
    if (targetData.hasProperty("centerPosition"))
        _centerPosition = targetData.get<math::vec3>("centerPosition");

    clear();

    // Signals
    _targetPropAddedSlot = targetData.propertyAdded().connect(
        std::bind(
            &DrawCallZSorter::propertyAddedHandler,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        )
    );

    _targetPropRemovedSlot = targetData.propertyRemoved().connect(
        std::bind(
            &DrawCallZSorter::propertyRemovedHandler,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        )
    );

    _rendererPropAddedSlot = rendererData.propertyAdded().connect(
        std::bind(
            &DrawCallZSorter::propertyAddedHandler,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        )
    );

    _rendererPropRemovedSlot = rendererData.propertyRemoved().connect(
        std::bind(
            &DrawCallZSorter::propertyRemovedHandler,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        )
    );

    _propertyChangedSlots.clear();

    for (auto& prop : _rawProperties)
    {
        auto source = prop.second;
        auto& store = (source == data::Binding::Source::RENDERER) ? rendererData : targetData;

        if (store.hasProperty(prop.first))
            propertyAddedHandler(store, nullptr, prop.first);
    }
}

void
DrawCallZSorter::clear()
{
    _targetPropAddedSlot = nullptr;
    _targetPropRemovedSlot = nullptr;
    _rendererPropAddedSlot = nullptr;
    _rendererPropRemovedSlot = nullptr;
}

void
DrawCallZSorter::propertyAddedHandler(data::Store& store, 
                                      std::shared_ptr<data::Provider> data, 
                                      const std::string& propertyName)
{
    if (!store.hasProperty(propertyName))
        return;

    if (propertyName == "modelToWorldMatrix" && store.hasProperty("modelToWorldMatrix"))
        _modelToWorldMatrix = store.getPointer<math::mat4>("modelToWorldMatrix");
    else if (propertyName == "worldToScreenMatrix" && store.hasProperty("worldToScreenMatrix"))
        _worldToScreenMatrix = store.getPointer<math::mat4>("worldToScreenMatrix");

    if (_propertyChangedSlots.find(propertyName) == _propertyChangedSlots.end())
    {
        _propertyChangedSlots[propertyName] = store.propertyChanged(propertyName).connect(
            std::bind(
                &DrawCallZSorter::requestZSort,
                shared_from_this()
            )
        );
    }

    requestZSort();
}

void
DrawCallZSorter::propertyRemovedHandler(data::Store& store,
                                        std::shared_ptr<data::Provider>,
                                        const std::string& propertyName)
{
    if (!store.hasProperty(propertyName))
        return;

    if (propertyName == "modelToWorldMatrix")
        _modelToWorldMatrix = nullptr;
    else if (propertyName == "worldToScreenMatrix")
        _worldToScreenMatrix = nullptr;

    if (_propertyChangedSlots.find(propertyName) != _propertyChangedSlots.end())
        _propertyChangedSlots.erase(propertyName);

    requestZSort();
}

void
DrawCallZSorter::requestZSort()
{
    if (_drawcall->zSorted())
        _drawcall->zSortNeeded()->execute(_drawcall);
}

math::vec3
DrawCallZSorter::getEyeSpacePosition() const
{
    auto modelView = math::mat4();
    
    if (_modelToWorldMatrix != nullptr)
        modelView = *_modelToWorldMatrix;
    if (_worldToScreenMatrix != nullptr)
        modelView = (*_worldToScreenMatrix) * modelView;

    return math::vec3(modelView * math::vec4(_centerPosition, 1));
}