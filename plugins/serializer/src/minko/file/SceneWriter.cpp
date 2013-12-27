/*
Copyright (c) 2013 Aerys

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

#include "minko/component/AbstractComponent.hpp"
#include "minko/filE/SceneWriter.hpp"
#include <queue>
#include <iostream>
#include <fstream>



using namespace minko;
using namespace minko::file;

std::map<const type_info*, std::function<std::string(std::shared_ptr<scene::Node>, std::shared_ptr<file::Dependency>)>> SceneWriter::_componentIdToWriteFunction;

void
SceneWriter::registerComponent(const type_info*																				componentType,
							  std::function<std::string(std::shared_ptr<scene::Node>, std::shared_ptr<file::Dependency>)>	readFunction)
{
	_componentIdToWriteFunction[componentType] = readFunction;
}

std::string
SceneWriter::embed(std::shared_ptr<AssetLibrary>	assetLibrary,
				  std::shared_ptr<Options>			options,
				  Dependency::Ptr					dependency)
{
	std::stringstream												sbuf;
	std::queue<std::shared_ptr<scene::Node>>						queue;
	std::vector<SerializedNode>										nodePack;
	std::vector<std::string>										serializedControllerList;
	std::map<std::shared_ptr<component::AbstractComponent>, int>	controllerMap;
	std::map<std::tuple<uint, uint>, std::string>					serializedAssetMap;

	queue.push(data());

	while (queue.size() > 0)
	{
		std::shared_ptr<scene::Node>	currentNode = queue.front();

		nodePack.push_back(writeNode(currentNode, serializedControllerList, controllerMap, assetLibrary, dependency));
		
		for (uint i = 0; i < currentNode->children().size(); ++i)
			queue.push(currentNode->children()[i]);

		queue.pop();
	}

	msgpack::type::tuple<std::vector<std::string>, std::vector<SerializedNode>> res(serializedControllerList, nodePack);
	msgpack::pack(sbuf, res);
	
	return sbuf.str();
}

SceneWriter::SerializedNode
SceneWriter::writeNode(std::shared_ptr<scene::Node>										node,
					  std::vector<std::string>&											serializedControllerList,
					  std::map<std::shared_ptr<component::AbstractComponent>, int>&		controllerMap,
					  std::shared_ptr<file::AssetLibrary>								assetLibrary,
					  std::shared_ptr<Dependency>										dependency)
{
	std::vector<uint>								componentsId;
	int												componentIndex = 0;
	std::shared_ptr<component::AbstractComponent>	currentComponent = node->component<component::AbstractComponent>(0);
	auto copy	= _componentIdToWriteFunction;

	while (currentComponent != nullptr)
	{
		int index = -1;

		if (controllerMap.find(currentComponent) != controllerMap.end())
			index = controllerMap[currentComponent];
		else
		{
			const type_info* currentComponentType = &typeid(*currentComponent);

			if (_componentIdToWriteFunction.find(currentComponentType) != _componentIdToWriteFunction.end())
			{
				index = serializedControllerList.size();
				serializedControllerList.push_back(_componentIdToWriteFunction[currentComponentType](node, dependency));
			}
		}

		if (index != -1)
			componentsId.push_back(index);

		currentComponent = node->component<component::AbstractComponent>(++componentIndex);
	}

	SerializedNode res(node->name(), node->layouts(), node->children().size(), componentsId);

	return res;
}

