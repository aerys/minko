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

#include "TypeDeserializer.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/Any.hpp"
#include "minko/Qark.hpp"
#include "minko/data/Provider.hpp"
#include "minko/deserialize/MkTypes.hpp"
#include "minko/deserialize/NameConverter.hpp"
#include "minko/math/Vector3.hpp"

using namespace minko;
using namespace minko::deserialize;

math::Matrix4x4::Ptr
TypeDeserializer::matrix4x4(Any& matrixObject)
{
	Qark::ByteArray&					matrixData = Any::cast<Qark::ByteArray&>(matrixObject);
	std::shared_ptr<math::Matrix4x4>	matrix = math::Matrix4x4::create();
	std::vector<float>					datas;

	datas.resize(16);

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

data::Provider::Ptr
TypeDeserializer::provider(std::vector<Any>&									properties,
						   std::map<int, std::shared_ptr<render::Texture>>&		idToTexture,
						   NameConverter::Ptr									nameConverter)
{
	std::shared_ptr<data::Provider> material = data::Provider::create();

	for (unsigned int propertyId = 0; propertyId < properties.size(); ++propertyId)
	{
		Qark::Map&		property		= Any::cast<Qark::Map&>(properties[propertyId]);
		Qark::Map&		propertyValue	= Any::cast<Qark::Map&>(property["value"]);
		std::string&	propertyName	= Any::cast<std::string&>(property["name"]);
		int				type			= Any::cast<int>(propertyValue["type"]);
       
		if (type == MkTypes::TEXTURE_RESOURCE)
			material->set(nameConverter->convertString(propertyName), idToTexture[Any::cast<int>(propertyValue["id"])]);
		if (type == MkTypes::NUMBER)
		{
            if (propertyName == "diffuseColor" || propertyName == "specular")
            {
			    unsigned int color = 0;
			    if (typeid(unsigned int) == propertyValue["value"].type())
				    color = Any::cast<unsigned int>(propertyValue["value"]);
			    else 
				    color = Any::cast<int>(propertyValue["value"]);

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
            else if (propertyName == "alphaThreshold")
            {
                material->set(nameConverter->convertString(propertyName), Any::cast<float>(propertyValue["value"]));
            }
		}
	}

	if (!material->hasProperty("material.specular"))
		material->set("material.specular", math::Vector3::create(.8f, .8f, .8f));
	
	material->set("material.shininess", 10.0f);
	
	return material;
}

