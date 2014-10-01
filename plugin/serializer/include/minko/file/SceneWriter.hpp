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

#include "msgpack.hpp"
#include "minko/file/AbstractWriter.hpp"

namespace minko
{
    namespace file
    {
        class WriterOptions;
    }
}

namespace minko
{
	namespace file
	{
		class SceneWriter :
			public AbstractWriter<std::shared_ptr<scene::Node>>
		{
		public:
			typedef std::shared_ptr<SceneWriter>													Ptr;
			typedef msgpack::type::tuple<std::string, uint, uint, std::vector<uint>, std::string>	SerializedNode;

		private:
			typedef std::shared_ptr<file::Dependency> 					DependencyPtr;
			typedef std::shared_ptr<scene::Node> 						NodePtr;
			typedef std::shared_ptr<component::AbstractComponent>		AbstractComponentPtr;
			typedef std::function<std::string(NodePtr, AbstractComponentPtr, DependencyPtr)>	NodeWriterFunc;
			typedef std::shared_ptr<file::AssetLibrary>					AssetLibraryPtr;
			typedef std::shared_ptr<Options>                            OptionsPtr;

		private:
			static std::map<const std::type_info*, NodeWriterFunc> _componentIdToWriteFunction;

		public:
			static
			void
			registerComponent(const std::type_info*	componentType,
							  NodeWriterFunc		readFunction);

			inline static
			Ptr
			create()
			{
				return std::shared_ptr<SceneWriter>(new SceneWriter());
			}

			std::string
            embed(AssetLibraryPtr                       assetLibrary,
                  OptionsPtr                            options,
                  DependencyPtr                         dependency,
                  std::shared_ptr<WriterOptions>        writerOptions);

			SerializedNode
			writeNode(std::shared_ptr<scene::Node>			node,
					  std::vector<std::string>&				serializedControllerList,
					  std::map<AbstractComponentPtr, int>&	controllerMap,
					  AssetLibraryPtr						assetLibrary,
					  DependencyPtr							dependency);

		private :
			inline
			std::shared_ptr<scene::Node>
			getNode()
			{
				return _data;
			}

		protected:
			SceneWriter();
		};
	}
}
