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

#include "minko/serialize/TypeSerializer.hpp"
#include "minko/Any.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/Types.hpp"

using namespace minko;
using namespace minko::serialize;

std::tuple<uint, std::string>
TypeSerializer::serializeVector4(Any value)
{
	auto vect4 = Any::cast<math::vec4>(value);
	uint type = 0x00000000;
	float* values = math::value_ptr(vect4);
	std::stringstream stream;

	type += VECTOR4 << 24;

	for (uint i = 0; i < 4; ++i)
	{
		float compareValue = 0;

		if (i == 3)
			compareValue = 1;

		if (values[i] != compareValue)
		{
			//res.push_back(values[i]);
			write(stream, values[i]);
			type += 1u << i;
		}
	}

	return std::tuple<uint, std::string>(type, stream.str());
}
		
std::tuple<uint, std::string>
TypeSerializer::serializeVector3(Any value)
{
	auto vect3 = Any::cast<math::vec3>(value);
	uint type = 0x00000000;
    float* values = math::value_ptr(vect3);
	std::stringstream stream;

	type += VECTOR3 << 24;

	for (uint i = 0; i < 3; ++i)
	{
		if (values[i] != 0)
		{
			//res.push_back(values[i]);
			write(stream, values[i]);
			type += 1u << i;
		}
	}

	return std::tuple<uint, std::string>(type, stream.str());
}

std::tuple<uint, std::string>
TypeSerializer::serializeVector2(Any value)
{
	auto vect2 = Any::cast<math::vec2>(value);
	uint type = 0x00000000;
	float* values = math::value_ptr(vect2);
	std::stringstream	stream;

	type += VECTOR2 << 24;

	for (uint i = 0; i < 2; ++i)
	{
		if (values[i] != 0)
		{
			//res.push_back(values[i]);
			write(stream, values[i]);
			type += 1u << i;
		}
	}

	return std::tuple<uint, std::string>(type, stream.str());
}

std::tuple<uint, std::string>
TypeSerializer::serializeMatrix4x4(Any value)
{
	auto mat = Any::cast<math::mat4>(value);
	float* values = math::value_ptr(mat);
	std::stringstream stream;

	uint type = 0x00000000;

	type += MATRIX4X4 << 24;

	for (unsigned int i = 0; i < 16; ++i)
	{
		float compareValue = 0;

		if (i == 0 || i == 5 || i == 10 || i == 15)
			compareValue = 1;

		if (values[i] != compareValue)
		{
			type += 1u << i;
			//res.push_back(values[i]);
			write(stream, values[i]);
		}
	}

	return std::tuple<uint, std::string>(type, stream.str());
}

std::tuple<uint, std::string>
TypeSerializer::serializeBlending(Any value)
{
	render::Blending::Mode mode = Any::cast<render::Blending::Mode>(value);

	std::string res = "";

	if (mode == render::Blending::Mode::ADDITIVE)
		res = "+";
	else if (mode == render::Blending::Mode::ALPHA)
		res = "a";
	else 
		res = "d";

	uint type = 0x00000000;

	type += BLENDING << 24;

	return std::tuple<uint, std::string>(type, res);
}

std::tuple<uint, std::string>
TypeSerializer::serializeCulling(Any value)
{
	render::TriangleCulling tc = Any::cast<render::TriangleCulling>(value);

	std::string res = "";

	if (tc == render::TriangleCulling::BACK)
		res = "b";
	else if (tc == render::TriangleCulling::BOTH)
		res = "u";
	else if (tc == render::TriangleCulling::FRONT)
		res = "f";
	else if (tc == render::TriangleCulling::NONE)
		res = "n";
	else
		res = "b";

	uint type = 0x00000000;

	type += TRIANGLECULLING << 24;

	return std::tuple<uint, std::string>(type, res);
}

std::tuple<uint, std::string>
TypeSerializer::serializeTexture(Any value)
{
	short textureId = Any::cast<uint>(value);

	uint type = 0x00000000;

	type += TEXTURE << 24;

	type += textureId;

	return std::tuple<uint, std::string>(type, "");

}