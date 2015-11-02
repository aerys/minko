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

#include "GeometryTest.hpp"

#include "minko/MinkoTests.hpp"

using namespace minko;
using namespace minko::geometry;
using namespace minko::render;

TEST_F(GeometryTest, Create)
{
	try
	{
		auto g = Geometry::create();
	}
	catch (...)
	{
		ASSERT_TRUE(false);
	}
}

TEST_F(GeometryTest, AddVertexBuffer)
{
	auto g = Geometry::create();
	float vertices[9] = {0.f, .5f, 0.f, .5f, -.5f, .0f, -.5f, -.5f, 0.f};
	auto vb = render::VertexBuffer::create(MinkoTests::canvas()->context(), std::begin(vertices), std::end(vertices));

	vb->addAttribute("position", 3);

	g->addVertexBuffer(vb);

	ASSERT_TRUE(std::find(g->vertexBuffers().begin(), g->vertexBuffers().end(), vb) != g->vertexBuffers().end());
}

TEST_F(GeometryTest, RemoveVertexBuffer)
{
	auto g = Geometry::create();
	float vertices[9] = {0.f, .5f, 0.f, .5f, -.5f, .0f, -.5f, -.5f, 0.f};
	auto vb = render::VertexBuffer::create(MinkoTests::canvas()->context(), std::begin(vertices), std::end(vertices));

	vb->addAttribute("position", 3);

	g->addVertexBuffer(vb);
	g->removeVertexBuffer(vb);

	ASSERT_FALSE(std::find(g->vertexBuffers().begin(), g->vertexBuffers().end(), vb) != g->vertexBuffers().end());
}

TEST_F(GeometryTest, RemoveVertexBufferByAttributeName)
{
	auto g = Geometry::create();
	float vertices[9] = {0.f, .5f, 0.f, .5f, -.5f, .0f, -.5f, -.5f, 0.f};
	auto vb = render::VertexBuffer::create(MinkoTests::canvas()->context(), std::begin(vertices), std::end(vertices));

	vb->addAttribute("position", 3);

	g->addVertexBuffer(vb);
	g->removeVertexBuffer("position");

	ASSERT_FALSE(std::find(g->vertexBuffers().begin(), g->vertexBuffers().end(), vb) != g->vertexBuffers().end());
}

TEST_F(GeometryTest, HasVertexBuffer)
{
	auto g = Geometry::create();
	float vertices[9] = {0.f, .5f, 0.f, .5f, -.5f, .0f, -.5f, -.5f, 0.f};
	auto vb = render::VertexBuffer::create(MinkoTests::canvas()->context(), std::begin(vertices), std::end(vertices));

	vb->addAttribute("position", 3);

	g->addVertexBuffer(vb);

	ASSERT_TRUE(g->hasVertexBuffer(vb));
}

TEST_F(GeometryTest, HasVertexAttribute)
{
	auto g = Geometry::create();
	float vertices[9] = {0.f, .5f, 0.f, .5f, -.5f, .0f, -.5f, -.5f, 0.f};
	auto vb = render::VertexBuffer::create(MinkoTests::canvas()->context(), std::begin(vertices), std::end(vertices));

	vb->addAttribute("position", 3);

	g->addVertexBuffer(vb);

	ASSERT_TRUE(g->hasVertexAttribute("position"));
}

TEST_F(GeometryTest, VertexBufferAddedInData)
{
	auto g = Geometry::create();
	float vertices[9] = {0.f, .5f, 0.f, .5f, -.5f, .0f, -.5f, -.5f, 0.f};
	auto vb = render::VertexBuffer::create(MinkoTests::canvas()->context(), std::begin(vertices), std::end(vertices));

	vb->addAttribute("position", 3);

	g->addVertexBuffer(vb);

	ASSERT_TRUE(g->data()->hasProperty("position"));
	ASSERT_EQ(g->data()->get<render::VertexAttribute>("position"), vb->attribute("position"));
}

TEST_F(GeometryTest, VertexBufferRemovedFromData)
{
	auto g = Geometry::create();
	float vertices[9] = {0.f, .5f, 0.f, .5f, -.5f, .0f, -.5f, -.5f, 0.f};
	auto vb = render::VertexBuffer::create(MinkoTests::canvas()->context(), std::begin(vertices), std::end(vertices));

	vb->addAttribute("position", 3);

	g->addVertexBuffer(vb);
	g->removeVertexBuffer(vb);

	ASSERT_FALSE(g->data()->hasProperty("position"));
}

