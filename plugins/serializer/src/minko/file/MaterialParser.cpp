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

std::map<uint, std::function<Any(msgpack::type::tuple<uint, std::string>&)>> MaterialParser::_typeIdToReadFunction;

MaterialParser::MaterialParser()
{
	_typeIdToReadFunction[serializer::VECTOR4]			= std::bind(&deserialize::TypeDeserializer::deserializeVector4, std::placeholders::_1);
	_typeIdToReadFunction[serializer::MATRIX4X4]		= std::bind(&deserialize::TypeDeserializer::deserializeMatrix4x4, std::placeholders::_1);
	_typeIdToReadFunction[serializer::VECTOR3]			= std::bind(&deserialize::TypeDeserializer::deserializeVector3, std::placeholders::_1);
	_typeIdToReadFunction[serializer::VECTOR2]			= std::bind(&deserialize::TypeDeserializer::deserializeVector2, std::placeholders::_1);
	_typeIdToReadFunction[serializer::BLENDING]			= std::bind(&deserialize::TypeDeserializer::deserializeBlending, std::placeholders::_1);
	_typeIdToReadFunction[serializer::TRIANGLECULLING]	= std::bind(&deserialize::TypeDeserializer::deserializeTriangleCulling, std::placeholders::_1);
}

void
MaterialParser::parse(const std::string&				filename,
					  const std::string&                resolvedFilename,
					  std::shared_ptr<Options>          options,
					  const std::vector<unsigned char>&	data,
					  std::shared_ptr<AssetLibrary>		assetLibrary)
{
	msgpack::object			msgpackObject;
	msgpack::zone			mempool;
	std::string				str = extractDependencies(assetLibrary, data, options);

	msgpack::type::tuple<std::vector<msgpack::type::tuple<std::string, msgpack::type::tuple<uint, std::string>>>, std::vector<msgpack::type::tuple<std::string, float>>> serializedMaterial;
	msgpack::unpack(str.data(), str.size(), NULL, &mempool, &msgpackObject);
	msgpackObject.convert(&serializedMaterial);

	std::vector<msgpack::type::tuple<std::string, msgpack::type::tuple<uint, std::string>>> complexProperties	= serializedMaterial.a0;
	std::vector<msgpack::type::tuple<std::string, float>>									basicProperties		= serializedMaterial.a1;

	material::Material::Ptr material = material::Material::create();

	for (auto serializedComplexProperty : complexProperties)
		deserializeComplexProperty(material, serializedComplexProperty);

	for (auto serializedBasicProperty : basicProperties)
		deserializeBasicProperty(material, serializedBasicProperty);

	assetLibrary->material(resolvedFilename, material);
	_lastParsedAssetName = resolvedFilename;
}

void
MaterialParser::deserializeComplexProperty(std::shared_ptr<material::Material>											material,
										   msgpack::type::tuple<std::string, msgpack::type::tuple<uint, std::string>>	serializedProperty)
{
	uint type = serializedProperty.a1.a0 >> 24;

	if (type == serializer::VECTOR4)
		material->set<std::shared_ptr<math::Vector4>>(
			serializedProperty.a0, 
			Any::cast<std::shared_ptr<math::Vector4>>(deserialize::TypeDeserializer::deserializeVector4(serializedProperty.a1)));
	else if (type == serializer::MATRIX4X4)
		material->set<std::shared_ptr<math::Matrix4x4>>(
			serializedProperty.a0, 
			Any::cast<std::shared_ptr<math::Matrix4x4>>(deserialize::TypeDeserializer::deserializeMatrix4x4(serializedProperty.a1)));
	else if (type == serializer::VECTOR2)
		material->set<std::shared_ptr<math::Vector2>>(
			serializedProperty.a0, 
			Any::cast<std::shared_ptr<math::Vector2>>(deserialize::TypeDeserializer::deserializeVector2(serializedProperty.a1)));
	else if (type == serializer::VECTOR3)
		material->set<std::shared_ptr<math::Vector3>>(
			serializedProperty.a0, 
			Any::cast<std::shared_ptr<math::Vector3>>(deserialize::TypeDeserializer::deserializeVector3(serializedProperty.a1)));
	else if (type == serializer::BLENDING)
		material->set<render::Blending::Mode>(
			serializedProperty.a0, 
			Any::cast<render::Blending::Mode>(deserialize::TypeDeserializer::deserializeBlending(serializedProperty.a1)));
	else if (type == serializer::TRIANGLECULLING)
		material->set<render::TriangleCulling>(
			serializedProperty.a0, 
			Any::cast<render::TriangleCulling>(deserialize::TypeDeserializer::deserializeTriangleCulling(serializedProperty.a1)));
	else if (type == serializer::TEXTURE)
		material->set<std::shared_ptr<render::Texture>>(
			serializedProperty.a0,
			_dependencies->getTextureReference(Any::cast<uint>(deserialize::TypeDeserializer::deserializeTextureId(serializedProperty.a1))));
}

void
MaterialParser::deserializeBasicProperty(std::shared_ptr<material::Material>		material,
										 msgpack::type::tuple<std::string, float>	serializedProperty)
{
	material->set<float>(serializedProperty.a0, serializedProperty.a1);
}
