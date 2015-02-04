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

#include "minko/Types.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/file/GeometryWriter.hpp"
#include "minko/file/TextureWriter.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/file/LinkedAsset.hpp"
#include "minko/file/MaterialWriter.hpp"
#include "minko/file/WriterOptions.hpp"
#include "minko/material/Material.hpp"
#include "minko/serialize/TypeSerializer.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::serialize;

std::unordered_map<uint, Dependency::GeometryTestFunc>			Dependency::_geometryTestFunctions;
std::unordered_map<uint, Dependency::GeometryWriterFunction>	Dependency::_geometryWriteFunctions;
Dependency::TextureWriterFunction	Dependency::_textureWriteFunction;
Dependency::MaterialWriterFunction	Dependency::_materialWriteFunction;

Dependency::Dependency()
{
	_currentId = 0;

	setGeometryFunction(std::bind(&Dependency::serializeGeometry,
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3,
		std::placeholders::_4,
		std::placeholders::_5,
		std::placeholders::_6,
		std::placeholders::_7),
		[=](std::shared_ptr<geometry::Geometry> geometry) -> bool
			{
				return true;
			},
		0);

    if (_textureWriteFunction == nullptr)
    {
        _textureWriteFunction = std::bind(&Dependency::serializeTexture,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4,
            std::placeholders::_5,
            std::placeholders::_6);
    }

    if (_materialWriteFunction == nullptr)
    {
		_materialWriteFunction = std::bind(&Dependency::serializeMaterial,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4,
            std::placeholders::_5,
            std::placeholders::_6);
	}
}

bool
Dependency::hasDependency(std::shared_ptr<render::Effect> effect)
{
	return _effectDependencies.find(effect) != _effectDependencies.end();
}

uint
Dependency::registerDependency(std::shared_ptr<render::Effect> effect)
{
	if (!hasDependency(effect))
		_effectDependencies[effect] = _currentId++;

	return _effectDependencies[effect];
}

bool
Dependency::hasDependency(std::shared_ptr<geometry::Geometry> geometry)
{
	return _geometryDependencies.find(geometry) != _geometryDependencies.end();
}

uint
Dependency::registerDependency(std::shared_ptr<geometry::Geometry> geometry)
{
	if (!hasDependency(geometry))
		_geometryDependencies[geometry] = _currentId++;

	return _geometryDependencies[geometry];
}

bool
Dependency::hasDependency(std::shared_ptr<material::Material> material)
{
	return _materialDependencies.find(material) != _materialDependencies.end();
}

uint
Dependency::registerDependency(std::shared_ptr<material::Material> material)
{
	if (!hasDependency(material))
		_materialDependencies[material] = _currentId++;

	return _materialDependencies[material];
}

bool
Dependency::hasDependency(AbsTexturePtr texture)
{
	return _textureDependencies.find(texture) != _textureDependencies.end();
}

uint
Dependency::registerDependency(AbsTexturePtr texture)
{
	if (!hasDependency(texture))
		_textureDependencies[texture] = _currentId++;

	return _textureDependencies[texture];
}

bool
Dependency::hasDependency(std::shared_ptr<scene::Node> subScene)
{
	return _subSceneDependencies.find(subScene) != _subSceneDependencies.end();
}

uint
Dependency::registerDependency(std::shared_ptr<scene::Node> subScene)
{
	if (!hasDependency(subScene))
		_subSceneDependencies[subScene] = _currentId++;

	return _subSceneDependencies[subScene];
}

bool
Dependency::hasDependency(std::shared_ptr<LinkedAsset> linkedAsset)
{
	return _linkedAssetDependencies.find(linkedAsset) != _linkedAssetDependencies.end();
}

uint
Dependency::registerDependency(std::shared_ptr<LinkedAsset> linkedAsset)
{
	if (!hasDependency(linkedAsset))
		_linkedAssetDependencies[linkedAsset] = _currentId++;

	return _linkedAssetDependencies[linkedAsset];
}

std::shared_ptr<geometry::Geometry>
Dependency::getGeometryReference(uint geometryId)
{
	return _geometryReferences[geometryId];
}

void
Dependency::registerReference(uint referenceId, std::shared_ptr<geometry::Geometry> geometry)
{
	_geometryReferences[referenceId] = geometry;
}

std::shared_ptr<material::Material>
Dependency::getMaterialReference(uint materialId)
{
	return _materialReferences[materialId];
}

void
Dependency::registerReference(uint referenceId, std::shared_ptr<material::Material> material)
{
	_materialReferences[referenceId] = material;
}

Dependency::AbsTexturePtr
Dependency::getTextureReference(uint textureId)
{
	return _textureReferences[textureId];
}

void
Dependency::registerReference(uint referenceId, AbsTexturePtr texture)
{
	_textureReferences[referenceId] = texture;
}

std::shared_ptr<scene::Node>
Dependency::getSubsceneReference(uint subSceneId)
{
	return _subSceneReferences[subSceneId];
}

void
Dependency::registerReference(uint referenceId, std::shared_ptr<scene::Node> subScene)
{
	_subSceneReferences[referenceId] = subScene;
}

