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
#include "minko/MkTypes.hpp"
#include "msgpack.hpp"
#include "minko/file/AbstractMkParser.hpp"
#include "minko/deserialize/ComponentDeserializer.hpp"
#include "minko/file/GeometryParser.hpp"
#include "minko/file/MaterialParser.hpp"


namespace minko
{
	namespace file
	{
		class SceneParser : 
			public AbstractMkParser
		{

		//typedef
		public:
			typedef std::shared_ptr<SceneParser>																														Ptr;
			typedef std::function<std::shared_ptr<component::AbstractComponent>(std::string, std::shared_ptr<file::AssetLibrary>, std::shared_ptr<file::Dependency>)>	ComponentReadFunction;
			typedef msgpack::type::tuple<std::string, uint, uint, std::vector<uint>>																					SerializedNode;

		// methods
		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<SceneParser>(new SceneParser());
			}

			static
			void
			registerComponent(int8_t				componentId,
							  ComponentReadFunction readFunction);

			void
			parse(const std::string&				filename,
				  const std::string&                resolvedFilename,
                  std::shared_ptr<Options>          options,
				  const std::vector<unsigned char>&	data,
				  std::shared_ptr<AssetLibrary>		assetLibrary);

		private:

			std::shared_ptr<scene::Node>
			parseNode(std::vector<SerializedNode>			nodePack, 
					  std::vector<std::string>				componentPack,
					  std::shared_ptr<file::AssetLibrary>	assetLibrary);

			SceneParser()
			{
				_geometryParser = file::GeometryParser::create();
				_materialParser = file::MaterialParser::create();

				registerComponent(mk::PROJECTION_CAMERA,
					std::bind(&deserialize::ComponentDeserializer::deserializeProjectionCamera,
							  std::placeholders::_1,
							  std::placeholders::_2,
							  std::placeholders::_3));
			
				registerComponent(mk::TRANSFORM,
					std::bind(&deserialize::ComponentDeserializer::deserializeTransform,
							  std::placeholders::_1,
							  std::placeholders::_2,
							  std::placeholders::_3));

				registerComponent(mk::AMBIENT_LIGHT,
					std::bind(&deserialize::ComponentDeserializer::deserializeAmbientLight,
							  std::placeholders::_1,
							  std::placeholders::_2,
							  std::placeholders::_3));

				registerComponent(mk::DIRECTIONAL_LIGHT,
					std::bind(&deserialize::ComponentDeserializer::deserializeDirectionalLight,
							  std::placeholders::_1,
							  std::placeholders::_2,
							  std::placeholders::_3));

				registerComponent(mk::SPOT_LIGHT,
					std::bind(&deserialize::ComponentDeserializer::deserializeSpotLight,
							  std::placeholders::_1,
							  std::placeholders::_2,
							  std::placeholders::_3));

				registerComponent(mk::POINT_LIGHT,
					std::bind(&deserialize::ComponentDeserializer::deserializePointLight,
							  std::placeholders::_1,
							  std::placeholders::_2,
							  std::placeholders::_3));

				registerComponent(mk::SURFACE,
					std::bind(&deserialize::ComponentDeserializer::deserializeSurface,
							  std::placeholders::_1,
							  std::placeholders::_2,
							  std::placeholders::_3));

				registerComponent(mk::RENDERER,
					std::bind(&deserialize::ComponentDeserializer::deserializeRenderer,
							  std::placeholders::_1,
							  std::placeholders::_2,
							  std::placeholders::_3));
			}
		};
	}
}
