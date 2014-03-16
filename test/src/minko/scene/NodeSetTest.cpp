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

#include "minko/scene/NodeSetTest.hpp"

using namespace minko;
using namespace minko::scene;

TEST_F(NodeSetTest, DescendantsBreadthFirst)
{
	auto scene = Node::create()
		->addChild(Node::create())
		->addChild(Node::create());
	auto nodeSet = NodeSet::create(scene)->descendants(true, false);

	ASSERT_EQ(nodeSet->nodes()[0], scene);
	ASSERT_EQ(nodeSet->nodes()[1], scene->children()[0]);
	ASSERT_EQ(nodeSet->nodes()[2], scene->children()[1]);
}

TEST_F(NodeSetTest, DescendantsBreadthFirst2)
{
	auto scene = Node::create()
		->addChild(Node::create()
				   ->addChild(Node::create()))
		->addChild(Node::create());
	auto nodeSet = NodeSet::create(scene)->descendants(true, false);

	ASSERT_EQ(nodeSet->nodes()[0], scene);
	ASSERT_EQ(nodeSet->nodes()[1], scene->children()[0]);
	ASSERT_EQ(nodeSet->nodes()[2], scene->children()[1]);
	ASSERT_EQ(nodeSet->nodes()[3], scene->children()[0]->children()[0]);
}

TEST_F(NodeSetTest, DescendantsBreadthFirst3)
{
	auto scene = Node::create()
		->addChild(Node::create()
				   ->addChild(Node::create())
				   ->addChild(Node::create()))
		->addChild(Node::create());
	auto nodeSet = NodeSet::create(scene)->descendants(true, false);

	ASSERT_EQ(nodeSet->nodes()[0], scene);
	ASSERT_EQ(nodeSet->nodes()[1], scene->children()[0]);
	ASSERT_EQ(nodeSet->nodes()[2], scene->children()[1]);
	ASSERT_EQ(nodeSet->nodes()[3], scene->children()[0]->children()[0]);
	ASSERT_EQ(nodeSet->nodes()[4], scene->children()[0]->children()[1]);
}

TEST_F(NodeSetTest, DescendantsBreadthFirst4)
{
	auto scene = Node::create()
		->addChild(Node::create()
				   ->addChild(Node::create()
							  ->addChild(Node::create()))
				   ->addChild(Node::create()))
		->addChild(Node::create()
				   ->addChild(Node::create()));
	auto nodeSet = NodeSet::create(scene)->descendants(true, false);

	ASSERT_EQ(nodeSet->nodes()[0], scene);
	ASSERT_EQ(nodeSet->nodes()[1], scene->children()[0]);
	ASSERT_EQ(nodeSet->nodes()[2], scene->children()[1]);
	ASSERT_EQ(nodeSet->nodes()[3], scene->children()[0]->children()[0]);
	ASSERT_EQ(nodeSet->nodes()[4], scene->children()[0]->children()[1]);
	ASSERT_EQ(nodeSet->nodes()[5], scene->children()[1]->children()[0]);
	ASSERT_EQ(nodeSet->nodes()[6], scene->children()[0]->children()[0]->children()[0]);
}

TEST_F(NodeSetTest, DescendantsBreadthFirst5)
{
	auto scene = Node::create()
		->addChild(Node::create()
				   ->addChild(Node::create()
							  ->addChild(Node::create()
										 ->addChild(Node::create()
													->addChild(Node::create()
															   ->addChild(Node::create()))))));
	auto nodeSet = NodeSet::create(scene)->descendants(true, false);

	for (auto i = 0; i < 6; ++i)
	{
		ASSERT_EQ(nodeSet->nodes()[i], scene);
		scene = scene->children()[0];
	}
}