void
Dependency::registerReference(uint referenceId, std::shared_ptr<render::Effect> effect)
{
	_effectReferences[referenceId] = effect;
}

void
Dependency::registerReference(uint referenceId, std::shared_ptr<LinkedAsset> linkedAsset)
{
	_linkedAssetReferences[referenceId] = linkedAsset;
}

std::shared_ptr<render::Effect>
Dependency::getEffectReference(uint effectId)
{
	return _effectReferences[effectId];
}

std::shared_ptr<LinkedAsset>
Dependency::getLinkedAssetReference(uint referenceId)
{
	return _linkedAssetReferences[referenceId];
}

bool
Dependency::geometryReferenceExist(uint referenceId)
{
	return _geometryReferences.find(referenceId) != _geometryReferences.end();
}

bool
Dependency::textureReferenceExist(uint referenceId)
{
	return _textureReferences.find(referenceId) != _textureReferences.end();
}

bool
Dependency::materialReferenceExist(uint referenceId)
{
	return _materialReferences.find(referenceId) != _materialReferences.end();
}

bool
Dependency::effectReferenceExist(uint referenceId)
{
	return _effectReferences.find(referenceId) != _effectReferences.end();
}

bool
Dependency::linkedAssetReferenceExist(uint referenceId)
{
	return _linkedAssetReferences.find(referenceId) != _linkedAssetReferences.end();
}

Dependency::SerializedAsset
Dependency::serializeGeometry(std::shared_ptr<Dependency>				dependency,
							  std::shared_ptr<file::AssetLibrary>		assetLibrary,
							  std::shared_ptr<geometry::Geometry>		geometry,
							  uint										resourceId,
							  std::shared_ptr<file::Options>			options,
                              std::shared_ptr<file::WriterOptions>		writerOptions,
							  std::vector<Dependency::SerializedAsset>&	userDefinedDependency)
{
	GeometryWriter::Ptr         geometryWriter = GeometryWriter::create();
    serialize::AssetType        assetType;
    std::string                 content;

    geometryWriter->data(geometry);

    if (writerOptions->embedMode() & WriterOptions::EmbedMode::Geometry)
    {
        assetType = serialize::AssetType::EMBED_GEOMETRY_ASSET;

		content = geometryWriter->embedAll(assetLibrary, options, writerOptions, dependency, userDefinedDependency);
    }
    else
    {
        assetType = serialize::AssetType::GEOMETRY_ASSET;

        static auto geometryNameId = 0;
        auto filename = std::string();

        try
        {
            filename = assetLibrary->geometryName(geometry) + ".geometry";
        }
        catch (const std::exception& exception)
        {
            filename = "geometry" + std::to_string(geometryNameId++) + ".geometry";
        }

        auto completeFilename = writerOptions->outputAssetUriFunction()(filename);

		geometryWriter->write(completeFilename, assetLibrary, options, writerOptions, dependency, userDefinedDependency);

        content = filename;
    }

    SerializedAsset res(assetType, resourceId, content);

	return res;
}

Dependency::SerializedAsset
Dependency::serializeTexture(std::shared_ptr<Dependency>				dependency,
						     std::shared_ptr<file::AssetLibrary>		assetLibrary,
							 std::shared_ptr<render::AbstractTexture>	texture,
							 uint										resourceId,
							 std::shared_ptr<file::Options>				options,
                             std::shared_ptr<file::WriterOptions>       writerOptions)
{
    static const auto textureExtension = "texture";

	auto writer                 = TextureWriter::create();
    auto fileName = assetLibrary->textureName(texture);
    auto assetType              = serialize::AssetType();
    auto content                = std::string();

    auto extension = fileName.substr(fileName.find_last_of(".") + 1);
    if (extension != textureExtension)
        fileName = fileName.substr(0, fileName.size() - (extension.size())) + std::string(textureExtension);

    writer->data(texture);

    if (writerOptions->embedMode() & WriterOptions::EmbedMode::Texture)
    {
        assetType = serialize::AssetType::EMBED_TEXTURE_PACK_ASSET;

        content = writer->embedAll(assetLibrary, options, writerOptions, dependency);
    }
    else
    {
        assetType = serialize::AssetType::TEXTURE_PACK_ASSET;

        auto completeFileName = writerOptions->outputAssetUriFunction()(fileName);

        // TODO fixme
        // differentiate asset location relatively to scene root from asset name
        auto assetName = fileName.substr(fileName.find_last_of("/\\") + 1);

        writer->write(completeFileName, assetLibrary, options, writerOptions, dependency);

        content = assetName;
    }

    auto metaData = static_cast<unsigned short>(writer->headerSize());

    auto fullMetaData =
        static_cast<unsigned int>(assetType) +
        static_cast<unsigned int>(metaData << 16);

    SerializedAsset res(fullMetaData, resourceId, content);

	return res;
}

