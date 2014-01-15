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

using namespace minko;
using namespace minko::file;
using namespace minko::deserialize;
using namespace minko::serialize;

std::map<uint, std::function<Any(std::tuple<uint, std::string>&)>> MaterialParser::_typeIdToReadFunction;

MaterialParser::MaterialParser()
{
	_typeIdToReadFunction[VECTOR4]			= std::bind(&deserialize::TypeDeserializer::deserializeVector4,			std::placeholders::_1);
	_typeIdToReadFunction[MATRIX4X4]		= std::bind(&deserialize::TypeDeserializer::deserializeMatrix4x4,		std::placeholders::_1);
	_typeIdToReadFunction[VECTOR3]			= std::bind(&deserialize::TypeDeserializer::deserializeVector3,			std::placeholders::_1);
	_typeIdToReadFunction[VECTOR2]			= std::bind(&deserialize::TypeDeserializer::deserializeVector2,			std::placeholders::_1);
	_typeIdToReadFunction[BLENDING]			= std::bind(&deserialize::TypeDeserializer::deserializeBlending,		std::placeholders::_1);
	_typeIdToReadFunction[TRIANGLECULLING]	= std::bind(&deserialize::TypeDeserializer::deserializeTriangleCulling, std::placeholders::_1);
}

void
MaterialParser::parse(const std::string&				filename,
					  const std::string&                resolvedFilename,
					  OptionsPtr						options,
					  const std::vector<unsigned char>&	data,
					  AssetLibraryPtr					assetLibrary)
{
	msgpack::object		msgpackObject;
	msgpack::zone		mempool;
	std::string			str = extractDependencies(assetLibrary, data, options, extractFolderPath(filename));

	msgpack::type::tuple<std::vector<ComplexProperty>, std::vector<BasicProperty>> serializedMaterial;
	msgpack::unpack(str.data(), str.size(), NULL, &mempool, &msgpackObject);
	msgpackObject.convert(&serializedMaterial);

	std::vector<ComplexProperty> complexProperties	= serializedMaterial.a0;
	std::vector<BasicProperty>	 basicProperties	= serializedMaterial.a1;

	MaterialPtr material = material::Material::create();

	for (auto serializedComplexProperty : complexProperties)
		deserializeComplexProperty(material, serializedComplexProperty);

	for (auto serializedBasicProperty : basicProperties)
		deserializeBasicProperty(material, serializedBasicProperty);

	assetLibrary->material(resolvedFilename, material);
	_lastParsedAssetName = resolvedFilename;
}

void
MaterialParser::deserializeComplexProperty(MaterialPtr			material,
										   ComplexProperty		serializedProperty)
{
	uint type = serializedProperty.a1.a0 >> 24;

	if (type == VECTOR4)
		material->set<Vector4Ptr>(
			serializedProperty.a0, 
			Any::cast<Vector4Ptr>(TypeDeserializer::deserializeVector4(std::make_tuple(serializedProperty.a1.a0, serializedProperty.a1.a1))));
	else if (type == MATRIX4X4)
		material->set<Matrix4x4Ptr>(
			serializedProperty.a0, 
			Any::cast<Matrix4x4Ptr>(TypeDeserializer::deserializeMatrix4x4(std::make_tuple(serializedProperty.a1.a0, serializedProperty.a1.a1))));
	else if (type == VECTOR2)
		material->set<Vector2Ptr>(
			serializedProperty.a0, 
			Any::cast<Vector2Ptr>(TypeDeserializer::deserializeVector2(std::make_tuple(serializedProperty.a1.a0, serializedProperty.a1.a1))));
	else if (type == VECTOR3)
		material->set<Vector3Ptr>(
			serializedProperty.a0, 
			Any::cast<Vector3Ptr>(TypeDeserializer::deserializeVector3(std::make_tuple(serializedProperty.a1.a0, serializedProperty.a1.a1))));
	else if (type == BLENDING)
		material->set<render::Blending::Mode>(
			serializedProperty.a0, 
			Any::cast<render::Blending::Mode>(TypeDeserializer::deserializeBlending(std::make_tuple(serializedProperty.a1.a0, serializedProperty.a1.a1))));
	else if (type == TRIANGLECULLING)
		material->set<render::TriangleCulling>(
			serializedProperty.a0, 
			Any::cast<render::TriangleCulling>(TypeDeserializer::deserializeTriangleCulling(std::make_tuple(serializedProperty.a1.a0, serializedProperty.a1.a1))));
	else if (type == TEXTURE)
		material->set<TexturePtr>(
			serializedProperty.a0,
			_dependencies->getTextureReference(Any::cast<uint>(TypeDeserializer::deserializeTextureId(std::make_tuple(serializedProperty.a1.a0, serializedProperty.a1.a1)))));
}

void
MaterialParser::deserializeBasicProperty(MaterialPtr		material,
										 BasicProperty		serializedProperty)
{
	material->set<float>(serializedProperty.a0, serializedProperty.a1);
}
