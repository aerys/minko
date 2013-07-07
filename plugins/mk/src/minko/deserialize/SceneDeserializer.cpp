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

#include "SceneDeserializer.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::math;

namespace minko
{
	namespace deserialize
	{
		std::shared_ptr<scene::Node>
		SceneDeserializer::deserializeScene(Qark::Object		sceneObject,
										    Qark::Object		assetsObject,
										    OptionsPtr			options,
										    ControllerMap		controllerMap,
										    NodeMap				nodeMap)
		{
			initializeNodeDeserializer();

			return deserializeNode(sceneObject, options, controllerMap, nodeMap);
		}

		void
		SceneDeserializer::initializeNodeDeserializer()
		{
			_nodeDeserializer[MkTypes::GROUP]	= &SceneDeserializer::deserializeGroup;
			_nodeDeserializer[MkTypes::MESH]	= &SceneDeserializer::deserializeMesh;
			_nodeDeserializer[MkTypes::LIGHT]	= &SceneDeserializer::deserializeMesh;
			_nodeDeserializer[MkTypes::CAMERA]	= &SceneDeserializer::deserializeMesh;
		}

		std::shared_ptr<scene::Node>
		SceneDeserializer::deserializeNode(Qark::Object		nodeObject,
										   OptionsPtr		options,
										   ControllerMap	controllerMap,
										   NodeMap			nodeMap)
		{
			NodeInfo nodeInformation = Any::cast<NodeInfo>(nodeObject);

			int type			= Any::cast<int>(nodeInformation["type"]);
			NodeDeserializer f	= _nodeDeserializer[type];

			return (this->*f)(nodeInformation, options, controllerMap, nodeMap);
		}

		std::shared_ptr<scene::Node>
		SceneDeserializer::deserializeGroup(NodeInfo		nodeInfo,
											OptionsPtr		options,
											ControllerMap	controllerMap,
											NodeMap			nodeMap)
		{
			std::shared_ptr<scene::Node> node = scene::Node::create(Any::cast<std::string>(nodeInfo["name"]));


			std::vector<Any> children = Any::cast<std::vector<Any>>(nodeInfo["children"]);

			for (int i = 0; i < children.size(); ++i)
				node->addChild(deserializeNode(children[i], options, controllerMap, nodeMap));
			
			std::cout << "Group " << node->name() << std::endl << std::flush;

			return node;
		}

		std::shared_ptr<scene::Node>
		SceneDeserializer::deserializeMesh(NodeInfo			nodeInfo,
										   OptionsPtr		options,
										   ControllerMap	controllerMap,
										   NodeMap			nodeMap)
		{
			auto view			= Matrix4x4::create()->perspective(.785f, 800.f / 600.f, .1f, 1000.f);
			auto color			= Vector4::create(0.f, 0.f, 1.f, .1f);
			auto lightDirection = Vector3::create(0.f, -1.f, -1.f);

			std::shared_ptr<scene::Node> node = scene::Node::create(Any::cast<std::string>(nodeInfo["name"]));

			node->addController(controller::TransformController::create());
			node->controller<controller::TransformController>()->transform()->appendTranslation(0.f, 0.f, -3.f);
			node->addController(
				controller::SurfaceController::create(
					options->assets()->geometry("cube"),
					data::Provider::create()
						->set("material/diffuse/rgba",			color)
						->set("transform/worldToScreenMatrix",	view)
						->set("light/direction",				lightDirection)
						->set("material/diffuse/map",			options->assets()->texture("textures/box3.png")),
					options->assets()->effect("texture")));
			std::cout << "Mesh " << node->name() << std::endl << std::flush;

			return node;
		}

	}
}