Dependency::SerializedAsset
Dependency::serializeMaterial(std::shared_ptr<Dependency>			dependency,
							  std::shared_ptr<file::AssetLibrary>	assetLibrary,
							  std::shared_ptr<material::Material>   material,
							  uint									resourceId,
							  std::shared_ptr<file::Options>		options,
                              std::shared_ptr<file::WriterOptions>  writerOptions)
{
	MaterialWriter::Ptr         materialWriter = MaterialWriter::create();
    serialize::AssetType        assetType;
    std::string                 content;

    materialWriter->data(material);

    if (writerOptions->embedMode() & WriterOptions::EmbedMode::Material)
    {
        assetType = serialize::AssetType::EMBED_MATERIAL_ASSET;
        content = materialWriter->embedAll(assetLibrary, options, writerOptions, dependency);
    }
    else
    {
        assetType = serialize::AssetType::MATERIAL_ASSET;

        static auto materialNameId = 0;
        auto materialName = std::string();

        try
        {
            materialName = assetLibrary->materialName(material) + ".material";
        }
        catch (std::exception exception)
        {
            materialName = "default" + std::to_string(materialNameId++) + ".material";
        }

        auto materialNameExtensionLocation = materialName.find_last_of(".");
        auto materialNameExtension = std::string { };

        if (materialNameExtensionLocation != std::string::npos)
            materialNameExtension = materialName.substr(materialNameExtensionLocation + 1);

        auto filename = materialName + (materialNameExtension == "material" ? "" : ".material");

        auto completeFilename = writerOptions->outputAssetUriFunction()(filename);

        materialWriter->write(completeFilename, assetLibrary, options, writerOptions, dependency);

        content = filename;
    }

    SerializedAsset res(assetType, resourceId, content);

	return res;
}

std::vector<Dependency::SerializedAsset>
Dependency::serialize(const std::string&                        parentFilename,
                      std::shared_ptr<file::AssetLibrary>       assetLibrary,
					  std::shared_ptr<file::Options>            options,
                      std::shared_ptr<file::WriterOptions>      writerOptions,
                      std::vector<unsigned char>&               internalLinkedAssetData)
{
	std::vector<SerializedAsset> serializedAsset;

    for (const auto& itGeometry : _geometryDependencies)
	{
		uint maxPriority = 0;

		for (auto testGeomFunc : _geometryTestFunctions)
			if (testGeomFunc.second(itGeometry.first) && maxPriority < testGeomFunc.first)
				maxPriority = testGeomFunc.first;

		std::vector<SerializedAsset> includeDependencies;

		auto res = _geometryWriteFunctions[maxPriority](
            shared_from_this(),
			assetLibrary,
			itGeometry.first,
			itGeometry.second,
			options,
			writerOptions,
			includeDependencies
        );

		serializedAsset.push_back(res);
	}

    for (const auto& itMaterial : _materialDependencies)
	{
		auto res = _materialWriteFunction(
            shared_from_this(),
            assetLibrary,
            itMaterial.first,
            itMaterial.second,
            options,
            writerOptions
        );

		serializedAsset.push_back(res);
	}

    for (const auto& itTexture : _textureDependencies)
	{
		auto res = _textureWriteFunction(
            shared_from_this(),
            assetLibrary,
            itTexture.first,
            itTexture.second,
            options,
            writerOptions
        );

		serializedAsset.insert(serializedAsset.begin(), res);
	}

    auto internalLinkedAssetDataOffset = internalLinkedAssetData.size();

    for (const auto& linkedAssetToIdPair : _linkedAssetDependencies)
    {
        const auto& linkedAsset = *linkedAssetToIdPair.first;
        const auto id = linkedAssetToIdPair.second;

        msgpack::type::tuple<int, int, std::string, std::vector<unsigned char>, int> linkedAssetData(
            linkedAsset.offset(),
            linkedAsset.length(),
            linkedAsset.filename(),
            {},
            static_cast<int>(linkedAsset.linkType())
        );

        switch (linkedAsset.linkType())
        {
        case LinkedAsset::LinkType::Copy:
            linkedAssetData.a3 = linkedAsset.data();
            break;

        case LinkedAsset::LinkType::Internal:
        {
            linkedAssetData.a0 = internalLinkedAssetDataOffset;

            const auto validFilename = File::removePrefixPathFromFilename(parentFilename);
            linkedAssetData.a2 = validFilename;

            internalLinkedAssetData.resize(internalLinkedAssetData.size() + linkedAsset.length());

            std::copy(
                linkedAsset.data().begin(),
                linkedAsset.data().begin() + linkedAsset.length(),
                internalLinkedAssetData.begin() + internalLinkedAssetDataOffset
            );

            internalLinkedAssetDataOffset += linkedAsset.length();
            break;
        }

        case LinkedAsset::LinkType::External:
            break;

        default:
            break;
        }

        std::stringstream linkedAssetSerializedData;
        msgpack::pack(linkedAssetSerializedData, linkedAssetData);

        const auto serializedLinkedAsset = SerializedAsset(
            LINKED_ASSET,
            id,
            linkedAssetSerializedData.str()
        );

        serializedAsset.insert(serializedAsset.begin(), serializedLinkedAsset);
    }

    return serializedAsset;
}
