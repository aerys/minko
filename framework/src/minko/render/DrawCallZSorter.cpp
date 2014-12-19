/*,
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

    props["material[${materialUuid}].priority"] = PropertyInfo(data::Binding::Source::TARGET, false);
    props["material[${materialUuid}].zSort"] = PropertyInfo(data::Binding::Source::TARGET, false);
    props["geometry[${geometryUuid}].position"] = PropertyInfo(data::Binding::Source::TARGET, false);
    props["transform.modelToWorldMatrix"] = PropertyInfo(data::Binding::Source::TARGET, true);
    props["camera.worldToScreenMatrix"] = PropertyInfo(data::Binding::Source::RENDERER, true);

    return props;
}

DrawCallZSorter::DrawCallZSorter(DrawCall* drawcall):
    _drawcall(drawcall),
    _properties(),
    _targetPropAddedSlot(nullptr),
    _targetPropRemovedSlot(nullptr),
    _rendererPropAddedSlot(nullptr),
    _rendererPropRemovedSlot(nullptr),
    _vertexPositions(),
    _modelToWorldMatrix(),
    _worldToScreenMatrix()
{
    if (_drawcall == nullptr)
        throw std::invalid_argument("drawcall");
}

void
DrawCallZSorter::initialize(data::Store& targetData,
                            data::Store& rendererData,
                            data::Store& rootData)
{
    clear();
    
    // format raw property name to fit with the current
    for (auto& prop : _rawProperties)
        _properties[data::Store::getActualPropertyName(_drawcall->variables(), prop.first)] = prop.second;

    _vertexPositions.first = data::Store::getActualPropertyName(_drawcall->variables(), "geometry[${geometryUuid}].position");
    _modelToWorldMatrix.first = data::Store::getActualPropertyName(_drawcall->variables(), "transform.modelToWorldMatrix");
    _worldToScreenMatrix.first = data::Store::getActualPropertyName(_drawcall->variables(), "camera.worldToScreenMatrix");
    
    _targetPropAddedSlot = targetData.propertyAdded().connect(std::bind(
        &DrawCallZSorter::propertyAddedHandler,
        shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    _rendererPropAddedSlot = rendererData.propertyAdded().connect(std::bind(
        &DrawCallZSorter::propertyAddedHandler,
        shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    _targetPropRemovedSlot = targetData.propertyRemoved().connect(std::bind(
        &DrawCallZSorter::propertyRemovedHandler,
        shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    _rendererPropRemovedSlot = rendererData.propertyRemoved().connect(std::bind(
        &DrawCallZSorter::propertyRemovedHandler,
        shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    _propChangedSlots.clear();

    for (auto& prop : _properties)
    {
        auto source = prop.second.source;
        auto store = source == data::Binding::Source::RENDERER ? rendererData : targetData;
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
    _propChangedSlots.clear();
    _matrixChangedSlots.clear();
    _properties.clear();
}

void
DrawCallZSorter::propertyAddedHandler(data::Store& store, 
                                      std::shared_ptr<data::Provider>, 
                                      const std::string& propertyName)
{
    const auto foundPropIt = _properties.find(propertyName);

    if (foundPropIt == _properties.end())
        return;

    recordIfPositionalMembers(store, propertyName, true, false);

    if (_propChangedSlots.find(propertyName) == _propChangedSlots.end())
    {
        _propChangedSlots[propertyName] = store.propertyChanged(propertyName).connect(std::bind(
            &DrawCallZSorter::requestZSort,
            shared_from_this()
        ));

        if (store.hasProperty(propertyName) && foundPropIt->second.isMatrix)
        {
            _matrixChangedSlots[propertyName] = store.propertyChanged(propertyName).connect(
                std::bind(
                    &DrawCallZSorter::requestZSort,
                    shared_from_this()
                )
            );
        }
    }

    requestZSort();
}

void
DrawCallZSorter::propertyRemovedHandler(data::Store& store,
                                        std::shared_ptr<data::Provider>,
                                        const std::string& propertyName)
{
    if (_properties.find(propertyName) == _properties.end())
        return;

    recordIfPositionalMembers(store, propertyName, false, true);

    if (_propChangedSlots.find(propertyName) != _propChangedSlots.end())
    {
        _propChangedSlots.erase(propertyName);

        if (_matrixChangedSlots.find(propertyName) != _matrixChangedSlots.end())
            _matrixChangedSlots.erase(propertyName);
    }
 
    requestZSort();
}

void
DrawCallZSorter::requestZSort()
{
    if (_drawcall->zSorted())
        _drawcall->zSortNeeded()->execute(_drawcall); // temporary ugly solution
}

void
DrawCallZSorter::recordIfPositionalMembers(data::Store&         store,
                                           const std::string&   propertyName,
                                           bool                 isPropertyAdded,
                                           bool                 isPropertyRemoved)
{
    if (isPropertyAdded)
    {
        if (propertyName == _vertexPositions.first)
        {
            for (auto test : _drawcall->targetData().collections())
            {
                auto test2 = test.get();
            }
            //_vertexPositions.second = _drawcall->targetData().collections()."geometry"].get<std::shared_ptr<geometry::Geometry>>(propertyName);
            //_vertexPositions.second = store.get<geometry::Geometry>(propertyName);
        }
        else if (propertyName == _modelToWorldMatrix.first)
            _modelToWorldMatrix.second = store.get<mat4>(propertyName);
        else if (propertyName == _worldToScreenMatrix.first)
            _worldToScreenMatrix.second = store.get<mat4>(propertyName);
    }
    else if (isPropertyRemoved)
    {
        if (propertyName == _vertexPositions.first)
            _vertexPositions.second = nullptr;
        else if (propertyName == _modelToWorldMatrix.first)
            _modelToWorldMatrix.second = math::mat4();
        else if (propertyName == _worldToScreenMatrix.first)
            _worldToScreenMatrix.second = math::mat4();
    }
}

math::vec3
DrawCallZSorter::getEyeSpacePosition() const
{
    auto localPos = math::vec3();
    auto modelView = math::mat4();
    
    if (_drawcall->targetData().hasProperty("centerPosition"))
        localPos = _drawcall->targetData().get<math::vec3>("centerPosition");
    
    if (_drawcall->targetData().hasProperty("modelToWorldMatrix"))
        modelView = _drawcall->targetData().get<math::mat4>("modelToWorldMatrix");

    if (_drawcall->rendererData().hasProperty("worldToScreenMatrix"))
        modelView = _drawcall->rendererData().get<math::mat4>("worldToScreenMatrix") * modelView;

    return math::vec3(modelView * math::vec4(localPos, 1));
}