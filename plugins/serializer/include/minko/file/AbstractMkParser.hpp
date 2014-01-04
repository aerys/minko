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
#include "minko/file/AbstractParser.hpp"
#include "msgpack.hpp"

namespace minko
{
	namespace file
	{
		class AbstractMkParser:
			public AbstractParser
		{
		public:
			typedef std::shared_ptr<AbstractMkParser> Ptr;
		
		protected:
			std::shared_ptr<Dependency>		_dependencies;
			std::shared_ptr<GeometryParser> _geometryParser;
			std::shared_ptr<MaterialParser> _materialParser;

			std::string						_lastParsedAssetName;

		public:
			inline static
			Ptr
			create();

			virtual
			void
			parse(const std::string&				filename,
				  const std::string&                resolvedFilename,
				  std::shared_ptr<Options>          options,
				  const std::vector<unsigned char>&	data,
				  std::shared_ptr<AssetLibrary>		assetLibrary);

		protected:
			std::string
			extractDependencies(std::shared_ptr<AssetLibrary>		assetLibrary,
								const std::vector<unsigned char>&	data,
								std::shared_ptr<Options>			options);

			inline
			void
			dependecy(std::shared_ptr<Dependency> dependecies)
			{
				_dependencies = dependecies;
			}

		protected:
			AbstractMkParser();

			void
			deserializedAsset(msgpack::type::tuple<unsigned char, short, std::string>	asset,
							  std::shared_ptr<AssetLibrary>								assetLibrary,
							  std::shared_ptr<Options>									options);
		};
	}
}
