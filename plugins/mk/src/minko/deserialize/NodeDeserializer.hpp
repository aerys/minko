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
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/file/MkOptions.hpp"
#include "minko/controller/SurfaceController.hpp"
#include "minko/controller/TransformController.hpp"
#include "minko/geometry/CubeGeometry.hpp"
#include "minko/math/Matrix4x4.hpp"

namespace minko
{
	namespace deserialize
	{
		class NodeDeserializer
		{

		private:
			typedef	std::map<std::string, Any>															NodeInfo;
			typedef std::map<std::shared_ptr<scene::Node>, std::vector<controller::AbstractController>>	ControllerMap;
			typedef std::map<std::shared_ptr<scene::Node>, uint>										NodeMap;
			typedef std::shared_ptr<file::MkOptions>													OptionsPtr;


		private:
			inline static
			std::string
			extractName(NodeInfo nodeInfo)
			{
				return Any::cast<std::string>(nodeInfo["name"]);
			}

		public:
			
			inline static
			std::shared_ptr<scene::Node>
			deserializeGroup(NodeInfo		nodeInfo,
							 OptionsPtr		options,
							 ControllerMap	controllerMap,
							 NodeMap			nodeMap)
			{
				std::shared_ptr<scene::Node>		group			= scene::Node::create(extractName(nodeInfo));
				std::shared_ptr<math::Matrix4x4>	transformMatrix = TypeDeserializer::matrix4x4(nodeInfo["transformation"]);

				group->addController(controller::TransformController::create());
				group->controller<controller::TransformController>()->transform()->copyFrom(transformMatrix);

				return group;
			}

			inline static
			std::shared_ptr<scene::Node>
			deserializeMesh(NodeInfo			nodeInfo,
							OptionsPtr		options,
							ControllerMap	controllerMap,
							NodeMap			nodeMap)
			{
				std::shared_ptr<scene::Node>		mesh			= scene::Node::create(extractName(nodeInfo));
				std::shared_ptr<math::Matrix4x4>	transformMatrix = TypeDeserializer::matrix4x4(nodeInfo["transform"]);

				mesh->addController(controller::TransformController::create());
				mesh->controller<controller::TransformController>()->transform()->copyFrom(transformMatrix);
				// temp
				auto view			= math::Matrix4x4::create()->perspective(.785f, 800.f / 600.f, .1f, 1000.f);
				auto color			= math::Vector4::create(0.f, 0.f, 1.f, 1.f);
				auto lightDirection = math::Vector3::create(0.f, -1.f, -1.f);

				mesh->addController(
					controller::SurfaceController::create(
					options->assetsLibrary()->geometry("cube"),
					data::Provider::create()
					->set("material/diffuse/rgba",			color)
					->set("transform/worldToScreenMatrix",	view)
					->set("light/direction",				lightDirection)
					->set("material/diffuse/map",			options->assetsLibrary()->texture("box3.png")),
					options->assetsLibrary()->effect("texture")));

				return mesh;
			}

			inline static
			std::shared_ptr<scene::Node>
			deserializeLight(NodeInfo		nodeInfo,
							 OptionsPtr		options,
							 ControllerMap	controllerMap,
							 NodeMap		nodeMap)
			{
				std::shared_ptr<scene::Node> light = scene::Node::create(extractName(nodeInfo));

				return light;
			}

			inline static
			std::shared_ptr<scene::Node>
			deserializeCamera(NodeInfo		nodeInfo,
							  OptionsPtr	options,
							  ControllerMap	controllerMap,
							  NodeMap		nodeMap)
			{
				std::shared_ptr<scene::Node> camera = scene::Node::create(extractName(nodeInfo));

				return camera;
			}


		};
	}
}
