/*
Copyright (c) 2014 Aerys

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
#include "minko/file/TextureParser.hpp"
#include "minko/file/TextureWriter.hpp"
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
                                            std::list<std::shared_ptr<component::JobManager::Job>>&)>> AbstractSerializerParser::_assetTypeToFunction =
{
    { serialize::AssetType::TEXTURE_PACK_ASSET, std::bind(deserializeTexture, 
                                                          std::placeholders::_1,
                                                          std::placeholders::_2,
                                                          std::placeholders::_3,
                                                          std::placeholders::_4,
                                                          std::placeholders::_5,
                                                          std::placeholders::_6) }
};

void
AbstractSerializerParser::registerAssetFunction(uint assetTypeId, AssetDeserializeFunction f)
{
	_assetTypeToFunction[assetTypeId] = f;
}

AbstractSerializerParser::AbstractSerializerParser()
{
	_dependencies = Dependency::create();
}


void
AbstractSerializerParser::parse(const std::string&					filename,
								const std::string&					resolvedFilename,
								std::shared_ptr<Options>			options,
								const std::vector<unsigned char>&	data,
								AssetLibraryPtr						assetLibrary)
{
}

void
AbstractSerializerParser::extractDependencies(AssetLibraryPtr						assetLibrary,
											  const std::vector<unsigned char>&		data,
											  short									dataOffset,
											  unsigned int							dependenciesSize,
											  std::shared_ptr<Options>				options,
											  std::string&							assetFilePath)
{
	msgpack::object							msgpackObject;
	msgpack::zone							mempool;
	SerializedAsset							serializedAsset;

	auto nbDependencies = readShort(data, dataOffset);

	unsigned int offset = dataOffset + 2;

	for (int index = 0; index < nbDependencies; ++index)
	{
		if (offset >(dataOffset + dependenciesSize))
        {
            _error->execute(shared_from_this(), Error("DependencyParsingError", "Error while parsing dependencies"));
            return;
        }

		auto assetSize = readUInt(data, offset);

		offset += 4;

		msgpack::unpack((char*)&data[offset], assetSize, NULL, &mempool, &msgpackObject);
		msgpackObject.convert(&serializedAsset);

		deserializeAsset(serializedAsset, assetLibrary, options, assetFilePath);

		offset += assetSize;
	}
}

void
AbstractSerializerParser::deserializeAsset(SerializedAsset&				asset,
											AssetLibraryPtr				assetLibrary,
											std::shared_ptr<Options>	options,
											std::string&				assetFilePath)
{
	std::vector<unsigned char>	data;
	std::string					assetCompletePath	= assetFilePath + "/";
	std::string					resolvedPath		= "";
	unsigned char				metaByte			= (asset.a0 & 0xFF000000) >> 24;

	asset.a0 = asset.a0 & 0x00FF;

    if (asset.a0 < 10)
    {
		assetCompletePath += asset.a2;
		resolvedPath = asset.a2;
    }

	if (asset.a0 < 10 && _assetTypeToFunction.find(asset.a0) == _assetTypeToFunction.end()) // external
	{
		auto protocolFunction = options->protocolFunction();
		auto protocol = protocolFunction(assetCompletePath);

		auto fileOptions = Options::create(options);
		fileOptions->loadAsynchronously(false);

        auto fileSuccessfullyLoaded = true;
		auto errorSlot = protocol->error()->connect([&](AbstractProtocol::Ptr)
		{
			switch (asset.a0)
			{
			case serialize::AssetType::GEOMETRY_ASSET:
                _error->execute(shared_from_this(), Error("MissingGeometryDependency", "Missing geometry dependency: '" + assetCompletePath + "'"));
                break;

			case serialize::AssetType::MATERIAL_ASSET:
                _error->execute(shared_from_this(), Error("MissingMaterialDependency", "Missing material dependency: '" + assetCompletePath + "'"));
                break;

			case serialize::AssetType::TEXTURE_ASSET:
                _error->execute(shared_from_this(), Error("MissingTextureDependency", "Missing texture dependency: '" + assetCompletePath + "'"));
                break;

			case serialize::AssetType::EFFECT_ASSET:
                _error->execute(shared_from_this(), Error("MissingEffectDependency", "Missing effect dependency: '" + assetCompletePath + "'"));
                break;

			default:
				break;
			}
            
            fileSuccessfullyLoaded = false;
		});

		auto completeSlot = protocol->complete()->connect([&](AbstractProtocol::Ptr p)
		{
			data.assign(p->file()->data().begin(), p->file()->data().end());
		});

		protocol->load(assetCompletePath, fileOptions);
        
        if (!fileSuccessfullyLoaded)
            return;
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
		_dependencies->registerReference(asset.a1, assetLibrary->material(_materialParser->_lastParsedAssetName));
		_jobList.splice(_jobList.end(), _materialParser->_jobList);
	}
    else if ((asset.a0 == serialize::AssetType::EMBED_TEXTURE_ASSET ||
        asset.a0 == serialize::AssetType::TEXTURE_ASSET) &&
			(_dependencies->textureReferenceExist(asset.a1) == false || _dependencies->getTextureReference(asset.a1) == nullptr)) // texture
	{
		if (asset.a0 == serialize::AssetType::EMBED_TEXTURE_ASSET)
		{
            auto imageFormat = static_cast<serialize::ImageFormat>(metaByte);

            auto extension = serialize::extensionFromImageFormat(imageFormat);

			resolvedPath = std::to_string(asset.a1) + "." + extension;
			assetCompletePath += resolvedPath;
		}

			auto extension = resolvedPath.substr(resolvedPath.find_last_of(".") + 1);

			std::shared_ptr<file::AbstractParser> parser = assetLibrary->loader()->options()->getParser(extension);

        static auto nameId = 0;
        auto uniqueName = resolvedPath;

        while (assetLibrary->texture(uniqueName) != nullptr)
            uniqueName = "texture" + std::to_string(nameId++);

        parser->parse(uniqueName, assetCompletePath, options, data, assetLibrary);

        auto texture = assetLibrary->texture(uniqueName);

        if (options->disposeTextureAfterLoading())
            texture->disposeData();

        _dependencies->registerReference(asset.a1, texture);
    }
    else if (asset.a0 == serialize::AssetType::EMBED_TEXTURE_PACK_ASSET &&
             (_dependencies->textureReferenceExist(asset.a1) == false ||
             _dependencies->getTextureReference(asset.a1) == nullptr))
    {
        resolvedPath = "texture_" + std::to_string(asset.a1);

        if (assetLibrary->texture(resolvedPath) == nullptr)
        {
            const auto headerSize = static_cast<int>(metaByte);

            _textureParser->textureHeaderSize(headerSize);
            _textureParser->dataEmbed(true);

            _textureParser->parse(resolvedPath, assetCompletePath, options, data, assetLibrary);

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

bool
AbstractSerializerParser::readHeader(const std::string&					filename,
                                     const std::vector<unsigned char>&     data,
                                     int                                   extension)
{
	_magicNumber = readInt(data, 0);

	// File should start with 0x4D4B03 (MK3). Last byte reserved for extensions (Material, Geometry...)
    if (_magicNumber != MINKO_SCENE_MAGIC_NUMBER + (extension & 0xFF))
    {
        _error->execute(shared_from_this(), Error("InvalidFile", "Invalid scene file '" + filename + "': magic number mismatch"));
        return false;
    }
    
	_version.version = readInt(data, 4);

    _version.major = int(data[4]);
    _version.minor = readShort(data, 5);
    _version.patch = int(data[7]);

    if (_version.major != MINKO_SCENE_VERSION_MAJOR || 
        _version.minor > MINKO_SCENE_VERSION_MINOR || 
        (_version.minor <= MINKO_SCENE_VERSION_MINOR && _version.patch < MINKO_SCENE_VERSION_PATCH))
	{
		auto fileVersion = std::to_string(_version.major) + "." + std::to_string(_version.minor) + "." + std::to_string(_version.patch);
		auto sceneVersion = std::to_string(MINKO_SCENE_VERSION_MAJOR) + "." + std::to_string(MINKO_SCENE_VERSION_MINOR) + "." + std::to_string(MINKO_SCENE_VERSION_PATCH);

        auto message = "File " + filename + " doesn't match serializer version (file has v" + fileVersion + " while current version is v" + sceneVersion + ")";

        std::cerr << message << std::endl;
        
        _error->execute(shared_from_this(), Error("InvalidFile", message));
        return false;
	}

	// Versions with the same MAJOR value but different MINOR or PATCH value should be compatible
#if DEBUG
    if (_version.minor != MINKO_SCENE_VERSION_MINOR || _version.patch != MINKO_SCENE_VERSION_PATCH)
	{
		auto fileVersion = std::to_string(_version.major) + "." + std::to_string(_version.minor) + "." + std::to_string(_version.patch);
		auto sceneVersion = std::to_string(MINKO_SCENE_VERSION_MAJOR) + "." + std::to_string(MINKO_SCENE_VERSION_MINOR) + "." + std::to_string(MINKO_SCENE_VERSION_PATCH);

		std::cout << "Warning: file " + filename + " is v" + fileVersion + " while current version is v" + sceneVersion << std::endl;
	}
#endif

	_fileSize = readUInt(data, 8);

	_headerSize = readShort(data, 12);

	_dependenciesSize = readUInt(data, 14);
	_sceneDataSize = readUInt(data, 18);

    return true;
}

void
AbstractSerializerParser::deserializeTexture(unsigned char      metaByte,
                                             AssetLibraryPtr    assetLibrary,
                                             std::string&       assetCompletePath,
                                             DependencyPtr      dependency,
                                             short              assetId,
                                             std::list<JobPtr>& jobs)
{
    if (assetLibrary->texture(assetCompletePath) != nullptr)
        return;

    auto assetHeaderSize = MINKO_SCENE_HEADER_SIZE + 2;
    auto textureHeaderSize = static_cast<unsigned int>(metaByte);

    auto options = Options::create(assetLibrary->loader()->options());

    options
        ->seekingOffset(0)
        ->seekedLength(assetHeaderSize + textureHeaderSize)
        ->parserFunction([&](const std::string& extension) -> AbstractParser::Ptr
    {
        if (extension != std::string("texture"))
            return nullptr;

        auto textureParser = TextureParser::create();

        textureParser->textureHeaderSize(textureHeaderSize);
        textureParser->dataEmbed(false);

        return textureParser;
    });

    auto textureLoader = Loader::create();
    textureLoader->options(options);

    auto texture = render::AbstractTexture::Ptr();

    auto loaderCompleteSlot = textureLoader->complete()->connect([&](Loader::Ptr loader)
    {
        texture = assetLibrary->texture(assetCompletePath);
    });

    textureLoader
        ->queue(assetCompletePath)
        ->load();

    if (options->disposeTextureAfterLoading())
        texture->disposeData();

    dependency->registerReference(assetId, texture);
}
