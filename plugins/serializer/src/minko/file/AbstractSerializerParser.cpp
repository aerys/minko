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
						std::shared_ptr<AssetLibrary>		assetLibrary)
{
}

std::string
AbstractSerializerParser::extractDependencies(std::shared_ptr<AssetLibrary>		assetLibrary,
									  const std::vector<unsigned char>&			data,
									  std::shared_ptr<Options>					options)
{
	msgpack::object			msgpackObject;
	msgpack::zone			mempool;
	std::string				str(data.begin(), data.end());
	msgpack::type::tuple<std::vector<msgpack::type::tuple<unsigned char, short, std::string>>, std::string> serilizedAssets;

	msgpack::unpack(str.data(), str.size(), NULL, &mempool, &msgpackObject);
	msgpackObject.convert(&serilizedAssets);

	// do stuff with assets

	for (uint index = 0; index < serilizedAssets.a0.size(); ++index)
		deserializedAsset(serilizedAssets.a0[index], assetLibrary, options);

	return serilizedAssets.a1;
}

void
AbstractSerializerParser::deserializedAsset(msgpack::type::tuple<unsigned char, short, std::string>	asset,
							  std::shared_ptr<AssetLibrary>									assetLibrary,
							  std::shared_ptr<Options>										options)
{
	if (asset.a0 <= 3) // external
	{
		auto							flags = std::ios::in | std::ios::ate | std::ios::binary;
		std::fstream					file(asset.a2, flags);
		std::vector<unsigned char>      data;
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

		if (asset.a0 == 0) // geometry
		{
			_geometryParser->dependecy(_dependencies);
			_geometryParser->parse(asset.a2, asset.a2, options, data, assetLibrary);
			_dependencies->registerReference(asset.a1, assetLibrary->geometry(_geometryParser->_lastParsedAssetName));
		}
		else if (asset.a0 == 1) // material
		{
			_materialParser->dependecy(_dependencies);
			_materialParser->parse(asset.a2, asset.a2, options, data, assetLibrary);
			_dependencies->registerReference(asset.a1, std::dynamic_pointer_cast<data::Provider>(assetLibrary->material(_materialParser->_lastParsedAssetName)));
		}
		else if (asset.a0 == 2) // texture
		{
			assetLibrary->load(asset.a2);
			_dependencies->registerReference(asset.a1, assetLibrary->texture(asset.a2));
		}
		else if (asset.a0 == 3) // effect
		{
			assetLibrary->load(asset.a2);
			_dependencies->registerReference(asset.a1, assetLibrary->effect(asset.a2));
		}
	}
}