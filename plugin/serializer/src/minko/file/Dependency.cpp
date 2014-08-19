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
#include "minko/file/MaterialWriter.hpp"
#include "minko/file/WriterOptions.hpp"
#include "minko/material/Material.hpp"

using namespace minko;
using namespace minko::file;

std::unordered_map<uint, Dependency::GeometryTestFunc>            Dependency::_geometryTestFunctions;
std::unordered_map<uint, Dependency::GeometryWriterFunction>    Dependency::_geometryWriteFunctions;
Dependency::TextureWriterFunction    Dependency::_textureWriteFunction;
Dependency::MaterialWriterFunction    Dependency::_materialWriteFunction;

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

std::shared_ptr<render::Effect>
Dependency::getEffectReference(uint effectId)
{
    return _effectReferences[effectId];
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

Dependency::SerializedAsset
Dependency::serializeGeometry(std::shared_ptr<Dependency>                dependency,
                              std::shared_ptr<file::AssetLibrary>        assetLibrary,
                              std::shared_ptr<geometry::Geometry>        geometry,
                              uint                                        resourceId,
                              std::shared_ptr<file::Options>            options,
                              std::shared_ptr<file::WriterOptions>        writerOptions,
                              std::vector<Dependency::SerializedAsset>&    includeDependecies)
{
    GeometryWriter::Ptr         geometryWriter = GeometryWriter::create();
    serialize::AssetType        assetType;
    std::string                 content;

    geometryWriter->data(geometry);

    if (writerOptions->embedAll())
    {
        assetType = serialize::AssetType::EMBED_GEOMETRY_ASSET;

        content = geometryWriter->embedAll(assetLibrary, options, writerOptions, includeDependecies);
    }
    else
    {
        assetType = serialize::AssetType::GEOMETRY_ASSET;

        auto filename = assetLibrary->geometryName(geometry) + ".geometry";

        auto completeFilename = writerOptions->outputAssetUriFunction()(filename);

        geometryWriter->write(completeFilename, assetLibrary, options, writerOptions, includeDependecies);

        content = filename;
    }

    SerializedAsset res(assetType, resourceId, content);

    return res;
}

Dependency::SerializedAsset
Dependency::serializeTexture(std::shared_ptr<Dependency>                dependency,
                             std::shared_ptr<file::AssetLibrary>        assetLibrary,
                             std::shared_ptr<render::AbstractTexture>    texture,
                             uint                                        resourceId,
                             std::shared_ptr<file::Options>                options,
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

    if (writerOptions->embedAll())
    {
        assetType = serialize::AssetType::EMBED_TEXTURE_PACK_ASSET;

        content = writer->embedAll(assetLibrary, options, writerOptions);
    }
    else
    {
        assetType = serialize::AssetType::TEXTURE_PACK_ASSET;

        auto resolvedFileName = writerOptions->fileNameSolverUriFunction()(fileName);
        auto completeFileName = writerOptions->outputAssetUriFunction()(resolvedFileName);

        writer->write(completeFileName, assetLibrary, options, writerOptions);

        content = resolvedFileName;
    }

    auto metaByte = static_cast<unsigned char>(writer->headerSize());

    auto metaData =
        static_cast<unsigned int>(assetType) +
        static_cast<unsigned int>(metaByte << 24);

    SerializedAsset res(metaData, resourceId, content);

    return res;
}

Dependency::SerializedAsset
Dependency::serializeMaterial(std::shared_ptr<Dependency>            dependency,
                              std::shared_ptr<file::AssetLibrary>    assetLibrary,
							  std::shared_ptr<material::Material>   material,
                              uint                                    resourceId,
                              std::shared_ptr<file::Options>        options,
                              std::shared_ptr<file::WriterOptions>  writerOptions)
{
    MaterialWriter::Ptr         materialWriter = MaterialWriter::create();
    serialize::AssetType        assetType;
    std::string                 content;

    materialWriter->data(material);

    if (writerOptions->embedAll())
    {
        assetType = serialize::AssetType::EMBED_MATERIAL_ASSET;
        materialWriter->parentDependencies(dependency);
        content = materialWriter->embedAll(assetLibrary, options, writerOptions);
    }
    else
    {
        assetType = serialize::AssetType::MATERIAL_ASSET;
        materialWriter->parentDependencies(nullptr);

        auto materialName = assetLibrary->materialName(material);

        auto materialNameExtensionLocation = materialName.find_last_of(".");
        auto materialNameExtension = std::string { };

        if (materialNameExtensionLocation != std::string::npos)
            materialNameExtension = materialName.substr(materialNameExtensionLocation + 1);

        auto filename = materialName + (materialNameExtension == "material" ? "" : ".material");

        auto completeFilename = writerOptions->outputAssetUriFunction()(filename);

        materialWriter->write(completeFilename, assetLibrary, options, writerOptions);

        content = filename;
    }

    SerializedAsset res(assetType, resourceId, content);

    return res;
}

std::vector<Dependency::SerializedAsset>
Dependency::serialize(std::shared_ptr<file::AssetLibrary>       assetLibrary,
                      std::shared_ptr<file::Options>            options,
                      std::shared_ptr<file::WriterOptions>      writerOptions)
{
    std::vector<SerializedAsset> serializedAsset;

    for (const auto& itGeometry : _geometryDependencies)
    {
        uint maxPriority = 0;

        for (auto testGeomFunc : _geometryTestFunctions)
            if (testGeomFunc.second(itGeometry.first) && maxPriority < testGeomFunc.first)
                maxPriority = testGeomFunc.first;

        std::vector<SerializedAsset> includeDependencies;

        auto res = _geometryWriteFunctions[maxPriority](shared_from_this(),
                                                        assetLibrary,
                                                        itGeometry.first,
                                                        itGeometry.second,
                                                        options,
                                                        writerOptions,
                                                        includeDependencies);

        serializedAsset.push_back(res);
    }

    for (const auto& itMaterial : _materialDependencies)
    {
        auto res = _materialWriteFunction(shared_from_this(),
                                          assetLibrary,
                                          itMaterial.first,
                                          itMaterial.second,
                                          options,
                                          writerOptions);

        serializedAsset.push_back(res);
    }

    for (const auto& itTexture : _textureDependencies)
    {
        auto res = _textureWriteFunction(shared_from_this(),
                                         assetLibrary,
                                         itTexture.first,
                                         itTexture.second,
                                         options,
                                         writerOptions);

        serializedAsset.insert(serializedAsset.begin(), res);
    }

#if 0
    for (const auto& itEffect : _effectDependencies)
    {
#ifdef DEBUG
        std::string filenameInput= "bin/debug/" + assetLibrary->effectName(itEffect.first);
#else
        std::string filenameInput= assetLibrary->effectName(itEffect.first);
#endif
        std::ifstream source(filenameInput, std::ios::binary);

        SerializedAsset dependencyRes;

        copyEffectDependency(assetLibrary, options, source, itEffect.first, dependencyRes);

        serialize::AssetType    assetType;
        std::string             content;

        if (writerOptions->embedAll())
        {
            assetType = serialize::AssetType::EMBED_EFFECT_ASSET;

            serializedAsset.insert(serializedAsset.begin(), dependencyRes);

            content = std::string(std::istreambuf_iterator<char>(source),
                                  std::istreambuf_iterator<char>());
        }
        else
        {
            assetType = serialize::AssetType::EFFECT_ASSET;

            std::string filenameOutput= "";

            for (int charIndex = filenameInput.size() - 1;
                 charIndex >= 0 && filenameInput[charIndex] != '/';
                 --charIndex)
            {
                filenameOutput.insert(0, filenameInput.substr(charIndex, 1));
            }

            auto completeOutputFilename = writerOptions->outputAssetUriFunction()(filenameOutput);

            std::ofstream dst(completeOutputFilename, std::ios::binary);

            dst << source.rdbuf();

            source.close();
            dst.close();

            content = filenameOutput;
        }

        SerializedAsset res(assetType, itEffect.second, content);
        serializedAsset.insert(serializedAsset.begin(), res);
    }
#endif
    return serializedAsset;
}

void
Dependency::copyEffectDependency(std::shared_ptr<AssetLibrary>          assets,
                                 std::shared_ptr<Options>               options,
                                 const std::ifstream&                   source,
                                 std::shared_ptr<render::Effect>        effect,
                                 SerializedAsset&                       result,
                                 std::shared_ptr<WriterOptions>         writerOptions)
{
    std::stringstream   effectContent;
    std::size_t         found;

    effectContent << source.rdbuf();

    uint i = 0;

    while ((found = effectContent.str().find(".glsl", i)) != std::string::npos)
    {
        uint position = found;

        while (effectContent.str()[position - 1] != '\'')
            position--;

        std::string dependencyFile = effectContent.str().substr(position, found + 5 - position);

        std::cout << dependencyFile << std::endl;

#ifdef DEBUG
        std::ifstream dependencySource("bin/debug/effect/" + dependencyFile, std::ios::binary);
#else
        std::ifstream dependencySource("effect/" + dependencyFile, std::ios::binary);
#endif
        if (writerOptions->embedAll())
        {
            // TODO
            // see how effect dep are processed
        }
        else
        {
            dependencyFile = writerOptions->outputAssetUriFunction()(dependencyFile);

            std::ofstream dst(dependencyFile, std::ios::binary);

            dst << dependencySource.rdbuf();

            dependencySource.close();
            dst.close();
        }

        i = (found + 6);
    }
}
