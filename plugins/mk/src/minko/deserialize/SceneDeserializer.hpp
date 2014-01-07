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

#include "minko/MkCommon.hpp"
#include "minko/Qark.hpp"
#include "minko/file/MkOptions.hpp"

namespace minko
{
	namespace deserialize
	{
		class SceneDeserializer
		{
			// typedef
		public:
			typedef std::shared_ptr<SceneDeserializer>													Ptr;
			typedef std::shared_ptr<render::AbstractContext>											ContextPtr;
		
		private:
			typedef	std::function<std::shared_ptr<scene::Node>(NodeInfo&, OptionsPtr, NodeMap&)> NodeDeserializerFunc;

			// attributes
		private:
			std::shared_ptr<file::AssetLibrary>	                _deserializedAssets;
			ContextPtr							                _context; 
			std::map<int, NodeDeserializerFunc>	                _nodeDeserializer;
			std::map<std::shared_ptr<scene::Node>, NodeInfo&>   _pluginControllers;

			// method
		public:
			inline static
			Ptr
			create(ContextPtr context)
			{
				return std::shared_ptr<SceneDeserializer>(new SceneDeserializer(context));
			}

			inline
			std::shared_ptr<file::AssetLibrary>
			deserializedAssets()
			{
				return _deserializedAssets;
			}

			std::shared_ptr<scene::Node>
			deserializeScene(Qark::Object&	sceneObject,
							 Qark::Object&	assetsObject,
							 OptionsPtr		options,
							 NodeMap&		nodeMap);

		private:
			std::shared_ptr<scene::Node>
			deserializeNode(Qark::Object&	nodeObject,
							OptionsPtr		options,
							NodeMap&		nodeMap);


			std::shared_ptr<scene::Node>
			deserializeGroup(NodeInfo&		nodeInfo,
							 OptionsPtr		options,
							 NodeMap&		nodeMap);

			std::shared_ptr<scene::Node>
			deserializeMesh(NodeInfo&		nodeInfo,
							OptionsPtr		options,
							NodeMap&			nodeMap);

			std::shared_ptr<scene::Node>
			deserializeCamera(NodeInfo&			nodeInfo,
							  OptionsPtr		options,
							  NodeMap&			nodeMap);

			std::shared_ptr<scene::Node>
			deserializeLight(NodeInfo&		nodeInfo,
							 OptionsPtr		options,
							 NodeMap&		nodeMap);

			void
			initializeNodeDeserializer();

			SceneDeserializer(ContextPtr context) : 
				_context(context),
				_deserializedAssets(file::AssetLibrary::create(context))
			{
			}
		};
	}
}
