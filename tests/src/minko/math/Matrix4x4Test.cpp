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

#include "minko/math/Matrix4x4Test.hpp"

#include "minko/math/Matrix4x4.hpp"
#include "minko/math/Vector3.hpp"

using namespace minko::math;

TEST_F(Matrix4x4Test, Create)
{
	try
	{
		auto m = Matrix4x4::create();
	}
	catch (...)
	{
		ASSERT_TRUE(false);
	}
}

TEST_F(Matrix4x4Test, CreateCopy)
{
	auto m1 = Matrix4x4::create();

	m1->initialize(
		rand(), rand(), rand(), rand(),
		rand(), rand(), rand(), rand(),
		rand(), rand(), rand(), rand(),
		rand(), rand(), rand(), rand()
	);

	auto m2 = Matrix4x4::create(m1);

	ASSERT_TRUE(m1->equals(m2));
	ASSERT_TRUE(m1->data() == m2->data());
}

TEST_F(Matrix4x4Test, Identity)
{
	auto m1 = Matrix4x4::create()->initialize(
		rand(), rand(), rand(), rand(),
		rand(), rand(), rand(), rand(),
		rand(), rand(), rand(), rand(),
		rand(), rand(), rand(), rand()
	);

	m1->identity();

	ASSERT_EQ(m1->data()[0], 1.f);
	ASSERT_EQ(m1->data()[1], 0.f);
	ASSERT_EQ(m1->data()[2], 0.f);
	ASSERT_EQ(m1->data()[3], 0.f);
	ASSERT_EQ(m1->data()[4], 0.f);
	ASSERT_EQ(m1->data()[5], 1.f);
	ASSERT_EQ(m1->data()[6], 0.f);
	ASSERT_EQ(m1->data()[7], 0.f);
	ASSERT_EQ(m1->data()[8], 0.f);
	ASSERT_EQ(m1->data()[9], 0.f);
	ASSERT_EQ(m1->data()[10], 1.f);
	ASSERT_EQ(m1->data()[11], 0.f);
	ASSERT_EQ(m1->data()[12], 0.f);
	ASSERT_EQ(m1->data()[13], 0.f);
	ASSERT_EQ(m1->data()[14], 0.f);
	ASSERT_EQ(m1->data()[15], 1.f);
}

TEST_F(Matrix4x4Test, AppendTranslation)
{
	auto m = Matrix4x4::create();
	auto x = rand();
	auto y = rand();
	auto z = rand();

	m->appendTranslation(x, y, z);

	ASSERT_EQ(m->data()[3], x);
	ASSERT_EQ(m->data()[7], y);
	ASSERT_EQ(m->data()[11], z);
}

TEST_F(Matrix4x4Test, PrependTranslation)
{
	auto m = Matrix4x4::create();
	auto x = rand();
	auto y = rand();
	auto z = rand();

	m->appendTranslation(x, y, z);

	ASSERT_EQ(m->data()[3], x);
	ASSERT_EQ(m->data()[7], y);
	ASSERT_EQ(m->data()[11], z);
}

TEST_F(Matrix4x4Test, InvertIdentity)
{
	auto m1 = Matrix4x4::create();
	auto m2 = Matrix4x4::create();

	m2->invert();

	ASSERT_EQ(m1->data(), m2->data());
}
