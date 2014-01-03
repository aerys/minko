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
TypeDeserializer::deserializeVector4(msgpack::type::tuple<uint, std::string>& serializedVector)
{
	std::vector<float>	defaultValues(4, 0);
	uint				serializedIndex = 0;
	std::stringstream	stream;
	
	defaultValues[3] = 1;

	stream << serializedVector.a1;

	for (unsigned int i = 0; i < 4; ++i)
	{
		std::cout << i << " " << (serializedVector.a0 & (1u << serializedIndex)) << std::endl;

		if (serializedVector.a0 & (1u << serializedIndex++))
			read(stream, defaultValues[i]);
			//defaultValues[i] = serializedVector.a1[serializedIndex++];
	}

	return Any(math::Vector4::create(defaultValues[0], defaultValues[1], defaultValues[2], defaultValues[3]));
}
		
Any
TypeDeserializer::deserializeVector3(msgpack::type::tuple<uint, std::string>& serializedVector)
{
	std::vector<float>	defaultValues(3, 0);
	uint				serializedIndex = 0;
	std::stringstream	stream;
	
	stream << serializedVector.a1;//&*serializedVector.a1.begin(), serializedVector.a1.size());


	for (unsigned int i = 0; i < 3; ++i)
	{
		if (serializedVector.a0 & (1u << serializedIndex++))
			read(stream, defaultValues[i]);
//			defaultValues[i] = serializedVector.a1[serializedIndex++];
	}

	return Any(math::Vector3::create(defaultValues[0], defaultValues[1], defaultValues[2]));
}
			
Any
TypeDeserializer::deserializeVector2(msgpack::type::tuple<uint, std::string>& serializedVector)
{
	std::vector<float>	defaultValues(2, 0);
	uint				serializedIndex = 0;
	std::stringstream	stream;
	
	stream << serializedVector.a1;//(&*serializedVector.a1.begin(), serializedVector.a1.size());

	for (unsigned int i = 0; i < 2; ++i)
	{
		if (serializedVector.a0 & (1u << serializedIndex++))
			read(stream, defaultValues[i]);
			//defaultValues[i] = serializedVector.a1[serializedIndex++];
	}

	return Any(math::Vector2::create(defaultValues[0], defaultValues[1]));
}

Any
TypeDeserializer::deserializeMatrix4x4(msgpack::type::tuple<uint, std::string>& serializeMatrix)
{
	std::vector<float> matrixValues(16, 0);
	std::stringstream	stream;
	
	stream << serializeMatrix.a1;
	//(&*serializeMatrix.a1.begin(), serializeMatrix.a1.size());
	matrixValues[0] = 1;
	matrixValues[5] = 1;
	matrixValues[10] = 1;
	matrixValues[15] = 1;

	unsigned int serializedIndex = 0;

	for (unsigned int i = 0; i < 16; ++i)
	{
		if (serializeMatrix.a0 & (1u << i))
			read(stream, matrixValues[i]);
			//matrixValues[i] = serializeMatrix.a1[serializedIndex++];
	}

	return Any(math::Matrix4x4::create()->initialize(matrixValues));
}

Any
TypeDeserializer::deserializeBlending(msgpack::type::tuple<uint, std::string>& seriliazedBlending)
{
	if (seriliazedBlending.a1 == "+")
		return Any(render::Blending::Mode::ADDITIVE);
	
	if (seriliazedBlending.a1 == "a")
		return Any(render::Blending::Mode::ALPHA);

	return Any(render::Blending::Mode::DEFAULT);
}

Any
TypeDeserializer::deserializeTriangleCulling(msgpack::type::tuple<uint, std::string>& seriliazedTriangleCulling)
{
	if (seriliazedTriangleCulling.a1 == "b")
		return Any(render::TriangleCulling::BACK);
	if (seriliazedTriangleCulling.a1 == "u")
		return Any(render::TriangleCulling::BOTH);
	if (seriliazedTriangleCulling.a1 == "f")
		return Any(render::TriangleCulling::FRONT);
	return Any(render::TriangleCulling::NONE);
}

Any
TypeDeserializer::deserializeTextureId(msgpack::type::tuple<uint, std::string>& seriliazedTextureId)
{
	return Any(seriliazedTextureId.a0 & 0x00FFFFFF);
}
