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

#include "minko/math/Vector4Test.hpp"

#include "minko/math/Vector4.hpp"

using namespace minko;
using namespace minko::math;

TEST_F(Vector4Test, Create)
{
	try
	{
		auto v = Vector4::create();
	}
	catch (...)
	{
		ASSERT_TRUE(false);
	}
}

TEST_F(Vector4Test, CreateCopy)
{
	auto x = random();
	auto y = random();
	auto z = random();
	auto u = Vector4::create(x, y, z);
	auto v = Vector4::create(u);

	ASSERT_EQ(x, v->x());
	ASSERT_EQ(y, v->y());
	ASSERT_EQ(z, v->z());
}

TEST_F(Vector4Test, GetX)
{
	auto x = random();
	auto y = random();
	auto z = random();
	auto v = Vector4::create(x, y, z);

	ASSERT_EQ(v->x(), x);
}

TEST_F(Vector4Test, GetY)
{
	auto x = random();
	auto y = random();
	auto z = random();
	auto v = Vector4::create(x, y, z);

	ASSERT_EQ(v->y(), y);
}

TEST_F(Vector4Test, GetZ)
{
	auto x = random();
	auto y = random();
	auto z = random();
	auto v = Vector4::create(x, y, z);

	ASSERT_EQ(v->z(), z);
}

TEST_F(Vector4Test, SetX)
{
	auto x = random();
	auto v = Vector4::create();

	v->x(x);

	ASSERT_EQ(v->x(), x);
}

TEST_F(Vector4Test, SetY)
{
	auto y = random();
	auto v = Vector4::create();

	v->y(y);

	ASSERT_EQ(v->y(), y);
}

TEST_F(Vector4Test, SetZ)
{
	auto z = random();
	auto v = Vector4::create();

	v->z(z);

	ASSERT_EQ(v->z(), z);
}
