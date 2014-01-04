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

#pragma once

#include "minko/Common.hpp"
#include "minko/file/AbstractWriter.hpp"
#include "minko/material/Material.hpp"
#include "minko/data/Provider.hpp"
#include "minko/render/Blending.hpp"
#include "msgpack.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Options.hpp"
#include "minko/serialize/TypeSerializer.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/data/Provider.hpp"

namespace minko
{
	namespace file
	{
		static std::map<const std::type_info*, std::function<msgpack::type::tuple<uint, std::string>(Any)>> _typeToWriteFunction;

		class MaterialWriter:
			public AbstractWriter<data::Provider::Ptr>
		{

		public:
			typedef std::shared_ptr<MaterialWriter> Ptr;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<MaterialWriter>(new MaterialWriter());
			}

			std::string
			embed(std::shared_ptr<AssetLibrary>		assetLibrary,
				  std::shared_ptr<Options>			options,
				  Dependency::Ptr					dependency)
			{
				material::Material::Ptr																	material = std::dynamic_pointer_cast<material::Material>(data());
				std::vector<msgpack::type::tuple<std::string, msgpack::type::tuple<uint, std::string>>> serializedComplexProperties;
				std::vector<msgpack::type::tuple<std::string, float>>									serializedBasicProperties;

				for (std::string structuredPropertyName : material->propertyNames())
				{
					std::string propertyName = (structuredPropertyName).substr(material->structureName().size() + 1);

					msgpack::type::tuple<std::string, msgpack::type::tuple<uint, std::string>> serializedProperty;

					if (serializeMaterialValue<uint>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
						continue;
					else if (serializeMaterialValue<int>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
						continue;
					else if (serializeMaterialValue<unsigned short>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
						continue;
					else if (serializeMaterialValue<short>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
						continue;
					else if (serializeMaterialValue<unsigned char>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
						continue;
					else if (serializeMaterialValue<char>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
						continue;
					else if (serializeMaterialValue<float>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
						continue;
					else if (serializeMaterialValue<bool>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
						continue;
					else if (serializeMaterialValue<render::Blending::Mode>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
						continue;
					else if (serializeMaterialValue<render::TriangleCulling>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
						continue;
					else if (serializeMaterialValue<std::shared_ptr<math::Vector2>>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
						continue;
					else if (serializeMaterialValue<std::shared_ptr<math::Vector3>>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
						continue;
					else if (serializeMaterialValue<std::shared_ptr<math::Vector4>>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
						continue;
					else if (serializeMaterialValue<std::shared_ptr<math::Matrix4x4>>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
						continue;
					else if (serializeMaterialValue<std::shared_ptr<render::Texture>>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
						continue;
					else
						std::cerr << propertyName << " can't be serialized : missing technique" << std::endl << std::endl;
				}

				auto it1 = serializedBasicProperties.begin();
				auto it2 = serializedComplexProperties.begin();

				std::cout << "Material : " << assetLibrary->material(material) << std::endl;

				std::cout << "  Basic Type properties " << std::endl;

				while (it1 != serializedBasicProperties.end())
				{
					std::cout << "	" << it1->a0 << std::endl;
					it1++;
				}

				std::cout << "  Complex Type properties " << std::endl;

				while (it2 != serializedComplexProperties.end())
				{
					std::cout << "	" << it2->a0 << std::endl;
					it2++;
				}

				std::cout << std::endl;

				msgpack::type::tuple<std::vector<msgpack::type::tuple<std::string, msgpack::type::tuple<uint, std::string>>>, std::vector<msgpack::type::tuple<std::string, float>>> res(
					serializedComplexProperties, serializedBasicProperties);
				std::stringstream sbuf;
				msgpack::pack(sbuf, res);

				return sbuf.str();
			}

		private:
			MaterialWriter()
			{
				_typeToWriteFunction[&typeid(std::shared_ptr<math::Matrix4x4>)]		= std::bind(&serialize::TypeSerializer::serializeMatrix4x4, std::placeholders::_1);
				_typeToWriteFunction[&typeid(std::shared_ptr<math::Vector2>)]		= std::bind(&serialize::TypeSerializer::serializeVector2, std::placeholders::_1);
				_typeToWriteFunction[&typeid(std::shared_ptr<math::Vector3>)]		= std::bind(&serialize::TypeSerializer::serializeVector3, std::placeholders::_1);
				_typeToWriteFunction[&typeid(std::shared_ptr<math::Vector4>)]		= std::bind(&serialize::TypeSerializer::serializeVector4, std::placeholders::_1);
				_typeToWriteFunction[&typeid(render::Blending::Mode)]				= std::bind(&serialize::TypeSerializer::serializeBlending, std::placeholders::_1);
				_typeToWriteFunction[&typeid(render::TriangleCulling)]				= std::bind(&serialize::TypeSerializer::serializeCulling, std::placeholders::_1);
			}

			template <typename T>
			typename std::enable_if<std::is_base_of<std::shared_ptr<render::Texture>, T>::value, bool>::type
			serializeMaterialValue(material::Material::Ptr																	material, 
								   std::string																				propertyName,
								   file::AssetLibrary::Ptr																	assets,
								   std::vector<msgpack::type::tuple<std::string, msgpack::type::tuple<uint, std::string>>>	*complexSerializedProperties,
								   std::vector<msgpack::type::tuple<std::string, float>>									*basicTypeSeriliazedProperties,
								   Dependency::Ptr																			dependency)
			{
				if (material->propertyHasType<std::shared_ptr<render::Texture>>(propertyName))
				{
					msgpack::type::tuple<std::string, msgpack::type::tuple<uint, std::string>> serializedProperty(
							propertyName,
							serialize::TypeSerializer::serializeTexture(Any(dependency->registerDependency(material->get<std::shared_ptr<render::Texture>>(propertyName)))));
							//static_cast<float>(dependency->registerDependency(material->get<std::shared_ptr<render::Texture>>(propertyName))));
						complexSerializedProperties->push_back(serializedProperty);

					return true;
				}
				
				return false;
			}

			template <typename T>
			typename std::enable_if<!std::is_arithmetic<T>::value && !std::is_base_of<std::shared_ptr<render::Texture>, T>::value, bool>::type
			serializeMaterialValue(material::Material::Ptr																			material, 
								   	std::string																				propertyName,
								   	file::AssetLibrary::Ptr																	assets,
								   	std::vector<msgpack::type::tuple<std::string, msgpack::type::tuple<uint, std::string>>>	*complexSerializedProperties,
								   	std::vector<msgpack::type::tuple<std::string, float>>									*basicTypeSeriliazedProperties,
								   	Dependency::Ptr																			dependency)
			{
				if (_typeToWriteFunction.find(&typeid(T)) != _typeToWriteFunction.end() &&
					material->propertyHasType<T>(propertyName))
				{
						Any propertyValue = material->get<T>(propertyName);

						msgpack::type::tuple<std::string, msgpack::type::tuple<uint, std::string>> serializedProperty(
							propertyName,
							_typeToWriteFunction[&typeid(T)](propertyValue));
						complexSerializedProperties->push_back(serializedProperty);
						return true;
				}
				
				return false;
			}

			template <typename T>
			typename std::enable_if<std::is_arithmetic<T>::value, bool>::type
			serializeMaterialValue(material::Material::Ptr																	material, 
								   std::string																				propertyName,
								   file::AssetLibrary::Ptr																	assets,
								   std::vector<msgpack::type::tuple<std::string, msgpack::type::tuple<uint, std::string>>>	*complexSerializedProperties,
								   std::vector<msgpack::type::tuple<std::string, float>>									*basicTypeSeriliazedProperties,
								   Dependency::Ptr																			dependency)
			{
				if (material->propertyHasType<T>(propertyName))
				{
					msgpack::type::tuple<std::string, float> basicTypeSerializedProperty(
							propertyName,
							static_cast<float>(material->get<T>(propertyName)));
						basicTypeSeriliazedProperties->push_back(basicTypeSerializedProperty);

					return true;
				}
				
				return false;
			}
		};

	}
}
