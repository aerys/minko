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
#include "minko/Types.hpp"
#include "minko/render/Texture.hpp"


using namespace minko;
using namespace minko::file;

std::unordered_map<uint, std::function<void(unsigned char,
											AbstractSerializerParser::AssetLibraryPtr,
											std::string&,
											std::shared_ptr<Dependency>,
											short,
											std::list<std::shared_ptr<component::JobManager::Job>>&)>> AbstractSerializerParser::_assetTypeToFunction;

void
AbstractSerializerParser::registerAssetFunction(uint assetTypeId, AssetDeserializeFunction f)
{
	_assetTypeToFunction[assetTypeId] = f;
}

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
AbstractSerializerParser::extractDependencies(AssetLibraryPtr						assetLibrary,
											  const std::vector<unsigned char>&		data,
											  std::shared_ptr<Options>				options,
											  std::string&							assetFilePath)
{
	msgpack::object			msgpackObject;
	msgpack::zone			mempool;
	msgpack::type::tuple<std::vector<SerializedAsset>, std::string> serilizedAssets;

	msgpack::unpack((char*)&data[0], data.size(), NULL, &mempool, &msgpackObject);
	msgpackObject.convert(&serilizedAssets);

	std::vector<unsigned char>* d = (std::vector<unsigned char>*)&data;
	d->clear();
	d->shrink_to_fit();

	for (uint index = 0; index < serilizedAssets.a0.size(); ++index)
		deserializedAsset(serilizedAssets.a0[index], assetLibrary, options, assetFilePath);

	return serilizedAssets.a1;
}

void
AbstractSerializerParser::deserializedAsset(SerializedAsset&			asset,
											AssetLibraryPtr				assetLibrary,
											std::shared_ptr<Options>	options,
											std::string&				assetFilePath)
{
	std::vector<unsigned char>	data;
	std::string					assetCompletePath	= assetFilePath + "/";
	std::string					resolvedPath		= "";
	unsigned char				metaByte			= (asset.a0 & 0xFF00) >> 8;

	asset.a0 = asset.a0 & 0x00FF;

	assetCompletePath += asset.a2;
	resolvedPath = asset.a2;

	if (asset.a0 < 10 && _assetTypeToFunction.find(asset.a0) == _assetTypeToFunction.end()) // external
	{
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
        {
            auto filePath = assetCompletePath;

            switch (asset.a0)
            {
            case serialize::AssetType::GEOMETRY_ASSET:
                throw ParserError("MissingGeometryDependency", "Missing geometry dependency: '" + filePath + "'");

            case serialize::AssetType::MATERIAL_ASSET:
                throw ParserError("MissingMaterialDependency", "Missing material dependency: '" + filePath + "'");

            case serialize::AssetType::TEXTURE_ASSET:
                throw ParserError("MissingTextureDependency", "Missing texture dependency: '" + filePath + "'");

            case serialize::AssetType::EFFECT_ASSET:
                throw ParserError("MissingEffectDependency", "Missing effect dependency: '" + filePath + "'");

            default:
                break;
            }
        }
	}
	else
		data.assign(asset.a2.begin(), asset.a2.end());

	if ((asset.a0 == serialize::AssetType::GEOMETRY_ASSET || asset.a0 == serialize::AssetType::EMBED_GEOMETRY_ASSET) &&
		_dependencies->geometryReferenceExist(asset.a1) == false) // geometry
	{
        _geometryParser->_jobList.clear();
		_geometryParser->dependecy(_dependencies);

		if (asset.a0 == serialize::AssetType::EMBED_GEOMETRY_ASSET)
			resolvedPath = "geometry_" + std::to_string(asset.a1);

		_geometryParser->parse(resolvedPath, assetCompletePath, options, data, assetLibrary);
		_dependencies->registerReference(asset.a1, assetLibrary->geometry(_geometryParser->_lastParsedAssetName));
		_jobList.splice(_jobList.end(), _geometryParser->_jobList);
	}
	else if ((asset.a0 == serialize::AssetType::MATERIAL_ASSET || asset.a0 == serialize::AssetType::EMBED_MATERIAL_ASSET) &&
		_dependencies->materialReferenceExist(asset.a1) == false) // material
	{
		_materialParser->_jobList.clear();
		_materialParser->dependecy(_dependencies);

		if (asset.a0 == serialize::AssetType::EMBED_MATERIAL_ASSET)
			resolvedPath = "material_" + std::to_string(asset.a1);

		_materialParser->parse(resolvedPath, assetCompletePath, options, data, assetLibrary);
		_dependencies->registerReference(asset.a1, std::dynamic_pointer_cast<data::Provider>(assetLibrary->material(_materialParser->_lastParsedAssetName)));
		_jobList.splice(_jobList.end(), _materialParser->_jobList);
	}
	else if ((asset.a0 == serialize::AssetType::TEXTURE_ASSET ||
			  asset.a0 == serialize::AssetType::EMBED_TEXTURE_ASSET) &&
			(_dependencies->textureReferenceExist(asset.a1) == false || _dependencies->getTextureReference(asset.a1) == nullptr)) // texture
	{
		if (asset.a0 == serialize::AssetType::EMBED_TEXTURE_ASSET)
		{
            auto extension = ".jpg";
			resolvedPath = std::to_string(asset.a1) + extension;
			assetCompletePath += resolvedPath;
		}

		if (assetLibrary->texture(resolvedPath) == nullptr)
		{
			auto extension = resolvedPath.substr(resolvedPath.find_last_of(".") + 1);

			std::shared_ptr<file::AbstractParser> parser = assetLibrary->loader()->options()->getParser(extension);

			parser->parse(resolvedPath, assetCompletePath, options, data, assetLibrary);

        	auto texture = assetLibrary->texture(resolvedPath);

        	if (options->disposeTextureAfterLoading())
        	    texture->disposeData();
		}
		_dependencies->registerReference(asset.a1, assetLibrary->texture(resolvedPath));
	}
	else if (asset.a0 == serialize::AssetType::EFFECT_ASSET && _dependencies->effectReferenceExist(asset.a1) == false) // effect
	{
		assetLibrary->loader()->queue(assetCompletePath);
		_dependencies->registerReference(asset.a1, assetLibrary->effect(assetCompletePath));
	}
	else
	{
		if (_assetTypeToFunction.find(asset.a0) != _assetTypeToFunction.end())
			_assetTypeToFunction[asset.a0](metaByte, assetLibrary, assetCompletePath, _dependencies, asset.a1, _jobList);
	}

	data.clear();
	data.shrink_to_fit();

	asset.a2.clear();
	asset.a2.shrink_to_fit();
}

std::string
AbstractSerializerParser::extractFolderPath(const std::string& filepath)
{
	unsigned found = filepath.find_last_of("/\\");

	return filepath.substr(0, found);
}
