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

#include "NodeTest.hpp"

#include "minko/MinkoTests.hpp"

using namespace minko;
using namespace minko::scene;

TEST_F(NodeTest, Create)
{
    try
    {
        auto node = Node::create();
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }
}

TEST_F(NodeTest, CreateWithName)
{
    Node::Ptr node;

    try
    {
        node = Node::create("test");
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }

    ASSERT_EQ(node->name(), "test");
}

TEST_F(NodeTest, AddChild)
{
    auto n1 = Node::create("a");
    auto n2 = Node::create("b");

    n1->addChild(n2);

    ASSERT_EQ(n1->children().size(), 1);
    ASSERT_EQ(n1->children()[0], n2);
    ASSERT_EQ(n1->root(), n1);
    ASSERT_EQ(n1->parent(), nullptr);
    ASSERT_EQ(n2->children().size(), 0);
    ASSERT_EQ(n2->parent(), n1);
    ASSERT_EQ(n2->root(), n1);
}

TEST_F(NodeTest, RemoveChild)
{
    auto n1 = Node::create("a");
    auto n2 = Node::create("b");

    n1->addChild(n2);
    n1->removeChild(n2);

    ASSERT_EQ(n1->children().size(), 0);
    ASSERT_EQ(n1->root(), n1);
    ASSERT_EQ(n1->parent(), nullptr);
    ASSERT_EQ(n2->children().size(), 0);
    ASSERT_EQ(n2->root(), n2);
    ASSERT_EQ(n2->parent(), nullptr);
}

TEST_F(NodeTest, Added)
{
    auto n1 = Node::create("a");
    auto n2 = Node::create("b");
    auto added1 = false;
    auto added2 = false;
    auto _ = n1->added()->connect([&](Node::Ptr node, Node::Ptr target, Node::Ptr ancestor)
    {
        added1 = node == n1 && target == n2 && ancestor == n1;
    });
    auto __ = n2->added()->connect([&](Node::Ptr node, Node::Ptr target, Node::Ptr ancestor)
    {
        added2 = node == n2 && target == n2 && ancestor == n1;
    });

    n1->addChild(n2);

    ASSERT_TRUE(added1);
    ASSERT_TRUE(added2);
}

TEST_F(NodeTest, Removed)
{
    auto n1 = Node::create("a");
    auto n2 = Node::create("b");
    auto removed1 = false;
    auto removed2 = false;
    auto _ = n1->removed()->connect([&](Node::Ptr node, Node::Ptr target, Node::Ptr ancestor)
    {
        removed1 = node == n1 && target == n2 && ancestor == n1;
    });
    auto __ = n2->removed()->connect([&](Node::Ptr node, Node::Ptr target, Node::Ptr ancestor)
    {
        removed2 = node == n2 && target == n2 && ancestor == n1;
    });

    n1->addChild(n2);
    n1->removeChild(n2);

    ASSERT_TRUE(removed1);
    ASSERT_TRUE(removed2);
}

TEST_F(NodeTest, ComponentAdded)
{
    auto node = Node::create();
    auto componentAdded = false;
    auto comp = component::SceneManager::create(MinkoTests::context());
    auto _ = node->componentAdded()->connect([&](Node::Ptr n, Node::Ptr t, component::AbstractComponent::Ptr c)
    {
        componentAdded = node == n && node == t && c == comp;
    });

    node->addComponent(comp);

    ASSERT_TRUE(componentAdded);
    ASSERT_EQ(node->component<component::SceneManager>(), comp);
    ASSERT_EQ(comp->target(), node);
}

TEST_F(NodeTest, ComponentRemoved)
{
    auto node = Node::create();
    auto componentRemoved = false;
    auto comp = component::SceneManager::create(MinkoTests::context());
    auto _ = node->componentAdded()->connect([&](Node::Ptr n, Node::Ptr t, component::AbstractComponent::Ptr c)
    {
        componentRemoved = node == n && node == t && c == comp;
    });

    node->addComponent(comp);
    node->removeComponent(comp);

    ASSERT_TRUE(componentRemoved);
    ASSERT_EQ(node->components<component::SceneManager>().size(), 0);
    ASSERT_EQ(comp->target(), nullptr);
}