TEST_F(GeometryTest, VertexAttributeOffset)
{
    int numVertices = 3;
    float data[9] = { 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f };
    auto vb = render::VertexBuffer::create(MinkoTests::canvas()->context(), std::begin(data), std::end(data));

    vb->addAttribute("a", 1);
    vb->addAttribute("b", 2);

    float counter = 0.f;
    for (auto i = 0; i < numVertices; i++)
    {
        auto vertexsize = vb->vertexSize();
        for (auto attribute : vb->attributes())
        {
            for (auto j = 0; j < attribute.size; j++)
            {
                auto value = vb->data()[i * vertexsize + attribute.offset + j];
                ASSERT_EQ(value, counter);
                counter++;
            }
        }
    }
}

TEST_F(GeometryTest, ComputeNotExistingNormals)
{
    auto context = MinkoTests::canvas()->context();

    std::vector<float> expectedNormalData = {
        // top
        0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,
        // bottom
        0.f, -1.f, 0.f,
        0.f, -1.f, 0.f,
        0.f, -1.f, 0.f,
        0.f, -1.f, 0.f,
        0.f, -1.f, 0.f,
        0.f, -1.f, 0.f,
        // front
        0.f, 0.f, -1.f,
        0.f, 0.f, -1.f,
        0.f, 0.f, -1.f,
        0.f, 0.f, -1.f,
        0.f, 0.f, -1.f,
        0.f, 0.f, -1.f,
        // back
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        // left
        -1.f, 0.f, 0.f,
        -1.f, 0.f, 0.f,
        -1.f, 0.f, 0.f,
        -1.f, 0.f, 0.f,
        -1.f, 0.f, 0.f,
        -1.f, 0.f, 0.f,
        // right
        1.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 0.f, 0.f
    };

    std::vector<float> geometryData =
    {
        // top
        0.5, 0.5, -0.5, 1.f, 0.f,
        -0.5, 0.5, 0.5, 0.f, 1.f,
        0.5, 0.5, 0.5, 1.f, 1.f,
        0.5, 0.5, -0.5, 1.f, 0.f,
        -0.5, 0.5, -0.5, 0.f, 0.f,
        -0.5, 0.5, 0.5, 0.f, 1.f,
        // bottom
        -0.5, -0.5, 0.5,0.f, 0.f,
        0.5, -0.5, -0.5, 1., 1.,
        0.5, -0.5, 0.5, 1.f, 0.,
        -0.5, -0.5, -0.5, 0.f, 1.f,
        0.5, -0.5, -0.5, 1.f, 1.f,
        -0.5, -0.5, 0.5, 0.f, 0.f,
        // front
        0.5, -0.5, -0.5, 0.f, 1.f,
        -0.5, 0.5, -0.5, 1.f, 0.f,
        0.5, 0.5, -0.5, 0.f, 0.f,
        -0.5, 0.5, -0.5, 1.f, 0.f,
        0.5, -0.5, -0.5, 0.f, 1.f,
        -0.5, -0.5, -0.5, 1.f, 1.f,
        // back
        -0.5, 0.5, 0.5, 0.f, 0.f,
        -0.5, -0.5, 0.5, 0.f, 1.f,
        0.5, 0.5, 0.5, 1.f, 0.f,
        -0.5, -0.5, 0.5, 0.f, 1.f,
        0.5, -0.5, 0.5, 1.f, 1.f,
        0.5, 0.5, 0.5, 1.f, 0.f,
        // left
        -0.5, -0.5, 0.5, 1.f, 1.f,
        -0.5, 0.5, -0.5, 0.f, 0.f,
        -0.5, -0.5, -0.5, 0.f, 1.f,
        -0.5, 0.5, -0.5, 0.f, 0.f,
        -0.5, -0.5, 0.5, 1.f, 1.f,
        -0.5, 0.5, 0.5, 1.f, 0.f,
        // right
        0.5, -0.5, -0.5, 1.f, 1.f,
        0.5, 0.5, -0.5, 1.f, 0.f,
        0.5, 0.5, 0.5, 0.f, 0.f,
        0.5, 0.5, 0.5, 0.f, 0.f,
        0.5, -0.5, 0.5, 0.f, 1.f,
        0.5, -0.5, -0.5, 1.f, 1.f
    };

    unsigned short i[] = {
        0, 1, 2, 3, 4, 5,
        6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35
    };

    auto geometry = Geometry::create();

    auto vertexBuffer = VertexBuffer::create(context, std::begin(geometryData), std::end(geometryData));

    vertexBuffer->addAttribute("position", 3, 0);
    vertexBuffer->addAttribute("uv", 2, 3);

    geometry->addVertexBuffer(vertexBuffer);

    geometry->indices(IndexBuffer::create(context, std::begin(i), std::end(i)));

    geometry->computeNormals();

    const auto& normalData = geometry->vertexBuffer("normal")->data();

    ASSERT_EQ(normalData.size(), expectedNormalData.size());
    ASSERT_TRUE(std::equal(normalData.begin(), normalData.end(), expectedNormalData.begin()));
}

