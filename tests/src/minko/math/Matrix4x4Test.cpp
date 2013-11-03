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
		random(), random(), random(), random(),
		random(), random(), random(), random(),
		random(), random(), random(), random(),
		random(), random(), random(), random()
	);

	auto m2 = Matrix4x4::create(m1);

	ASSERT_TRUE(m1->equals(m2));
	ASSERT_TRUE(m1->data() == m2->data());
}

TEST_F(Matrix4x4Test, Identity)
{
	auto m1 = Matrix4x4::create()->initialize(
		random(), random(), random(), random(),
		random(), random(), random(), random(),
		random(), random(), random(), random(),
		random(), random(), random(), random()
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

TEST_F(Matrix4x4Test, DeterminantIdentity)
{
	auto m = Matrix4x4::create();

	ASSERT_EQ(m->determinant(), 1.f);
}

TEST_F(Matrix4x4Test, AppendTranslation)
{
	auto m = Matrix4x4::create();
	auto x = random();
	auto y = random();
	auto z = random();

	m->appendTranslation(x, y, z);

	ASSERT_EQ(m->data()[3], x);
	ASSERT_EQ(m->data()[7], y);
	ASSERT_EQ(m->data()[11], z);
}

TEST_F(Matrix4x4Test, AppendTranslations)
{
	auto m = Matrix4x4::create();
	auto x = random();
	auto y = random();
	auto z = random();

	m->appendTranslation(x, y, z);
	m->appendTranslation(y, z, x);
	m->appendTranslation(z, x, y);

	ASSERT_TRUE(nearEqual(m->data()[3], x + y + z));
	ASSERT_TRUE(nearEqual(m->data()[7], x + y + z));
	ASSERT_TRUE(nearEqual(m->data()[11], x + y + z));
}

TEST_F(Matrix4x4Test, PrependTranslation)
{
	auto m = Matrix4x4::create();
	auto x = random();
	auto y = random();
	auto z = random();

	m->appendTranslation(x, y, z);

	ASSERT_EQ(m->data()[3], x);
	ASSERT_EQ(m->data()[7], y);
	ASSERT_EQ(m->data()[11], z);
}

TEST_F(Matrix4x4Test, PrependTranslations)
{
	auto m = Matrix4x4::create();
	auto x = random();
	auto y = random();
	auto z = random();

	m->prependTranslation(x, y, z);
	m->prependTranslation(y, z, x);
	m->prependTranslation(z, x, y);

	ASSERT_TRUE(nearEqual(m->data()[3], x + y + z));
	ASSERT_TRUE(nearEqual(m->data()[7], x + y + z));
	ASSERT_TRUE(nearEqual(m->data()[11], x + y + z));
}

TEST_F(Matrix4x4Test, InvertIdentity)
{
	auto m1 = Matrix4x4::create();
	auto m2 = Matrix4x4::create();

	m2->invert();

	ASSERT_EQ(m1->data(), m2->data());
}

TEST_F(Matrix4x4Test, InvertTranslation)
{
	auto m = Matrix4x4::create();
	auto x = random();
	auto y = random();
	auto z = random();

	m->appendTranslation(x, y, z);
	m->invert();

	ASSERT_EQ(m->data()[3], -x);
	ASSERT_EQ(m->data()[7], -y);
	ASSERT_EQ(m->data()[11], -z);
}

TEST_F(Matrix4x4Test, TransposeIdentity)
{
	auto m1 = Matrix4x4::create();
	auto m2 = Matrix4x4::create();

	m2->transpose();

	ASSERT_EQ(m1->data(), m2->data());
}

TEST_F(Matrix4x4Test, Transpose)
{
	std::vector<float> d = {
		random(), random(), random(), random(),
		random(), random(), random(), random(),
		random(), random(), random(), random(),
		random(), random(), random(), random()
	};
	auto m = Matrix4x4::create()->initialize(
		d[0], 	d[1], 	d[2], 	d[3],
		d[4], 	d[5], 	d[6], 	d[7],
		d[8], 	d[9], 	d[10], 	d[11],
		d[12], 	d[13], 	d[14], 	d[15]
	);

	m->transpose();

	ASSERT_EQ(m->data()[0], d[0]);
	ASSERT_EQ(m->data()[1], d[4]);
	ASSERT_EQ(m->data()[2], d[8]);
	ASSERT_EQ(m->data()[3], d[12]);
	ASSERT_EQ(m->data()[4], d[1]);
	ASSERT_EQ(m->data()[5], d[5]);
	ASSERT_EQ(m->data()[6], d[9]);
	ASSERT_EQ(m->data()[7], d[13]);
	ASSERT_EQ(m->data()[8], d[2]);
	ASSERT_EQ(m->data()[9], d[6]);
	ASSERT_EQ(m->data()[10], d[10]);
	ASSERT_EQ(m->data()[11], d[14]);
	ASSERT_EQ(m->data()[12], d[3]);
	ASSERT_EQ(m->data()[13], d[7]);
	ASSERT_EQ(m->data()[14], d[11]);
	ASSERT_EQ(m->data()[15], d[15]);
}

TEST_F(Matrix4x4Test, TransformTranslation)
{
	auto m = Matrix4x4::create();
	auto x1 = random();
	auto y1 = random();
	auto z1 = random();
	auto x2 = random();
	auto y2 = random();
	auto z2 = random();

	m->appendTranslation(x1, y1, z1);

	auto v = m->transform(Vector3::create(x2, y2, z2));

	ASSERT_EQ(v->x(), x1 + x2);
	ASSERT_EQ(v->y(), y1 + y2);
	ASSERT_EQ(v->z(), z1 + z2);
}

TEST_F(Matrix4x4Test, TransformRotationXPi)
{
	auto m = Matrix4x4::create();

	m->appendRotationX(PI);

	auto xAxis = m->transform(Vector3::xAxis());
	auto yNegAxis = m->transform(Vector3::yAxis());
	auto zNegAxis = m->transform(Vector3::zAxis());

	ASSERT_TRUE(nearEqual(xAxis->x(), 1.f));
	ASSERT_TRUE(nearEqual(xAxis->y(), 0.f));
	ASSERT_TRUE(nearEqual(xAxis->z(), 0.f));
	ASSERT_TRUE(nearEqual(yNegAxis->x(), 0.f));
	ASSERT_TRUE(nearEqual(yNegAxis->y(), -1.f));
	ASSERT_TRUE(nearEqual(yNegAxis->z(), 0.f));
	ASSERT_TRUE(nearEqual(zNegAxis->x(), 0.f));
	ASSERT_TRUE(nearEqual(zNegAxis->y(), 0.f));
	ASSERT_TRUE(nearEqual(zNegAxis->z(), -1.f));
}

TEST_F(Matrix4x4Test, TransformRotationYPi)
{
	auto m = Matrix4x4::create();

	m->appendRotationY(PI);

	auto xNegAxis = m->transform(Vector3::xAxis());
	auto yAxis = m->transform(Vector3::yAxis());
	auto zNegAxis = m->transform(Vector3::zAxis());

	ASSERT_TRUE(nearEqual(xNegAxis->x(), -1.f));
	ASSERT_TRUE(nearEqual(xNegAxis->y(), 0.f));
	ASSERT_TRUE(nearEqual(xNegAxis->z(), 0.f));
	ASSERT_TRUE(nearEqual(yAxis->x(), 0.f));
	ASSERT_TRUE(nearEqual(yAxis->y(), 1.f));
	ASSERT_TRUE(nearEqual(yAxis->z(), 0.f));
	ASSERT_TRUE(nearEqual(zNegAxis->x(), 0.f));
	ASSERT_TRUE(nearEqual(zNegAxis->y(), 0.f));
	ASSERT_TRUE(nearEqual(zNegAxis->z(), -1.f));
}

TEST_F(Matrix4x4Test, TransformRotationZPi)
{
	auto m = Matrix4x4::create();

	m->appendRotationZ(PI);

	auto xNegAxis = m->transform(Vector3::xAxis());
	auto yNegAxis = m->transform(Vector3::yAxis());
	auto zAxis = m->transform(Vector3::zAxis());

	ASSERT_TRUE(nearEqual(xNegAxis->x(), -1.f));
	ASSERT_TRUE(nearEqual(xNegAxis->y(), 0.f));
	ASSERT_TRUE(nearEqual(xNegAxis->z(), 0.f));
	ASSERT_TRUE(nearEqual(yNegAxis->x(), 0.f));
	ASSERT_TRUE(nearEqual(yNegAxis->y(), -1.f));
	ASSERT_TRUE(nearEqual(yNegAxis->z(), 0.f));
	ASSERT_TRUE(nearEqual(zAxis->x(), 0.f));
	ASSERT_TRUE(nearEqual(zAxis->y(), 0.f));
	ASSERT_TRUE(nearEqual(zAxis->z(), 1.f));
}

TEST_F(Matrix4x4Test, TransformAppendRotationXRandom)
{
	auto m1 = Matrix4x4::create();
	auto m2 = Matrix4x4::create(m1);
	uint r = ((uint)((rand() / (float)RAND_MAX) * 100) / 2) * 2;
	std::vector<Vector3::Ptr> v;

	for (uint i = 0; i < r; ++i)
	{
		v.push_back(m1->transform(Vector3::zAxis()));
		m1->appendRotationX(PI * 2.f / (float)r);
	}

	for (auto i = 0; i < 16; ++i)
		ASSERT_TRUE(nearEqual(m1->data()[i], m2->data()[i]));
	for (auto i = 0; i < r / 2; ++i)
	{
		ASSERT_EQ(v[i]->x(), 0.f);
		ASSERT_EQ(-v[i + r / 2]->x(), 0.f);
		ASSERT_TRUE(nearEqual(v[i]->y(), -v[i + r / 2]->y()));
		ASSERT_TRUE(nearEqual(v[i]->z(), -v[i + r / 2]->z()));
	}
}

TEST_F(Matrix4x4Test, TransformAppendRotationYRandom)
{
	auto m1 = Matrix4x4::create();
	auto m2 = Matrix4x4::create(m1);
	uint r = ((uint)((rand() / (float)RAND_MAX) * 100) / 2) * 2;
	std::vector<Vector3::Ptr> v;

	for (uint i = 0; i < r; ++i)
	{
		v.push_back(m1->transform(Vector3::xAxis()));
		m1->appendRotationY(PI * 2.f / (float)r);
	}

	for (auto i = 0; i < 16; ++i)
		ASSERT_TRUE(nearEqual(m1->data()[i], m2->data()[i]));
	for (auto i = 0; i < r / 2; ++i)
	{
		ASSERT_EQ(v[i]->y(), 0.f);
		ASSERT_EQ(-v[i + r / 2]->y(), 0.f);
		ASSERT_TRUE(nearEqual(v[i]->x(), -v[i + r / 2]->x()));
		ASSERT_TRUE(nearEqual(v[i]->z(), -v[i + r / 2]->z()));
	}
}

TEST_F(Matrix4x4Test, TransformAppendRotationZRandom)
{
	auto m1 = Matrix4x4::create();
	auto m2 = Matrix4x4::create(m1);
	uint r = ((uint)((rand() / (float)RAND_MAX) * 100) / 2) * 2;
	std::vector<Vector3::Ptr> v;

	for (uint i = 0; i < r; ++i)
	{
		v.push_back(m1->transform(Vector3::yAxis()));
		m1->appendRotationZ(PI * 2.f / (float)r);
	}

	for (auto i = 0; i < 16; ++i)
		ASSERT_TRUE(nearEqual(m1->data()[i], m2->data()[i]));
	for (auto i = 0; i < r / 2; ++i)
	{
		ASSERT_EQ(v[i]->z(), 0.f);
		ASSERT_EQ(-v[i + r / 2]->z(), 0.f);
		ASSERT_TRUE(nearEqual(v[i]->x(), -v[i + r / 2]->x()));
		ASSERT_TRUE(nearEqual(v[i]->y(), -v[i + r / 2]->y()));
	}
}

TEST_F(Matrix4x4Test, TransformPrependRotationXRandom)
{
	auto m1 = Matrix4x4::create();
	auto m2 = Matrix4x4::create(m1);
	uint r = ((uint)((rand() / (float)RAND_MAX) * 100) / 2) * 2;
	std::vector<Vector3::Ptr> v;

	for (uint i = 0; i < r; ++i)
	{
		v.push_back(m1->transform(Vector3::zAxis()));
		m1->prependRotationX(PI * 2.f / (float)r);
	}

	for (auto i = 0; i < 16; ++i)
		ASSERT_TRUE(nearEqual(m1->data()[i], m2->data()[i]));
	for (auto i = 0; i < r / 2; ++i)
	{
		ASSERT_EQ(v[i]->x(), 0.f);
		ASSERT_EQ(-v[i + r / 2]->x(), 0.f);
		ASSERT_TRUE(nearEqual(v[i]->y(), -v[i + r / 2]->y()));
		ASSERT_TRUE(nearEqual(v[i]->z(), -v[i + r / 2]->z()));
	}
}

TEST_F(Matrix4x4Test, TransformPrependRotationYRandom)
{
	auto m1 = Matrix4x4::create();
	auto m2 = Matrix4x4::create(m1);
	uint r = ((uint)((rand() / (float)RAND_MAX) * 100) / 2) * 2;
	std::vector<Vector3::Ptr> v;

	for (uint i = 0; i < r; ++i)
	{
		v.push_back(m1->transform(Vector3::xAxis()));
		m1->prependRotationY(PI * 2.f / (float)r);
	}

	for (auto i = 0; i < 16; ++i)
		ASSERT_TRUE(nearEqual(m1->data()[i], m2->data()[i]));
	for (auto i = 0; i < r / 2; ++i)
	{
		ASSERT_EQ(v[i]->y(), 0.f);
		ASSERT_EQ(-v[i + r / 2]->y(), 0.f);
		ASSERT_TRUE(nearEqual(v[i]->x(), -v[i + r / 2]->x()));
		ASSERT_TRUE(nearEqual(v[i]->z(), -v[i + r / 2]->z()));
	}
}

TEST_F(Matrix4x4Test, TransformPrependRotationZRandom)
{
	auto m1 = Matrix4x4::create();
	auto m2 = Matrix4x4::create(m1);
	uint r = ((uint)((rand() / (float)RAND_MAX) * 100) / 2) * 2;
	std::vector<Vector3::Ptr> v;

	for (uint i = 0; i < r; ++i)
	{
		v.push_back(m1->transform(Vector3::yAxis()));
		m1->prependRotationZ(PI * 2.f / (float)r);
	}

	for (auto i = 0; i < 16; ++i)
		ASSERT_TRUE(nearEqual(m1->data()[i], m2->data()[i]));
	for (auto i = 0; i < r / 2; ++i)
	{
		ASSERT_EQ(v[i]->z(), 0.f);
		ASSERT_EQ(-v[i + r / 2]->z(), 0.f);
		ASSERT_TRUE(nearEqual(v[i]->x(), -v[i + r / 2]->x()));
		ASSERT_TRUE(nearEqual(v[i]->y(), -v[i + r / 2]->y()));
	}
}
