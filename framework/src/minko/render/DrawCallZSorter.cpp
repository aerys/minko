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

#include "DrawCallZSorter.hpp"

#include "minko/render/DrawCall.hpp"
#include "minko/data/Container.hpp"
#include "minko/render/VertexBuffer.hpp"

using namespace minko;
using namespace minko::data;
using namespace minko::render;

// names of the properties that may cause a z-sort change between drawcalls
/*static*/ const DrawCallZSorter::PropertyInfos	DrawCallZSorter::_rawProperties = initializeRawProperties();

/*static*/
DrawCallZSorter::PropertyInfos
DrawCallZSorter::initializeRawProperties()
{
	PropertyInfos props;

	props["material[${materialId}].priority"]	= data::BindingSource::TARGET;
	props["material[${materialId}].zSort"]		= data::BindingSource::TARGET;
	props["geometry[${geometryId}].position"]	= data::BindingSource::TARGET;
	props["transform.modelToWorldMatrix"]		= data::BindingSource::TARGET;
	props["camera.worldToScreenMatrix"]			= data::BindingSource::RENDERER;

	return props;
}

DrawCallZSorter::DrawCallZSorter(DrawCall::Ptr drawcall):
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
		throw new std::invalid_argument("drawcall");
}

void
DrawCallZSorter::initialize(Container::Ptr targetData, 
							Container::Ptr rendererData, 
							Container::Ptr /*rootData*/)
{
	assert(targetData);
	assert(rendererData);

	clear();

	// format raw property name to fit with the actual draw call
	for (auto& prop : _rawProperties)
		_properties[_drawcall->formatPropertyName(prop.first)] = prop.second;

	_vertexPositions.first		= _drawcall->formatPropertyName("geometry[${geometryId}].position");
	_modelToWorldMatrix.first	= _drawcall->formatPropertyName("transform.modelToWorldMatrix");
	_worldToScreenMatrix.first	= _drawcall->formatPropertyName("camera.worldToScreenMatrix");

	_targetPropAddedSlot	= targetData->propertyAdded()->connect(std::bind(
		&DrawCallZSorter::propertyAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_rendererPropAddedSlot	= rendererData->propertyAdded()->connect(std::bind(
		&DrawCallZSorter::propertyAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetPropRemovedSlot	= targetData->propertyRemoved()->connect(std::bind(
		&DrawCallZSorter::propertyRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
	
	_rendererPropRemovedSlot	= rendererData->propertyRemoved()->connect(std::bind(
		&DrawCallZSorter::propertyRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_propChangedSlots.clear();

	for (auto& prop : _properties)
	{
		auto container = prop.second == data::BindingSource::RENDERER ? rendererData : targetData;
		if (container->hasProperty(prop.first))
			propertyAddedHandler(container, prop.first); 
	}		
}

void
DrawCallZSorter::clear()
{
	_targetPropAddedSlot		= nullptr;
	_targetPropRemovedSlot		= nullptr;
	_rendererPropAddedSlot		= nullptr;
	_rendererPropRemovedSlot	= nullptr;
	_propChangedSlots			.clear();
	_matrixChangedSlots			.clear();
	_properties					.clear();
}

void
DrawCallZSorter::propertyAddedHandler(Container::Ptr		container, 
									  const std::string&	propertyName)
{
	assert(container);

	const auto foundPropIt	= _properties.find(propertyName);
	if (foundPropIt == _properties.end())
		return;
	
	recordIfPositionalMembers(container, propertyName, true, false);

	if (_propChangedSlots.find(propertyName) == _propChangedSlots.end())
	{
		_propChangedSlots[propertyName] = container->propertyValueChanged(propertyName)->connect(
			[&](data::Container::Ptr, const std::string&)
			{
				requestZSort();
			}
		);
	}

	requestZSort();
}

void
DrawCallZSorter::propertyRemovedHandler(Container::Ptr		container, 
										const std::string&	propertyName)
{
	assert(container);

	if (_properties.find(propertyName) == _properties.end())
		return;

	recordIfPositionalMembers(container, propertyName, false, true);

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
		_drawcall->zsortNeeded()->execute(_drawcall); // temporary ugly solution
}

void
DrawCallZSorter::recordIfPositionalMembers(Container::Ptr		container,
										   const std::string&	propertyName,
										   bool					isPropertyAdded,
										   bool					isPropertyRemoved)
{
	if (isPropertyAdded)
	{
		if (propertyName == _vertexPositions.first)
			_vertexPositions.second	= container->get<VertexBuffer::Ptr>(propertyName);
		else if (propertyName == _modelToWorldMatrix.first)
			_modelToWorldMatrix.second	= new math::mat4(container->get<math::mat4>(propertyName));
		else if (propertyName == _worldToScreenMatrix.first)
			_worldToScreenMatrix.second	= new math::mat4(container->get<math::mat4>(propertyName));
	}
	else if (isPropertyRemoved)
	{
		if (propertyName == _vertexPositions.first)
			_vertexPositions.second	= nullptr;
		else if (propertyName == _modelToWorldMatrix.first)
		{
			delete _modelToWorldMatrix.second;
			_modelToWorldMatrix.second = nullptr;
		}
		else if (propertyName == _worldToScreenMatrix.first)
		{
			delete _worldToScreenMatrix.second;
			_worldToScreenMatrix.second	= nullptr;
		}
	}
}

math::vec3
DrawCallZSorter::getEyeSpacePosition()  const
{
	const auto& vb = _vertexPositions.second;

	math::vec3 localPos(0.f);
	math::mat4 modelView(1.f);

	if (vb)
		localPos = vb->minPosition() + (vb->maxPosition() - vb->minPosition()) * .5f;

	if (_modelToWorldMatrix.second)
		modelView = *_modelToWorldMatrix.second;

	if (_worldToScreenMatrix.second)
		modelView = *_worldToScreenMatrix.second * modelView;

	return (modelView * math::vec4(localPos, 1.f)).xyz();
}