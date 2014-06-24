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

#include "minko/SerializerCommon.hpp"
#include "minko/file/AbstractParser.hpp"
#include "msgpack.hpp"
#include "minko/component/JobManager.hpp"

namespace minko
{
	namespace file
	{
		class AbstractSerializerParser:
			public AbstractParser
		{
		public:
			typedef std::shared_ptr<AbstractSerializerParser>				Ptr;
			typedef msgpack::type::tuple<unsigned int, short, std::string>	SerializedAsset;
			typedef std::shared_ptr<file::AssetLibrary>						AssetLibraryPtr;

		private:
			typedef std::shared_ptr<component::JobManager::Job>												JobPtr;
			typedef std::shared_ptr<Dependency>																DependencyPtr;
			typedef std::function<void(unsigned char, AssetLibraryPtr, std::string&, DependencyPtr, short, std::list<JobPtr>&)>	AssetDeserializeFunction;

		protected:
			DependencyPtr						_dependencies;
			std::shared_ptr<GeometryParser>		_geometryParser;
			std::shared_ptr<MaterialParser>		_materialParser;

			std::string												_lastParsedAssetName;
			std::list<std::shared_ptr<component::JobManager::Job>>	_jobList;

		private:
			static std::unordered_map<uint, AssetDeserializeFunction> _assetTypeToFunction;

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
				  AssetLibraryPtr					assetLibrary);

			static
			void
			registerAssetFunction(uint assetTypeId, AssetDeserializeFunction f);

		protected:
			void
			extractDependencies(AssetLibraryPtr							assetLibrary, 
			  				    const std::vector<unsigned char>&		data,
								short									dataOffset,
								unsigned int							dependenciesSize,
								std::shared_ptr<Options>				options,
								std::string&							assetFilePath);

			inline
			void
			dependecy(std::shared_ptr<Dependency> dependecies)
			{
				_dependencies = dependecies;
			}

		protected:
			AbstractSerializerParser();

			void
			deserializedAsset(SerializedAsset&					asset,
							  AssetLibraryPtr					assetLibrary,
							  std::shared_ptr<Options>			options,
							  std::string&						assetFilePath);

			std::string
			extractFolderPath(const std::string& filepath);

			int
			readInt(const std::vector<unsigned char>& data, int offset)
			{
				return (int)(data[offset] << 24 | data[offset + 1] << 16 | data[offset + 2] << 8 | data[offset + 3]);
			}

			unsigned int
			readUInt(const std::vector<unsigned char>& data, int offset)
			{
				return (unsigned int)(data[offset] << 24 | data[offset + 1] << 16 | data[offset + 2] << 8 | data[offset + 3]);
			}

			short
			readShort(const std::vector<unsigned char>& data, int offset)
			{
				return (short)(data[offset] << 8 | data[offset + 1]);
			}
		};
	}
}
