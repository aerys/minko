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

#include "minko/file/SceneWriter.hpp"

#include "minko/component/AbstractComponent.hpp"
#include "minko/scene/Node.hpp"
#include "minko/component/Animation.hpp"
#include "minko/component/MasterAnimation.hpp"
#include "minko/component/BoundingBox.hpp"
#include "minko/component/Camera.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/ImageBasedLight.hpp"
#include "minko/component/Skinning.hpp"
#include "minko/component/SpotLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/Metadata.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/file/WriterOptions.hpp"
#include "minko/serialize/ComponentSerializer.hpp"
#include "minko/Types.hpp"

using namespace minko;
using namespace minko::file;

std::map<const std::type_info*, SceneWriter::NodeWriterFunc> SceneWriter::_componentIdToWriteFunction;

SceneWriter::SceneWriter(WriterOptions::Ptr writerOptions)
{
	_magicNumber = MINKO_SCENE_MAGIC_NUMBER;

	registerComponent(
		&typeid(component::Camera),
		std::bind(
			&serialize::ComponentSerializer::serializeCamera,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
		)
	);

	registerComponent(
		&typeid(component::Transform),
		std::bind(
			&serialize::ComponentSerializer::serializeTransform,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
            )
	);

    registerComponent(
        &typeid(component::ImageBasedLight),
        std::bind(
            &serialize::ComponentSerializer::serializeImageBasedLight,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
        )
    );

	registerComponent(
		&typeid(component::AmbientLight),
		std::bind(
			&serialize::ComponentSerializer::serializeAmbientLight,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
		)
	);

	registerComponent(
		&typeid(component::DirectionalLight),
		std::bind(
			&serialize::ComponentSerializer::serializeDirectionalLight,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
		)
	);

	registerComponent(
		&typeid(component::SpotLight),
		std::bind(
			&serialize::ComponentSerializer::serializeSpotLight,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
		)
	);

	registerComponent(
		&typeid(component::PointLight),
		std::bind(
			&serialize::ComponentSerializer::serializePointLight,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
		)
	);

	registerComponent(
		&typeid(component::Surface),
		std::bind(
			&serialize::ComponentSerializer::serializeSurface,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
		)
	);

	registerComponent(
		&typeid(component::Renderer),
		std::bind(
			&serialize::ComponentSerializer::serializeRenderer,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
		)
	);

    if (writerOptions->writeAnimations())
    {
	    registerComponent(
	    	&typeid(component::MasterAnimation),
	    	std::bind(
	    		&serialize::ComponentSerializer::serializeMasterAnimation,
	    		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
	    	)
	    );

	    registerComponent(
	    	&typeid(component::Animation),
	    	std::bind(
	    		&serialize::ComponentSerializer::serializeAnimation,
	    		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
	    	)
	    );

        registerComponent(
	    	&typeid(component::Skinning),
	    	std::bind(
	    		&serialize::ComponentSerializer::serializeSkinning,
	    		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
	    	)
	    );
    }

	registerComponent(
		&typeid(component::BoundingBox),
		std::bind(
			&serialize::ComponentSerializer::serializeBoundingBox,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
		)
	);

	registerComponent(
		&typeid(component::Metadata),
		std::bind(
			&serialize::ComponentSerializer::serializeMetadata,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
		)
	);
}

void
SceneWriter::registerComponent(const std::type_info*	componentType,
							   NodeWriterFunc			readFunction)
{
	_componentIdToWriteFunction[componentType] = readFunction;
}

std::string
SceneWriter::embed(AssetLibraryPtr                      assetLibrary,
                   OptionsPtr                           options,
                   DependencyPtr                        dependency,
                   std::shared_ptr<WriterOptions>       writerOptions,
                   std::vector<unsigned char>&          embeddedHeaderData)
{
	std::stringstream								sbuf;
	std::queue<std::shared_ptr<scene::Node>>		queue;
	std::vector<SerializedNode>						nodePack;
	std::vector<std::string>						serializedControllerList;
	std::map<AbstractComponentPtr, int>				controllerMap;

	queue.push(data());

	while (queue.size() > 0)
	{
		std::shared_ptr<scene::Node>	currentNode = queue.front();

		nodePack.push_back(writeNode(currentNode, serializedControllerList, controllerMap, assetLibrary, dependency, writerOptions));

		for (uint i = 0; i < currentNode->children().size(); ++i)
			queue.push(currentNode->children()[i]);

		queue.pop();
	}

	msgpack::type::tuple<std::vector<std::string>, std::vector<SerializedNode>> res(serializedControllerList, nodePack);
	msgpack::pack(sbuf, res);

	return sbuf.str();
}

SceneWriter::SerializedNode
SceneWriter::writeNode(std::shared_ptr<scene::Node>		node,
					  std::vector<std::string>&			serializedControllerList,
					  std::map<AbstractComponentPtr, int>&	controllerMap,
					  AssetLibraryPtr					assetLibrary,
					  DependencyPtr						dependency,
					  WriterOptions::Ptr 				writerOptions)
{
	if (writerOptions->addBoundingBoxes() &&
		node->hasComponent<component::Surface>() &&
	 	!node->hasComponent<component::BoundingBox>())
	{
			node->addComponent(component::BoundingBox::create());
	}

 	std::vector<uint>	componentsId;
	int					componentIndex = 0;
	AbstractComponentPtr		currentComponent = node->component<component::AbstractComponent>(0);

	while (currentComponent != nullptr)
	{
		int index = -1;

		if (controllerMap.find(currentComponent) != controllerMap.end())
			index = controllerMap[currentComponent];
		else
		{
			const std::type_info* currentComponentType = &typeid(*currentComponent);

			if (_componentIdToWriteFunction.find(currentComponentType) != _componentIdToWriteFunction.end())
			{
				index = serializedControllerList.size();
				serializedControllerList.push_back(_componentIdToWriteFunction[currentComponentType](node, currentComponent, assetLibrary, dependency));
			}
		}

		if (index != -1)
			componentsId.push_back(index);

		currentComponent = node->component<component::AbstractComponent>(++componentIndex);
	}

	SerializedNode res(node->name(), node->layout(), node->children().size(), componentsId, node->uuid());

	return res;
}
