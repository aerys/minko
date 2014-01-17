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

#include "minko/file/AbstractSerializerParser.hpp"
#include "msgpack.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/GeometryParser.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/file/MaterialParser.hpp"
#include "minko/data/Provider.hpp"
#include "minko/material/Material.hpp"
#include "minko/file/AbstractParser.hpp"

using namespace minko;
using namespace minko::file;


AbstractSerializerParser::Ptr
AbstractSerializerParser::create()
{
	auto abstractParser = std::shared_ptr<AbstractSerializerParser>(new AbstractSerializerParser());
	
	return abstractParser;
}

AbstractSerializerParser::AbstractSerializerParser()
{
	_dependencies		= Dependency::create();
}


void
AbstractSerializerParser::parse(const std::string&					filename,
								const std::string&					resolvedFilename,
								std::shared_ptr<Options>			options,
								const std::vector<unsigned char>&	data,
								AssetLibraryPtr						assetLibrary)
{
}

std::string
AbstractSerializerParser::extractDependencies(AssetLibraryPtr				assetLibrary,
									  const std::vector<unsigned char>&		data,
									  std::shared_ptr<Options>				options,
									  std::string							assetFilePath)
{
	msgpack::object			msgpackObject;
	msgpack::zone			mempool;
	std::string				str(data.begin(), data.end());
	msgpack::type::tuple<std::vector<SerializedAsset>, std::string> serilizedAssets;

	msgpack::unpack(str.data(), str.size(), NULL, &mempool, &msgpackObject);
	msgpackObject.convert(&serilizedAssets);

	// do stuff with assets

	for (uint index = 0; index < serilizedAssets.a0.size(); ++index)
		deserializedAsset(serilizedAssets.a0[index], assetLibrary, options, assetFilePath);

	return serilizedAssets.a1;
}

void
AbstractSerializerParser::deserializedAsset(SerializedAsset				asset,
											AssetLibraryPtr				assetLibrary,
											std::shared_ptr<Options>	options,
											std::string					assetFilePath)
{
	std::vector<unsigned char>	data;
	std::string					assetCompletePath	= assetFilePath + "/";
	std::string					resolvedPath		= "";

	if (asset.a0 < 10) // external
	{
		assetCompletePath += asset.a2;
		resolvedPath = asset.a2;
		auto							flags = std::ios::in | std::ios::ate | std::ios::binary;
		std::fstream					file(assetCompletePath, flags);
		if (file.is_open())
		{
			unsigned int size = (unsigned int)file.tellg();

			// FIXME: use fixed size buffers and call _progress accordingly

			data.resize(size);

			file.seekg(0, std::ios::beg);
			file.read((char*)&data[0], size);
			file.close();
		}
		else
			throw std::invalid_argument("file already open");
	}
	else
	{
		std::copy(asset.a2.begin(), asset.a2.end(), back_inserter(data));
	}

	if (asset.a0 == 0 || asset.a0 == 10) // geometry
	{
		_geometryParser->dependecy(_dependencies);
		_geometryParser->parse(assetCompletePath, resolvedPath, options, data, assetLibrary);
		_dependencies->registerReference(asset.a1, assetLibrary->geometry(_geometryParser->_lastParsedAssetName));
	}
	else if (asset.a0 == 1 || asset.a0 == 11) // material
	{
		_materialParser->dependecy(_dependencies);
		_materialParser->parse(assetCompletePath, resolvedPath, options, data, assetLibrary);
		_dependencies->registerReference(asset.a1, std::dynamic_pointer_cast<data::Provider>(assetLibrary->material(_materialParser->_lastParsedAssetName)));
	}
	else if (asset.a0 == 2 || asset.a0 == 12) // texture
	{
		if (asset.a0 == 12)
		{
			resolvedPath = std::to_string(asset.a1) + ".png";
			assetCompletePath += resolvedPath;
		}

		std::shared_ptr<file::AbstractParser> parser = assetLibrary->getParser("png");

		parser->parse(assetCompletePath, resolvedPath, options, data, assetLibrary);

		//assetLibrary->load(assetCompletePath, nullptr, nullptr, false);
		_dependencies->registerReference(asset.a1, assetLibrary->texture(assetCompletePath));
	}
	else if (asset.a0 == 3) // effect
	{
		assetLibrary->load(assetCompletePath, nullptr, nullptr, false);
		_dependencies->registerReference(asset.a1, assetLibrary->effect(assetCompletePath));
	}
}

std::string
AbstractSerializerParser::extractFolderPath(const std::string& filepath)
{
	unsigned found = filepath.find_last_of("/\\");

	return filepath.substr(0, found);
}