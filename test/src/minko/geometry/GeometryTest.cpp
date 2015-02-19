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
