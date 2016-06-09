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

#include "minko/file/MaterialParser.hpp"
#include "minko/material/Material.hpp"
#include "minko/Types.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/Any.hpp"
#include "minko/data/Provider.hpp"
#include "minko/render/Texture.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/render/SamplerStates.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/file/Options.hpp"
#include "minko/render/Priority.hpp"
#include "minko/render/States.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::deserialize;
using namespace minko::serialize;
using namespace minko::render;

std::map<uint, std::function<Any(std::tuple<uint, std::string&>&)>> MaterialParser::_typeIdToReadFunction;

MaterialParser::MaterialParser()
{
    _typeIdToReadFunction[VECTOR4]          = std::bind(&deserialize::TypeDeserializer::deserializeVector4,            std::placeholders::_1);
    _typeIdToReadFunction[MATRIX4X4]        = std::bind(&deserialize::TypeDeserializer::deserializeMatrix4x4,        std::placeholders::_1);
    _typeIdToReadFunction[VECTOR3]          = std::bind(&deserialize::TypeDeserializer::deserializeVector3,            std::placeholders::_1);
    _typeIdToReadFunction[VECTOR2]          = std::bind(&deserialize::TypeDeserializer::deserializeVector2,            std::placeholders::_1);
    _typeIdToReadFunction[BLENDING]         = std::bind(&deserialize::TypeDeserializer::deserializeBlending,        std::placeholders::_1);
    _typeIdToReadFunction[TRIANGLECULLING] = std::bind(&deserialize::TypeDeserializer::deserializeTriangleCulling, std::placeholders::_1);
    _typeIdToReadFunction[STRING] = std::bind(&deserialize::TypeDeserializer::deserializeString, std::placeholders::_1);
}

void
MaterialParser::parse(const std::string&                filename,
                      const std::string&                resolvedFilename,
                      Options::Ptr                      options,
                      const std::vector<unsigned char>& data,
                      AssetLibraryPtr                   assetLibrary)
{
    if (!readHeader(filename, data, 0x4D))
        return;

	std::string 		folderpath = extractFolderPath(resolvedFilename);
    SerializedMaterial  serializedMaterial;

    extractDependencies(assetLibrary, data, _headerSize, _dependencySize, options, folderpath);

    unpack(serializedMaterial, data, _sceneDataSize, _headerSize + _dependencySize);

    std::vector<ComplexProperty>    complexProperties   = serializedMaterial.get<0>();
    std::vector<BasicProperty>      basicProperties     = serializedMaterial.get<1>();

	MaterialPtr material = options->material() ? material::Material::create(options->material()) : material::Material::create();

    for (auto serializedComplexProperty : complexProperties)
        deserializeComplexProperty(material, serializedComplexProperty);

    for (auto serializedBasicProperty : basicProperties)
        deserializeBasicProperty(material, serializedBasicProperty);

	material = options->materialFunction()(material->name(), material);

    static auto nameId = 0;
    auto uniqueName = material->name();

    while (assetLibrary->material(uniqueName) != nullptr)
        uniqueName = "material" + std::to_string(nameId++);

    assetLibrary->material(uniqueName, material);
    _lastParsedAssetName = uniqueName;
}

