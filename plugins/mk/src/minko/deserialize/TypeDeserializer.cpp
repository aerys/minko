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

#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/Any.hpp"
#include "minko/data/Provider.hpp"
#include "minko/deserialize/MkTypes.hpp"
#include "minko/deserialize/NameConverter.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Blending.hpp"

using namespace minko;
using namespace minko::deserialize;

std::map<const unsigned int, TypeDeserializer::ReadMkTypeFunction> 
TypeDeserializer::_typeToReadFunc = TypeDeserializer::initIdToFunctionMap();

std::map<const unsigned int, TypeDeserializer::ReadMkTypeFunction>
TypeDeserializer::initIdToFunctionMap()
{
	std::map<const unsigned int, TypeDeserializer::ReadMkTypeFunction> m;

	//m[MkTypes::VECTORM4X4]			= std::bind(&TypeDeserializer::unsupport, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
	//m[MkTypes::MATRIX4X4]			= std::bind(&TypeDeserializer::unsupport, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
	//m[MkTypes::VECTORN]				= std::bind(&TypeDeserializer::unsupport, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
	//m[MkTypes::VECTORV4]			= std::bind(&TypeDeserializer::unsupport, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);

	m[MkTypes::BOOLEAN]				= std::bind(&TypeDeserializer::boolean, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
	m[MkTypes::VECTOR4B]			= std::bind(&TypeDeserializer::vector4b, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
	m[MkTypes::VECTOR4]				= std::bind(&TypeDeserializer::vector4, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
	m[MkTypes::TEXTURE_RESOURCE]	= std::bind(&TypeDeserializer::texture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
	m[MkTypes::NUMBER]				= std::bind(&TypeDeserializer::number, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
	
	return m;
}

void
TypeDeserializer::texture(std::map<int, render::AbstractTexture::Ptr>&	idToTexture,
						  std::string&									propertyName,
						  std::shared_ptr<data::Provider>				material,
						  Qark::Map&									object,
						  std::shared_ptr<NameConverter>				nameConverter)
{
	material->set(nameConverter->convertString(propertyName), idToTexture[Any::cast<int>(object["id"])]);
}

void
TypeDeserializer::boolean(std::map<int, render::AbstractTexture::Ptr>&	idToTexture,
						  std::string&									propertyName,
						  std::shared_ptr<data::Provider>				material,
						  Qark::Map&									object,
						  std::shared_ptr<NameConverter>				nameConverter)
{
	material->set(nameConverter->convertString(propertyName), Any::cast<bool>(object["value"]));
}

void
TypeDeserializer::number(std::map<int, render::AbstractTexture::Ptr>&	idToTexture,
						 std::string&									propertyName,
						 std::shared_ptr<data::Provider>				material,
						 Qark::Map&										object,
						 std::shared_ptr<NameConverter>					nameConverter)
{
	if (propertyName == "diffuseColor" || propertyName == "specular")
	{
	    unsigned int color = 0;
	    if (typeid(unsigned int) == object["value"].type())
		    color = Any::cast<unsigned int>(object["value"]);
		else if (typeid(int) == object["value"].type())
			color = Any::cast<int>(object["value"]);
		else
			color = 0xFFFFFFFF;
	
	    unsigned int red	= (color & 0xFF000000) >> 24;
	    unsigned int blue	= (color & 0x00FF0000) >> 16;
	    unsigned int green	= (color & 0x0000FF00) >> 8;
	    unsigned int alpha	= (color & 0x000000FF);
	
	    if (propertyName == "specular")
		    material->set(
	            nameConverter->convertString(propertyName),
	            math::Vector3::create(float(red) / 255.0f, float(blue) / 255.0f, float(green) / 255.0f)
	        );
	    else
		    material->set(
	            nameConverter->convertString(propertyName),
	            math::Vector4::create(float(red) / 255.0f, float(blue) / 255.0f, float(green) / 255.0f, float(alpha) / 255.0f)
	        );
	}
	else
	{
		if (object["value"].type() == typeid(float))
			material->set(nameConverter->convertString(propertyName), Any::cast<float>(object["value"]));
		else if (object["value"].type() == typeid(int))
			material->set(nameConverter->convertString(propertyName), static_cast<float>(Any::cast<int>(object["value"])));
		else
			material->set(nameConverter->convertString(propertyName), Any::cast<unsigned int>(object["value"]));

	}

}

// tmp, will be removed when all mk write vector4 as vector4b
void
TypeDeserializer::vector4(std::map<int, render::AbstractTexture::Ptr>&	idToTexture,
						  std::string&									propertyName,
						  std::shared_ptr<data::Provider>				material,
						  Qark::Map&									object,
						  std::shared_ptr<NameConverter>				nameConverter)
{
	std::vector<Any>&	vectorValues	= Any::cast<std::vector<Any>&>(object["value"]);
	
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 1.f;

	if (vectorValues[0].type() == typeid(float))
		x = Any::cast<float>(vectorValues[0]);
	else 
		x = reinterpret_cast<float&>(Any::cast<int&>(vectorValues[0]));

	if (vectorValues[1].type() == typeid(float))
		y = Any::cast<float>(vectorValues[1]);
	else 
		y = reinterpret_cast<float&>(Any::cast<int&>(vectorValues[1]));

	if (vectorValues[2].type() == typeid(float))
		z = Any::cast<float>(vectorValues[2]);
	else 
		z = reinterpret_cast<float&>(Any::cast<int&>(vectorValues[2]));

	if (vectorValues[3].type() == typeid(float))
		w = Any::cast<float>(vectorValues[3]);
	else 
		w = reinterpret_cast<float&>(Any::cast<int&>(vectorValues[3]));

	// tmp
	if (propertyName != "specular")
		material->set(nameConverter->convertString(propertyName),  math::Vector4::create(x, y, z, w));
	else
		material->set(nameConverter->convertString(propertyName),  math::Vector3::create(x, y, z));
}

void
TypeDeserializer::vector4b(std::map<int, std::shared_ptr<render::AbstractTexture>>&	idToTexture,
						   std::string&												propertyName,
						   std::shared_ptr<data::Provider>							material,
						   Qark::Map&												object,
						   std::shared_ptr<NameConverter>							nameConverter)
{
	Qark::ByteArray&		vectorData	= Any::cast<Qark::ByteArray&>(object["value"]);
	std::vector<float>		datas(4, 0);

	for (int i = 0; i < 4; ++i)
	{
		unsigned int value = 
			(((unsigned int)vectorData[i * 4]		<< 24)	& 0xFF000000) + 
			(((unsigned int)vectorData[i * 4 + 1]	<< 16)	& 0x00FF0000) + 
			(((unsigned int)vectorData[i * 4 + 2]	<< 8)	& 0x0000FF00) + 
			(((unsigned int)vectorData[i * 4 + 3])			& 0x000000FF);
		datas[i] = reinterpret_cast<float&>(value);
	}
   
	if (propertyName != "specular")
		material->set(nameConverter->convertString(propertyName),  math::Vector4::create(datas[0], datas[1], datas[2], datas[3]));
	else
		material->set(nameConverter->convertString(propertyName),  math::Vector3::create(datas[0], datas[1], datas[2]));
}

math::Matrix4x4::Ptr
TypeDeserializer::matrix4x4(Any& matrixObject)
{
	Qark::ByteArray&		matrixData	= Any::cast<Qark::ByteArray&>(matrixObject);
	math::Matrix4x4::Ptr	matrix		= math::Matrix4x4::create();
	std::vector<float>		datas(16, 0);

	for (int i = 0; i < 16; ++i)
	{
		unsigned int value = 
			(((unsigned int)matrixData[i * 4]		<< 24)	& 0xFF000000) + 
			(((unsigned int)matrixData[i * 4 + 1]	<< 16)	& 0x00FF0000) + 
			(((unsigned int)matrixData[i * 4 + 2]	<< 8)	& 0x0000FF00) + 
			(((unsigned int)matrixData[i * 4 + 3])			& 0x000000FF);
		datas[i] = reinterpret_cast<float&>(value);
	}
   
	return matrix->initialize(datas)->transpose();
}

void
TypeDeserializer::unsupport(std::map<int, std::shared_ptr<render::AbstractTexture>>&	idToTexture,
							std::string&												propertyName,
							std::shared_ptr<data::Provider>								material,
							Qark::Map&													object,
							std::shared_ptr<NameConverter>								nameConverter)
{
}

data::Provider::Ptr
TypeDeserializer::provider(data::Provider::Ptr					defaultValues,
						   std::vector<Any>&					properties,
						   std::map<int, render::AbstractTexture::Ptr>&	idToTexture,
						   NameConverter::Ptr					nameConverter)
{
	data::Provider::Ptr material = data::Provider::create(defaultValues);

	for (unsigned int propertyId = 0; propertyId < properties.size(); ++propertyId)
	{
		Qark::Map&		property		= Any::cast<Qark::Map&>(properties[propertyId]);
		Qark::Map&		propertyValue	= Any::cast<Qark::Map&>(property["value"]);
		std::string&	propertyName	= Any::cast<std::string&>(property["name"]);
		int&			type			= Any::cast<int&>(propertyValue["type"]);

		if (_typeToReadFunc.find(type) != _typeToReadFunc.end())
		{
			_typeToReadFunc[type](idToTexture, propertyName, material, propertyValue, nameConverter);
		}
	}

	if (!material->hasProperty("material.specular"))
		material->set("material.specular", math::Vector3::create(.8f, .8f, .8f));

	if (material->hasProperty("material.environmentMap") && (!material->hasProperty("material.environmentAlpha")))
		material->set("material.environmentAlpha", 0.5f);

	if (material->hasProperty("material.blending"))
		material->set("material.blendMode", render::Blending::Mode::ALPHA);

	material->set("material.shininess", 10.f);
	
	return material;
}