TEST_F(GeometryTest, ComputeExistingNormals)
{
    auto context = MinkoTests::canvas()->context();

    std::vector<float> geometryData = {
        // top
        0.5, 0.5, -0.5,     0.f, -1.f, 0.f,     1.f, 0.f,
        -0.5, 0.5, 0.5,     0.f, 1.f, 0.f,      0.f, 1.f,
        0.5, 0.5, 0.5,      0.f, 1.f, 0.f,      1.f, 1.f,
        0.5, 0.5, -0.5,     0.f, 1.f, 0.f,      1.f, 0.f,
        -0.5, 0.5, -0.5,    0.f, 1.f, 0.f,      0.f, 0.f,
        -0.5, 0.5, 0.5,     0.f, 1.f, 0.f,      0.f, 1.f,
        // bottom
        -0.5, -0.5, 0.5,    0.f, -1.f, 0.f,     0.f, 0.f,
        0.5, -0.5, -0.5,    0.f, -1.f, 0.f,     1., 1.,
        0.5, -0.5, 0.5,     0.f, -1.f, 0.f,     1.f, 0.,
        -0.5, -0.5, -0.5,   0.f, -1.f, 0.f,     0.f, 1.f,
        0.5, -0.5, -0.5,    0.f, -1.f, 0.f,     1.f, 1.f,
        -0.5, -0.5, 0.5,    0.f, -1.f, 0.f,     0.f, 0.f,
        // front
        0.5, -0.5, -0.5,    0.f, 0.f, -1.f,     0.f, 1.f,
        -0.5, 0.5, -0.5,    0.f, 0.f, -1.f,     1.f, 0.f,
        0.5, 0.5, -0.5,     0.f, 0.f, -1.f,     0.f, 0.f,
        -0.5, 0.5, -0.5,    0.f, 0.f, -1.f,     1.f, 0.f,
        0.5, -0.5, -0.5,    0.f, 0.f, -1.f,     0.f, 1.f,
        -0.5, -0.5, -0.5,   0.f, 0.f, -1.f,     1.f, 1.f,
        // back
        -0.5, 0.5, 0.5,     0.f, 0.f, 1.f,      0.f, 0.f,
        -0.5, -0.5, 0.5,    0.f, 0.f, 1.f,      0.f, 1.f,
        0.5, 0.5, 0.5,      0.f, 0.f, 1.f,      1.f, 0.f,
        -0.5, -0.5, 0.5,    0.f, 0.f, 1.f,      0.f, 1.f,
        0.5, -0.5, 0.5,     0.f, 0.f, 1.f,      1.f, 1.f,
        0.5, 0.5, 0.5,      0.f, 0.f, 1.f,      1.f, 0.f,
        // left
        -0.5, -0.5, 0.5,    -1.f, 0.f, 0.f,     1.f, 1.f,
        -0.5, 0.5, -0.5,    -1.f, 0.f, 0.f,     0.f, 0.f,
        -0.5, -0.5, -0.5,   -1.f, 0.f, 0.f,     0.f, 1.f,
        -0.5, 0.5, -0.5,    -1.f, 0.f, 0.f,     0.f, 0.f,
        -0.5, -0.5, 0.5,    -1.f, 0.f, 0.f,     1.f, 1.f,
        -0.5, 0.5, 0.5,     -1.f, 0.f, 0.f,     1.f, 0.f,
        // right
        0.5, -0.5, -0.5,    1.f, 0.f, 0.f,      1.f, 1.f,
        0.5, 0.5, -0.5,     1.f, 0.f, 0.f,      1.f, 0.f,
        0.5, 0.5, 0.5,      1.f, 0.f, 0.f,      0.f, 0.f,
        0.5, 0.5, 0.5,      1.f, 0.f, 0.f,      0.f, 0.f,
        0.5, -0.5, 0.5,     1.f, 0.f, 0.f,      0.f, 1.f,
        0.5, -0.5, -0.5,    1.f, 0.f, 0.f,      1.f, 1.f
    };

    std::vector<float> expectedNormalData = {
        // top
        0.5, 0.5, -0.5,     0.f, 1.f, 0.f,      1.f, 0.f,
        -0.5, 0.5, 0.5,     0.f, 1.f, 0.f,      0.f, 1.f,
        0.5, 0.5, 0.5,      0.f, 1.f, 0.f,      1.f, 1.f,
        0.5, 0.5, -0.5,     0.f, 1.f, 0.f,      1.f, 0.f,
        -0.5, 0.5, -0.5,    0.f, 1.f, 0.f,      0.f, 0.f,
        -0.5, 0.5, 0.5,     0.f, 1.f, 0.f,      0.f, 1.f,
        // bottom
        -0.5, -0.5, 0.5,    0.f, -1.f, 0.f,     0.f, 0.f,
        0.5, -0.5, -0.5,    0.f, -1.f, 0.f,     1., 1.,
        0.5, -0.5, 0.5,     0.f, -1.f, 0.f,     1.f, 0.,
        -0.5, -0.5, -0.5,   0.f, -1.f, 0.f,     0.f, 1.f,
        0.5, -0.5, -0.5,    0.f, -1.f, 0.f,     1.f, 1.f,
        -0.5, -0.5, 0.5,    0.f, -1.f, 0.f,     0.f, 0.f,
        // front
        0.5, -0.5, -0.5,    0.f, 0.f, -1.f,     0.f, 1.f,
        -0.5, 0.5, -0.5,    0.f, 0.f, -1.f,     1.f, 0.f,
        0.5, 0.5, -0.5,     0.f, 0.f, -1.f,     0.f, 0.f,
        -0.5, 0.5, -0.5,    0.f, 0.f, -1.f,     1.f, 0.f,
        0.5, -0.5, -0.5,    0.f, 0.f, -1.f,     0.f, 1.f,
        -0.5, -0.5, -0.5,   0.f, 0.f, -1.f,     1.f, 1.f,
        // back
        -0.5, 0.5, 0.5,     0.f, 0.f, 1.f,      0.f, 0.f,
        -0.5, -0.5, 0.5,    0.f, 0.f, 1.f,      0.f, 1.f,
        0.5, 0.5, 0.5,      0.f, 0.f, 1.f,      1.f, 0.f,
        -0.5, -0.5, 0.5,    0.f, 0.f, 1.f,      0.f, 1.f,
        0.5, -0.5, 0.5,     0.f, 0.f, 1.f,      1.f, 1.f,
        0.5, 0.5, 0.5,      0.f, 0.f, 1.f,      1.f, 0.f,
        // left
        -0.5, -0.5, 0.5,    -1.f, 0.f, 0.f,     1.f, 1.f,
        -0.5, 0.5, -0.5,    -1.f, 0.f, 0.f,     0.f, 0.f,
        -0.5, -0.5, -0.5,   -1.f, 0.f, 0.f,     0.f, 1.f,
        -0.5, 0.5, -0.5,    -1.f, 0.f, 0.f,     0.f, 0.f,
        -0.5, -0.5, 0.5,    -1.f, 0.f, 0.f,     1.f, 1.f,
        -0.5, 0.5, 0.5,     -1.f, 0.f, 0.f,     1.f, 0.f,
        // right
        0.5, -0.5, -0.5,    1.f, 0.f, 0.f,      1.f, 1.f,
        0.5, 0.5, -0.5,     1.f, 0.f, 0.f,      1.f, 0.f,
        0.5, 0.5, 0.5,      1.f, 0.f, 0.f,      0.f, 0.f,
        0.5, 0.5, 0.5,      1.f, 0.f, 0.f,      0.f, 0.f,
        0.5, -0.5, 0.5,     1.f, 0.f, 0.f,      0.f, 1.f,
        0.5, -0.5, -0.5,    1.f, 0.f, 0.f,      1.f, 1.f
    };

    unsigned short i[] = {
        0, 1, 2, 3, 4, 5,
        6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35
    };

    auto geometry = Geometry::create();

    auto vertexBuffer = VertexBuffer::create(context, std::begin(geometryData), std::end(geometryData));

    vertexBuffer->addAttribute("position", 3, 0);
    vertexBuffer->addAttribute("normal", 3, 3);
    vertexBuffer->addAttribute("uv", 2, 6);

    geometry->addVertexBuffer(vertexBuffer);

    geometry->indices(IndexBuffer::create(context, std::begin(i), std::end(i)));

    geometry->computeNormals();

    const auto& normalData = geometry->vertexBuffer("normal")->data();

    ASSERT_EQ(normalData.size(), expectedNormalData.size());
    ASSERT_TRUE(std::equal(normalData.begin(), normalData.end(), expectedNormalData.begin()));
}
