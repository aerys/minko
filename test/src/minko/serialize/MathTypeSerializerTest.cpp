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

#include "minko/serialize/MathTypeSerializerTest.hpp"
#include "minko/serialize/TypeSerializer.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::serialize;
using namespace minko::deserialize;

TEST_F(MathTypeSerializerTest, DefaultVector4)
{
	auto vec = Vector4::create();
	std::tuple<uint, std::string> vecSerialized = TypeSerializer::serializeVector4(Any(vec));
	std::tuple<uint, std::string&> vecSerialized2(std::get<0>(vecSerialized), std::get<1>(vecSerialized));
	Vector4::Ptr vecResult = Any::cast<Vector4::Ptr>(TypeDeserializer::deserializeVector4(vecSerialized2));

	ASSERT_TRUE(vec->equals(vecResult));
}

TEST_F(MathTypeSerializerTest, RandomVector4)
{
	auto vec = Vector4::create(
		MathTypeSerializerTest::random(), 
		MathTypeSerializerTest::random(), 
		MathTypeSerializerTest::random(), 
		MathTypeSerializerTest::random());
	std::tuple<uint, std::string> vecSerialized = TypeSerializer::serializeVector4(Any(vec));
	std::tuple<uint, std::string&> vecSerialized2(std::get<0>(vecSerialized), std::get<1>(vecSerialized));
	Vector4::Ptr vecResult = Any::cast<Vector4::Ptr>(TypeDeserializer::deserializeVector4(vecSerialized2));

	ASSERT_TRUE(vec->equals(vecResult));
}

TEST_F(MathTypeSerializerTest, DefaultVector3)
{
	auto vec = Vector3::create();
	std::tuple<uint, std::string> vecSerialized = TypeSerializer::serializeVector3(Any(vec));
	std::tuple<uint, std::string&> vecSerialized2(std::get<0>(vecSerialized), std::get<1>(vecSerialized));
	Vector3::Ptr vecResult = Any::cast<Vector3::Ptr>(TypeDeserializer::deserializeVector3(vecSerialized2));

	ASSERT_TRUE(vec->equals(vecResult));
}

TEST_F(MathTypeSerializerTest, RandomVector3)
{
	auto vec = Vector3::create(
		MathTypeSerializerTest::random(),
		MathTypeSerializerTest::random(),
		MathTypeSerializerTest::random());
	std::tuple<uint, std::string> vecSerialized = TypeSerializer::serializeVector3(Any(vec));
	std::tuple<uint, std::string&> vecSerialized2(std::get<0>(vecSerialized), std::get<1>(vecSerialized));

	Vector3::Ptr vecResult = Any::cast<Vector3::Ptr>(TypeDeserializer::deserializeVector3(vecSerialized2));

	ASSERT_TRUE(vec->equals(vecResult));
}

TEST_F(MathTypeSerializerTest, DefaultVector2)
{
	auto vec = Vector2::create();
	std::tuple<uint, std::string> vecSerialized = TypeSerializer::serializeVector2(Any(vec));
	std::tuple<uint, std::string&> vecSerialized2(std::get<0>(vecSerialized), std::get<1>(vecSerialized));

	Vector2::Ptr vecResult = Any::cast<Vector2::Ptr>(TypeDeserializer::deserializeVector2(vecSerialized2));

	ASSERT_TRUE(vec->equals(vecResult));
}

TEST_F(MathTypeSerializerTest, RandomVector2)
{
	auto vec = Vector2::create(
		MathTypeSerializerTest::random(),
		MathTypeSerializerTest::random());
	std::tuple<uint, std::string> vecSerialized = TypeSerializer::serializeVector2(Any(vec));
	std::tuple<uint, std::string&> vecSerialized2(std::get<0>(vecSerialized), std::get<1>(vecSerialized));

	Vector2::Ptr vecResult = Any::cast<Vector2::Ptr>(TypeDeserializer::deserializeVector2(vecSerialized2));

	ASSERT_TRUE(vec->equals(vecResult));
}

TEST_F(MathTypeSerializerTest, DefaultMatrix4x4)
{
	auto matrix = Matrix4x4::create();
	std::tuple<uint, std::string> matrixSerialized = TypeSerializer::serializeMatrix4x4(Any(matrix));
	std::tuple<uint, std::string&> matrixSerialized2(std::get<0>(matrixSerialized), std::get<1>(matrixSerialized));

	Matrix4x4::Ptr matrixResult = Any::cast<Matrix4x4::Ptr>(TypeDeserializer::deserializeMatrix4x4(matrixSerialized2));

	ASSERT_TRUE(matrix->equals(matrixResult));
}

TEST_F(MathTypeSerializerTest, RandomMatrix4x4)
{
	auto matrix = Matrix4x4::create();
	matrix->initialize(
		MathTypeSerializerTest::random(), MathTypeSerializerTest::random(), MathTypeSerializerTest::random(), MathTypeSerializerTest::random(),
		MathTypeSerializerTest::random(), MathTypeSerializerTest::random(), MathTypeSerializerTest::random(), MathTypeSerializerTest::random(), 
		MathTypeSerializerTest::random(), MathTypeSerializerTest::random(), MathTypeSerializerTest::random(), MathTypeSerializerTest::random(), 
		MathTypeSerializerTest::random(), MathTypeSerializerTest::random(), MathTypeSerializerTest::random(), MathTypeSerializerTest::random());
	std::tuple<uint, std::string> matrixSerialized = TypeSerializer::serializeMatrix4x4(Any(matrix));
	std::tuple<uint, std::string&> matrixSerialized2(std::get<0>(matrixSerialized), std::get<1>(matrixSerialized));

	Matrix4x4::Ptr matrixResult = Any::cast<Matrix4x4::Ptr>(TypeDeserializer::deserializeMatrix4x4(matrixSerialized2));

	ASSERT_TRUE(matrix->equals(matrixResult));
}