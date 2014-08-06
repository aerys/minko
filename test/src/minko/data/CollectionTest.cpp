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

#include "CollectionTest.hpp"

#include "minko/data/Collection.hpp"
#include "minko/data/Provider.hpp"

using namespace minko;
using namespace minko::data;

TEST_F(CollectionTest, insert)
{
    auto c = Collection::create("test");
    auto added = false;
    auto p = Provider::create("foo");
    auto _ = c->itemAdded().connect([&](Collection& collection, Provider::Ptr provider)
    {
        added = provider == p;
    });

    c->insert(c->begin(), p);

    ASSERT_TRUE(added);
    ASSERT_EQ(c->front(), p);
}

TEST_F(CollectionTest, erase)
{
    auto c = Collection::create("test");
    auto removed = false;
    auto p = Provider::create("foo");
    auto _ = c->itemRemoved().connect([&](Collection& collection, Provider::Ptr provider)
    {
        removed = provider == p;
    });

    c->insert(c->begin(), p);
    c->erase(c->begin());

    ASSERT_TRUE(removed);
    ASSERT_EQ(c->items().size(), 0);
}

TEST_F(CollectionTest, pushBack)
{
    auto c = Collection::create("test");
    auto added = false;
    auto p1 = Provider::create("foo");
    auto p2 = Provider::create("foo");

    c->pushBack(p1);

    ASSERT_EQ(c->front(), p1);

    auto _ = c->itemAdded().connect([&](Collection& collection, Provider::Ptr provider)
    {
        added = provider == p2;
    });

    c->pushBack(p2);

    ASSERT_TRUE(added);
    ASSERT_EQ(c->front(), p1);
    ASSERT_EQ(c->back(), p2);
    ASSERT_EQ(c->items().size(), 2);
}

TEST_F(CollectionTest, pushFront)
{
    auto c = Collection::create("test");
    auto added = false;
    auto p1 = Provider::create("foo");
    auto p2 = Provider::create("foo");

    c->pushFront(p1);

    ASSERT_EQ(c->front(), p1);

    auto _ = c->itemAdded().connect([&](Collection& collection, Provider::Ptr provider)
    {
        added = provider == p2;
    });

    c->pushFront(p2);

    ASSERT_TRUE(added);
    ASSERT_EQ(c->back(), p1);
    ASSERT_EQ(c->front(), p2);
    ASSERT_EQ(c->items().size(), 2);
}

TEST_F(CollectionTest, popBack)
{
    auto c = Collection::create("test");
    auto removed = false;
    auto p1 = Provider::create("foo");
    auto p2 = Provider::create("foo");

    c->pushFront(p1);
    c->pushFront(p2);

    auto _ = c->itemRemoved().connect([&](Collection& collection, Provider::Ptr provider)
    {
        removed = provider == p1;
    });

    c->popBack();

    ASSERT_EQ(c->front(), c->back());
    ASSERT_EQ(c->front(), p2);
    ASSERT_TRUE(removed);

    removed = false;
    _ = c->itemRemoved().connect([&](Collection& collection, Provider::Ptr provider)
    {
        removed = provider == p2;
    });
    c->popBack();

    ASSERT_TRUE(removed);
    ASSERT_EQ(c->items().size(), 0);
}

TEST_F(CollectionTest, popFront)
{
    auto c = Collection::create("test");
    auto removed = false;
    auto p1 = Provider::create("foo");
    auto p2 = Provider::create("foo");

    c->pushFront(p1);
    c->pushFront(p2);

    auto _ = c->itemRemoved().connect([&](Collection& collection, Provider::Ptr provider)
    {
        removed = provider == p2;
    });

    c->popFront();

    ASSERT_EQ(c->front(), c->back());
    ASSERT_EQ(c->front(), p1);
    ASSERT_TRUE(removed);

    removed = false;
    _ = c->itemRemoved().connect([&](Collection& collection, Provider::Ptr provider)
    {
        removed = provider == p1;
    });
    c->popFront();

    ASSERT_TRUE(removed);
    ASSERT_EQ(c->items().size(), 0);
}