void
MaterialParser::deserializeComplexProperty(MaterialPtr            material,
                                           ComplexProperty        serializedProperty)
{
    uint type = serializedProperty.get<1>().get<0>() >> 24;

    std::tuple<uint, std::string&> serializedPropertyTuple(serializedProperty.get<1>().get<0>(), serializedProperty.get<1>().get<1>());

    if (type == VECTOR4)
		material->data()->set(
			serializedProperty.get<0>(),
			Any::cast<math::vec4>(TypeDeserializer::deserializeVector4(serializedPropertyTuple))
        );
	else if (type == MATRIX4X4)
		material->data()->set(
			serializedProperty.get<0>(),
			Any::cast<math::mat4>(TypeDeserializer::deserializeMatrix4x4(serializedPropertyTuple))
        );
	else if (type == VECTOR2)
		material->data()->set(
			serializedProperty.get<0>(),
			Any::cast<math::vec2>(TypeDeserializer::deserializeVector2(serializedPropertyTuple))
        );
	else if (type == VECTOR3)
		material->data()->set(
			serializedProperty.get<0>(),
			Any::cast<math::vec3>(TypeDeserializer::deserializeVector3(serializedPropertyTuple))
        );
	else if (type == BLENDING)
	{
        auto blendingMode = Any::cast<render::Blending::Mode>(TypeDeserializer::deserializeBlending(serializedPropertyTuple));
        auto srcBlendingMode = static_cast<render::Blending::Source>(static_cast<uint>(blendingMode) & 0x00ff);
        auto dstBlendingMode = static_cast<render::Blending::Destination>(static_cast<uint>(blendingMode) & 0xff00);

        material->data()->set<render::Blending::Mode>("blendingMode", blendingMode);
        material->data()->set<render::Blending::Source>(render::States::PROPERTY_BLENDING_SOURCE, srcBlendingMode);
        material->data()->set<render::Blending::Destination>(render::States::PROPERTY_BLENDING_DESTINATION, dstBlendingMode);

        if (!(blendingMode & render::Blending::Destination::ZERO))
        {
            material->data()->set("priority", render::Priority::TRANSPARENT);
            material->data()->set("zSorted", true);
        }
    }
    else if (type == TRIANGLECULLING)
    {
        material->data()->set<render::TriangleCulling>(
            serializedProperty.get<0>(),
            Any::cast<render::TriangleCulling>(TypeDeserializer::deserializeTriangleCulling(serializedPropertyTuple))
        );
    }
    else if (type == TEXTURE)
    {
        auto textureDependencyId = Any::cast<uint>(TypeDeserializer::deserializeTextureId(serializedPropertyTuple));

        if (_dependency->textureReferenceExists(textureDependencyId))
        {
            const auto textureType = serializedProperty.get<0>();

            auto& textureReference = _dependency->getTextureReference(textureDependencyId);

            if (textureReference.texture)
            {
                auto sampler = _dependency->getTextureReference(textureDependencyId).texture->sampler();

                material->data()->set(
                    serializedProperty.get<0>(),
                    sampler
                );

                material->data()->set(
                    SamplerStates::uniformNameToSamplerStateBindingName(
                        textureType,
                        SamplerStates::PROPERTY_WRAP_MODE
                    ),
                    sampler.wrapMode
                );

                material->data()->set(
                    SamplerStates::uniformNameToSamplerStateBindingName(
                        textureType,
                        SamplerStates::PROPERTY_TEXTURE_FILTER
                    ),
                    sampler.textureFilter
                );

                material->data()->set(
                    SamplerStates::uniformNameToSamplerStateBindingName(
                        textureType,
                        SamplerStates::PROPERTY_MIP_FILTER
                    ),
                    sampler.mipFilter
                );
            }
            else
            {
                textureReference.textureType = textureType;
                textureReference.dependentMaterialDataSet.emplace(material->data());
            }
        }
    }
    else if (type == STRING)
    {
        const auto name = Any::cast<std::string>(TypeDeserializer::deserializeString(serializedPropertyTuple));

        material->data()->set(serializedProperty.get<0>(), name);
    }
}

void
MaterialParser::deserializeBasicProperty(MaterialPtr        material,
                                         BasicProperty        serializedProperty)
{
    std::vector<float> serializedPropertyValue = deserialize::TypeDeserializer::deserializeVector<float>(serializedProperty.get<1>());

	// TODO remove basic and complex property types and always specify property content type

    if (serializedProperty.get<0>() == "zSorted")
        material->data()->set<bool>("zSorted", serializedPropertyValue[0]);
    else if (serializedProperty.get<0>() == "environmentMap2dType")
		material->data()->set<int>("environmentMap2dType", int(serializedPropertyValue[0]));
    else
	    material->data()->set<float>(serializedProperty.get<0>(), serializedPropertyValue[0]);
}
