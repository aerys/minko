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
#include "minko/math/Matrix4x4.hpp"
#include "minko/Any.hpp"
#include "minko/data/Provider.hpp"
#include "minko/math/Vector2.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/render/Texture.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/file/Options.hpp"
#include "minko/render/Priority.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::deserialize;
using namespace minko::serialize;

std::map<uint, std::function<Any(std::tuple<uint, std::string&>&)>> MaterialParser::_typeIdToReadFunction;

MaterialParser::MaterialParser()
{
    _typeIdToReadFunction[VECTOR4]            = std::bind(&deserialize::TypeDeserializer::deserializeVector4,            std::placeholders::_1);
    _typeIdToReadFunction[MATRIX4X4]        = std::bind(&deserialize::TypeDeserializer::deserializeMatrix4x4,        std::placeholders::_1);
    _typeIdToReadFunction[VECTOR3]            = std::bind(&deserialize::TypeDeserializer::deserializeVector3,            std::placeholders::_1);
    _typeIdToReadFunction[VECTOR2]            = std::bind(&deserialize::TypeDeserializer::deserializeVector2,            std::placeholders::_1);
    _typeIdToReadFunction[BLENDING]            = std::bind(&deserialize::TypeDeserializer::deserializeBlending,        std::placeholders::_1);
    _typeIdToReadFunction[TRIANGLECULLING]    = std::bind(&deserialize::TypeDeserializer::deserializeTriangleCulling, std::placeholders::_1);
}

void
MaterialParser::parse(const std::string&                filename,
                      const std::string&                resolvedFilename,
                      Options::Ptr                        options,
                      const std::vector<unsigned char>&    data,
                      AssetLibraryPtr                    assetLibrary)
{
    readHeader(filename, data);

    if (_magicNumber != 0x4D4B034D)
        throw std::logic_error("Invalid material data");

    msgpack::object        msgpackObject;
    msgpack::zone        mempool;
    std::string         folderpath = extractFolderPath(resolvedFilename);
    extractDependencies(assetLibrary, data, _headerSize, _dependenciesSize, options, folderpath);

    msgpack::type::tuple<std::vector<ComplexProperty>, std::vector<BasicProperty>> serializedMaterial;
    msgpack::unpack((char*)&data[_headerSize + _dependenciesSize], _sceneDataSize, NULL, &mempool, &msgpackObject);
    msgpackObject.convert(&serializedMaterial);

    std::vector<unsigned char>* d = (std::vector<unsigned char>*)&data;
    d->clear();
    d->shrink_to_fit();

    std::vector<ComplexProperty> complexProperties    = serializedMaterial.a0;
    std::vector<BasicProperty>     basicProperties    = serializedMaterial.a1;

    MaterialPtr material = material::Material::create();

    material->set("diffuseColor", math::Vector4::create(1.0, 1.0, 1.0, 1.0));

    for (auto serializedComplexProperty : complexProperties)
        deserializeComplexProperty(material, serializedComplexProperty);

    for (auto serializedBasicProperty : basicProperties)
        deserializeBasicProperty(material, serializedBasicProperty);

    material = options->materialFunction()(material->arrayName(), material);

    assetLibrary->material(filename, material);
    _lastParsedAssetName = filename;
}

void
MaterialParser::deserializeComplexProperty(MaterialPtr            material,
                                           ComplexProperty        serializedProperty)
{
    uint type = serializedProperty.a1.a0 >> 24;

    std::tuple<uint, std::string&> serializedPropertyTuple(serializedProperty.a1.a0, serializedProperty.a1.a1);

    if (type == VECTOR4)
        material->set<Vector4Ptr>(
            serializedProperty.a0,
            Any::cast<Vector4Ptr>(TypeDeserializer::deserializeVector4(serializedPropertyTuple)));
    else if (type == MATRIX4X4)
        material->set<Matrix4x4Ptr>(
            serializedProperty.a0,
            Any::cast<Matrix4x4Ptr>(TypeDeserializer::deserializeMatrix4x4(serializedPropertyTuple)));
    else if (type == VECTOR2)
        material->set<Vector2Ptr>(
            serializedProperty.a0,
            Any::cast<Vector2Ptr>(TypeDeserializer::deserializeVector2(serializedPropertyTuple)));
    else if (type == VECTOR3)
        material->set<Vector3Ptr>(
            serializedProperty.a0,
            Any::cast<Vector3Ptr>(TypeDeserializer::deserializeVector3(serializedPropertyTuple)));
    else if (type == BLENDING)
    {
        material->set<render::Blending::Mode>(
            serializedProperty.a0,
            Any::cast<render::Blending::Mode>(TypeDeserializer::deserializeBlending(serializedPropertyTuple)));

        if (material->get<render::Blending::Mode>("blendMode") != render::Blending::Mode::DEFAULT)

        material->set("priority", render::Priority::TRANSPARENT);
        material->set("zSort", true);
    }
    else if (type == TRIANGLECULLING)
        material->set<render::TriangleCulling>(
            serializedProperty.a0,
            Any::cast<render::TriangleCulling>(TypeDeserializer::deserializeTriangleCulling(serializedPropertyTuple)));
    else if (type == TEXTURE)
        material->set<TexturePtr>(
            serializedProperty.a0,
            _dependencies->getTextureReference(Any::cast<uint>(TypeDeserializer::deserializeTextureId(serializedPropertyTuple))));
    else if (type == ENVMAPTYPE)
    {
        auto envMapType = Any::cast<render::EnvironmentMap2dType>(TypeDeserializer::deserializeEnvironmentMap2dType(serializedPropertyTuple));

        material->set<int>(serializedProperty.a0, int(envMapType));
    }
}

void
MaterialParser::deserializeBasicProperty(MaterialPtr        material,
                                         BasicProperty        serializedProperty)
{
    std::vector<float> serializedPropertyValue = deserialize::TypeDeserializer::deserializeVector<float>(serializedProperty.a1);
    material->set<float>(serializedProperty.a0, serializedPropertyValue[0]);
}
