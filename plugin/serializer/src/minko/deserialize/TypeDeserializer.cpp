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
#include "minko/math/Vector2.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/TriangleCulling.hpp"

using namespace minko;
using namespace minko::deserialize;

Any
TypeDeserializer::deserializeVector4(std::tuple<uint, std::string&>& serializedVector)
{
	std::vector<float>	defaultValues(4, 0);
	uint				serializedIndex = 0;
	std::stringstream	stream;
	
	defaultValues[3] = 1;

	stream << std::get<1>(serializedVector);

	for (unsigned int i = 0; i < 4; ++i)
	{
		if (std::get<0>(serializedVector) & (1u << serializedIndex++))
			read(stream, defaultValues[i]);
			//defaultValues[i] = serializedVector.a1[serializedIndex++];
	}

	return Any(math::Vector4::create(defaultValues[0], defaultValues[1], defaultValues[2], defaultValues[3]));
}
		
Any
TypeDeserializer::deserializeVector3(std::tuple<uint, std::string&>& serializedVector)
{
	std::vector<float>	defaultValues(3, 0);
	uint				serializedIndex = 0;
	std::stringstream	stream;
	
	stream << std::get<1>(serializedVector);//&*serializedVector.a1.begin(), serializedVector.a1.size());


	for (unsigned int i = 0; i < 3; ++i)
	{
		if (std::get<0>(serializedVector) & (1u << serializedIndex++))
			read(stream, defaultValues[i]);
//			defaultValues[i] = serializedVector.a1[serializedIndex++];
	}

	return Any(math::Vector3::create(defaultValues[0], defaultValues[1], defaultValues[2]));
}
			
Any
TypeDeserializer::deserializeVector2(std::tuple<uint, std::string&>& serializedVector)
{
	std::vector<float>	defaultValues(2, 0);
	uint				serializedIndex = 0;
	std::stringstream	stream;
	
	stream << std::get<1>(serializedVector);//(&*serializedVector.a1.begin(), serializedVector.a1.size());

	for (unsigned int i = 0; i < 2; ++i)
	{
		if (std::get<0>(serializedVector) & (1u << serializedIndex++))
			read(stream, defaultValues[i]);
			//defaultValues[i] = serializedVector.a1[serializedIndex++];
	}

	return Any(math::Vector2::create(defaultValues[0], defaultValues[1]));
}

Any
TypeDeserializer::deserializeMatrix4x4(std::tuple<uint, std::string&>& serializeMatrix)
{
	std::vector<float> matrixValues(16, 0);
	std::stringstream	stream;
	
	stream << std::get<1>(serializeMatrix);
	//(&*serializeMatrix.a1.begin(), serializeMatrix.a1.size());
	matrixValues[0] = 1;
	matrixValues[5] = 1;
	matrixValues[10] = 1;
	matrixValues[15] = 1;

	unsigned int serializedIndex = 0;

	for (unsigned int i = 0; i < 16; ++i)
	{
		if (std::get<0>(serializeMatrix) & (1u << i))
			read<float>(stream, matrixValues[i]);
			//matrixValues[i] = serializeMatrix.a1[serializedIndex++];
	}

	return Any(math::Matrix4x4::create()->initialize(matrixValues));
}

Any
TypeDeserializer::deserializeBlending(std::tuple<uint, std::string&>& seriliazedBlending)
{
	if (std::get<1>(seriliazedBlending) == "+")
		return Any(render::Blending::Mode::ADDITIVE);
	
	if (std::get<1>(seriliazedBlending) == "a")
		return Any(render::Blending::Mode::ALPHA);

	return Any(render::Blending::Mode::DEFAULT);
}

Any
TypeDeserializer::deserializeTriangleCulling(std::tuple<uint, std::string&>& seriliazedTriangleCulling)
{
	if (std::get<1>(seriliazedTriangleCulling) == "b")
		return Any(render::TriangleCulling::BACK);
	if (std::get<1>(seriliazedTriangleCulling) == "u")
		return Any(render::TriangleCulling::BOTH);
	if (std::get<1>(seriliazedTriangleCulling) == "f")
		return Any(render::TriangleCulling::FRONT);
	return Any(render::TriangleCulling::NONE);
}

Any
TypeDeserializer::deserializeEnvironmentMap2dType(std::tuple<uint, std::string&>& serialized)
{
	if (std::get<1>(serialized) == "p")
		return Any(render::EnvironmentMap2dType::Probe);
	if (std::get<1>(serialized) == "b")
		return Any(render::EnvironmentMap2dType::BlinnNewell);

	return Any(render::EnvironmentMap2dType::Unset);
}

Any
TypeDeserializer::deserializeTextureId(std::tuple<uint, std::string&>& seriliazedTextureId)
{
	return Any(std::get<0>(seriliazedTextureId) & 0x00FFFFFF);
}
