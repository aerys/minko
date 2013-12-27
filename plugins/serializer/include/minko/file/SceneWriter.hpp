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

#include "msgpack.hpp"
#include "minko/scene/Node.hpp"
#include "minko/Common.hpp"
#include "minko/file/AbstractWriter.hpp"
#include "minko/serialize/ComponentSerializer.hpp"
#include "minko/MkTypes.hpp"
#include "minko/file/Dependency.hpp"

namespace minko
{
	namespace file
	{
		class SceneWriter : 
			public AbstractWriter<std::shared_ptr<scene::Node>>
		{

		// typedef
		public:
			typedef std::shared_ptr<SceneWriter>										Ptr;
			typedef msgpack::type::tuple<std::string, uint, uint, std::vector<uint>>	SerializedNode;

		// attributes
		private:
			static std::map<const type_info*, std::function<std::string(std::shared_ptr<scene::Node>, std::shared_ptr<file::Dependency>)>> _componentIdToWriteFunction;
		// methods
		public:

			static
			void
			registerComponent(const type_info*																				componentType,
							  std::function<std::string(std::shared_ptr<scene::Node>, std::shared_ptr<file::Dependency>)>	readFunction);

			inline static
			Ptr
			create()
			{
				return std::shared_ptr<SceneWriter>(new SceneWriter());
			}
			

			std::string
			embed(std::shared_ptr<AssetLibrary>		assetLibrary,
				  std::shared_ptr<Options>			options,
				  Dependency::Ptr					dependency);

			SerializedNode
			writeNode(std::shared_ptr<scene::Node>										node,
					  std::vector<std::string>&											serializedControllerList,
					  std::map<std::shared_ptr<component::AbstractComponent>, int>&		controllerMap,
					  std::shared_ptr<file::AssetLibrary>								assetLibrary,
					  std::shared_ptr<Dependency>										dependency);

		private :
			inline
			std::shared_ptr<scene::Node>
			getNode()
			{
				return _data;
			}

		protected:
			SceneWriter()
			{
				registerComponent(
					&typeid(component::PerspectiveCamera), 
					std::bind(
						&serialize::ComponentSerializer::serializePerspectiveCamera,
						std::placeholders::_1, std::placeholders::_2));
				
				registerComponent(
					&typeid(component::Transform),
					std::bind(
						&serialize::ComponentSerializer::serializeTransform,
						std::placeholders::_1, std::placeholders::_2));

				registerComponent(
					&typeid(component::AmbientLight),
					std::bind(
						&serialize::ComponentSerializer::serializeAmbientLight,
						std::placeholders::_1, std::placeholders::_2));

				registerComponent(
					&typeid(component::DirectionalLight),
					std::bind(
						&serialize::ComponentSerializer::serializeDirectionalLight,
						std::placeholders::_1, std::placeholders::_2));

				registerComponent(
					&typeid(component::SpotLight),
					std::bind(
						&serialize::ComponentSerializer::serializeSpotLight,
						std::placeholders::_1, std::placeholders::_2));

				registerComponent(
					&typeid(component::PointLight),
					std::bind(
						&serialize::ComponentSerializer::serializePointLight,
						std::placeholders::_1, std::placeholders::_2));

				registerComponent(
					&typeid(component::Surface),
					std::bind(
						&serialize::ComponentSerializer::serializeSurface,
						std::placeholders::_1, std::placeholders::_2));

				registerComponent(
					&typeid(component::Renderer),
					std::bind(
						&serialize::ComponentSerializer::serializeRenderer,
						std::placeholders::_1, std::placeholders::_2));
			}
		};
	}
}
