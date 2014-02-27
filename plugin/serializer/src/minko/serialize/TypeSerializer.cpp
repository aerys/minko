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

#include "minko/serialize/TypeSerializer.hpp"
#include "minko/Any.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector2.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/Types.hpp"

using namespace minko;
using namespace minko::serialize;

std::tuple<uint, std::string>
TypeSerializer::serializeVector4(Any value)
{
	math::Vector4::Ptr	vect4 = Any::cast<std::shared_ptr<math::Vector4>>(value);
	std::vector<float>	res;
	uint				type		= 0x00000000;
	float				values[4]	= {vect4->x(), vect4->y(), vect4->z(), vect4->w()};
	std::stringstream	stream;

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
	math::Vector3::Ptr vect3 = Any::cast<std::shared_ptr<math::Vector3>>(value);
	std::vector<float>	res;
	uint				type		= 0x00000000;
	float				values[3]	= {vect3->x(), vect3->y(), vect3->z()};
	std::stringstream	stream;

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
	math::Vector2::Ptr	vect2 = Any::cast<std::shared_ptr<math::Vector2>>(value);
	std::vector<float>	res;
	uint				type		= 0x00000000;
	float				values[2]	= {vect2->x(), vect2->y()};
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
	math::Matrix4x4::Ptr		mat		= Any::cast<std::shared_ptr<math::Matrix4x4>>(value);
	const std::vector<float>&	values	= mat->values();
	std::vector<float>			res;
	std::stringstream			stream;

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