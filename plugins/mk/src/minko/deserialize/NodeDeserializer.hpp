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

#pragma once

#include "minko/Common.hpp"
#include "minko/scene/Node.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/deserialize/SceneDeserializer.hpp"

namespace minko
{
	namespace deserialize
	{
		class NodeDeserializer
		{

		private:
			inline static
			std::string
			extractName(SceneDeserializer::NodeInfo nodeInfo)
			{
				return Any::cast<std::string>(nodeInfo["name"]);
			}

			inline static
			std::shared_ptr<math::Matrix4x4>
			extractTransform(SceneDeserializer::NodeInfo nodeInfo)
			{
				return math::Matrix4x4::create();
			}

		public:
			
			inline static
			std::shared_ptr<scene::Node>
			deserializeGroup(SceneDeserializer::NodeInfo		nodeInfo,
							 SceneDeserializer::OptionsPtr		options,
							 SceneDeserializer::ControllerMap	controllerMap,
							 SceneDeserializer::NodeMap			nodeMap)
			{
				std::shared_ptr<scene::Node> group = scene::Node::create(extractName(nodeInfo));

				return group;
			}

			inline static
			std::shared_ptr<scene::Node>
			deserializeMesh(SceneDeserializer::NodeInfo			nodeInfo,
							SceneDeserializer::OptionsPtr		options,
							SceneDeserializer::ControllerMap	controllerMap,
							SceneDeserializer::NodeMap			nodeMap)
			{
				std::shared_ptr<scene::Node> mesh = scene::Node::create(extractName(nodeInfo));

				return mesh;
			}

			inline static
			std::shared_ptr<scene::Node>
			deserializeLight(SceneDeserializer::NodeInfo		nodeInfo,
							 SceneDeserializer::OptionsPtr		options,
							 SceneDeserializer::ControllerMap	controllerMap,
							 SceneDeserializer::NodeMap			nodeMap)
			{
				std::shared_ptr<scene::Node> light = scene::Node::create(extractName(nodeInfo));

				return light;
			}

			inline static
			std::shared_ptr<scene::Node>
			deserializeCamera(SceneDeserializer::NodeInfo		nodeInfo,
							  SceneDeserializer::OptionsPtr		options,
							  SceneDeserializer::ControllerMap	controllerMap,
							  SceneDeserializer::NodeMap		nodeMap)
			{
				std::shared_ptr<scene::Node> camera = scene::Node::create(extractName(nodeInfo));

				return camera;
			}


		};
	}
}
