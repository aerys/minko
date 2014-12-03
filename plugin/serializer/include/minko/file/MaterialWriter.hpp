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

#pragma once

#include "minko/Common.hpp"
#include "minko/file/AbstractWriter.hpp"
#include "minko/material/Material.hpp"
#include "minko/data/Provider.hpp"
#include "msgpack.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Options.hpp"
#include "minko/serialize/TypeSerializer.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/render/Texture.hpp"

namespace minko
{
	namespace file
	{

		class MaterialWriter:
            public AbstractWriter<material::Material::Ptr>
		{

		public:
			typedef std::shared_ptr<MaterialWriter>						Ptr;

			typedef std::shared_ptr<render::AbstractTexture>			TexturePtr;
			typedef msgpack::type::tuple<uint, std::string>				TupleIntString;
			typedef msgpack::type::tuple<std::string, TupleIntString>	ComplexPropertyTuple;
			typedef msgpack::type::tuple<std::string, std::string>		BasicPropertyTuple;

		private:
			static std::map<const std::type_info*, std::function<std::tuple<uint, std::string>(Any)>> _typeToWriteFunction;

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
				  Dependency::Ptr					dependency,
                  std::shared_ptr<WriterOptions>    writerOptions);

		private:
			MaterialWriter();

			template <typename T>
			typename std::enable_if<std::is_base_of<TexturePtr, T>::value, bool>::type
			serializeMaterialValue(material::Material::Ptr										material,
								   std::string&													propertyName,
								   file::AssetLibrary::Ptr										assets,
								   std::vector<ComplexPropertyTuple>							*complexSerializedProperties,
								   std::vector<BasicPropertyTuple>								*basicTypeSeriliazedProperties,
								   Dependency::Ptr												dependency)
			{
				if (material->data()->propertyHasType<TexturePtr>(propertyName))
				{
					std::tuple<uint, std::string> serializedTexture = serialize::TypeSerializer::serializeTexture(Any(dependency->registerDependency(
                        assets->getTextureByUuid(material->data()->get<render::TextureSampler>(propertyName).uuid)
                    )));
					TupleIntString serializedMsgTexture(std::get<0>(serializedTexture), std::get<1>(serializedTexture));

					ComplexPropertyTuple serializedProperty(propertyName, serializedMsgTexture);
					complexSerializedProperties->push_back(serializedProperty);

					return true;
				}

				return false;
			}

			template <typename T>
			typename std::enable_if<!std::is_arithmetic<T>::value && !std::is_base_of<TexturePtr, T>::value, bool>::type
			serializeMaterialValue(material::Material::Ptr										material,
								   	std::string&												propertyName,
								   	file::AssetLibrary::Ptr										assets,
									std::vector<ComplexPropertyTuple>							*complexSerializedProperties,
									std::vector<BasicPropertyTuple>								*basicTypeSeriliazedProperties,
								   	Dependency::Ptr												dependency)
			{
				if (_typeToWriteFunction.find(&typeid(T)) != _typeToWriteFunction.end() &&
					material->data()->propertyHasType<T>(propertyName))
				{
					Any								propertyValue			= material->data()->get<T>(propertyName);
					std::tuple<uint, std::string>	serializedMaterialValue = _typeToWriteFunction[&typeid(T)](propertyValue);
					TupleIntString					serializedMsgMaterialValue(std::get<0>(serializedMaterialValue), std::get<1>(serializedMaterialValue));

					ComplexPropertyTuple serializedProperty(propertyName, serializedMsgMaterialValue);
					complexSerializedProperties->push_back(serializedProperty);

					return true;
				}

				return false;
			}

			template <typename T>
			typename std::enable_if<std::is_arithmetic<T>::value, bool>::type
			serializeMaterialValue(material::Material::Ptr							material,
								   std::string&										propertyName,
								   file::AssetLibrary::Ptr							assets,
								   std::vector<ComplexPropertyTuple>				*complexSerializedProperties,
								   std::vector<BasicPropertyTuple>					*basicTypeSeriliazedProperties,
								   Dependency::Ptr									dependency)
			{
				if (material->data()->propertyHasType<T>(propertyName))
				{
					std::vector<float> propertyValue;
					
					propertyValue.push_back(static_cast<float>(material->data()->get<T>(propertyName)));

					std::string serializePropertyValue = serialize::TypeSerializer::serializeVector<float>(propertyValue);

					BasicPropertyTuple basicTypeSerializedProperty(
						propertyName,
						serializePropertyValue
                    );

					basicTypeSeriliazedProperties->push_back(basicTypeSerializedProperty);

					return true;
				}

				return false;
			}
		};

	}
}
