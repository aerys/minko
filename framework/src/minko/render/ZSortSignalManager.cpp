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

#include "ZSortSignalManager.hpp"

#include "minko/render/DrawCall.hpp"
#include "minko/data/Container.hpp"
#include "minko/math/Matrix4x4.hpp"

using namespace minko;
using namespace minko::data;
using namespace minko::render;
using namespace minko::math;

// names of the properties that may cause a z-sort change between drawcalls
/*static*/ const std::unordered_set<std::string> ZSortSignalManager::TARGET_PNAMES = {
	"material.priority",
	"material.zsorted",
	"geometry.vertex.attribute.position",
	"transform.modelToWorldMatrix"
};
/*static*/ const std::unordered_set<std::string> ZSortSignalManager::RENDERER_PNAMES = {
	"camera.worldToScreenMatrix"
};


ZSortSignalManager::ZSortSignalManager(DrawCall::Ptr drawcall):
	_drawcall(drawcall),
	_targetPropAddedSlot(nullptr),
	_targetPropRemovedSlot(nullptr),
	_rendererPropAddedSlot(nullptr),
	_rendererPropRemovedSlot(nullptr)
{
	if (drawcall == nullptr)
		throw new std::invalid_argument("drawcall");
}

void
ZSortSignalManager::initialize(Container::Ptr targetData, 
							   Container::Ptr rendererData, 
							   Container::Ptr /*rootData*/)
{
	assert(targetData);
	assert(rendererData);

	_targetPropAddedSlot	= targetData->propertyAdded()->connect(std::bind(
		&ZSortSignalManager::propertyAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		TARGET_PNAMES
	));

	_rendererPropAddedSlot	= rendererData->propertyAdded()->connect(std::bind(
		&ZSortSignalManager::propertyAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		RENDERER_PNAMES
	));

	_targetPropRemovedSlot	= targetData->propertyRemoved()->connect(std::bind(
		&ZSortSignalManager::propertyRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		TARGET_PNAMES
	));
	
	_rendererPropRemovedSlot	= rendererData->propertyRemoved()->connect(std::bind(
		&ZSortSignalManager::propertyRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		RENDERER_PNAMES
	));

	_propChangedSlots.clear();

	for (auto& pname : TARGET_PNAMES)
		propertyAddedHandler(targetData, pname, TARGET_PNAMES);
	for (auto& pname : RENDERER_PNAMES)
		propertyAddedHandler(rendererData, pname, RENDERER_PNAMES);
}

void
ZSortSignalManager::clear()
{
	_targetPropAddedSlot		= nullptr;
	_targetPropRemovedSlot		= nullptr;
	_rendererPropAddedSlot		= nullptr;
	_rendererPropRemovedSlot	= nullptr;
	_propChangedSlots			.clear();
	_matrixChangedSlots			.clear();
}

void
ZSortSignalManager::propertyAddedHandler(Container::Ptr							container, 
										 const std::string&						propertyName,
										 const std::unordered_set<std::string>& possibleNames)
{
	assert(container);

	if (possibleNames.find(propertyName) == possibleNames.end())
		return;
	
	if (_propChangedSlots.find(propertyName) == _propChangedSlots.end())
	{
		_propChangedSlots[propertyName] = container->propertyReferenceChanged(propertyName)->connect(std::bind(
			&ZSortSignalManager::requestZSort,
			shared_from_this()
		));

		if (container->hasProperty(propertyName) && 
			container->propertyHasType<Matrix4x4::Ptr>(propertyName, true))
		{
			auto matrix = container->get<Matrix4x4::Ptr>(propertyName);

			_matrixChangedSlots[propertyName] = matrix->changed()->connect(std::bind(
				&ZSortSignalManager::requestZSort,
				shared_from_this()
			));
		}
	}

	requestZSort();
}

void
ZSortSignalManager::propertyRemovedHandler(Container::Ptr							container, 
										   const std::string&						propertyName,
										   const std::unordered_set<std::string>&	possibleNames)
{
	assert(container);

	if (possibleNames.find(propertyName) == possibleNames.end())
		return;

	if (_propChangedSlots.find(propertyName) != _propChangedSlots.end())
	{
		_propChangedSlots.erase(propertyName);

		if (_matrixChangedSlots.find(propertyName) != _matrixChangedSlots.end())
			_matrixChangedSlots.erase(propertyName);
	}

	requestZSort();
}

void
ZSortSignalManager::requestZSort()
{
	if (_drawcall && _drawcall->zsorted())
		_drawcall->zsortNeeded()->execute(_drawcall); // temporary ugly solution
}