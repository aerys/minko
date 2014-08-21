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

#include "ContainerTest.hpp"

using namespace minko::data;

TEST_F(ContainerTest, AddProvider)
{
	Container c;
	auto p = Provider::create();

	p->set("foo", 42);

	c.addProvider(p);

	ASSERT_NE(std::find(c.providers().begin(), c.providers().end(), p), c.providers().end());
	ASSERT_TRUE(c.hasProperty("foo"));
	ASSERT_EQ(c.get<int>("foo"), 42);
}

TEST_F(ContainerTest, RemoveProvider)
{
	Container c;
	auto p = Provider::create();

	p->set("foo", 42);

	c.addProvider(p);
	c.removeProvider(p);

    ASSERT_EQ(std::find(c.providers().begin(), c.providers().end(), p), c.providers().end());
	ASSERT_FALSE(c.hasProperty("foo"));
}

TEST_F(ContainerTest, PropertyAdded)
{
	Container c;
	auto p = Provider::create();
	int v = 0;

	p->set("foo", 42);

	auto _ = c.propertyAdded().connect(
		[&](Container& container, const std::string& propertyName, const std::string& fullPropertyName)
		{
			if (propertyName == "foo")
				v = container.get<int>("foo");
		}
	);

	c.addProvider(p);

	ASSERT_EQ(v, 42);
}

TEST_F(ContainerTest, PropertyRemoved)
{
	Container c;
	auto p = Provider::create();
	int v = 0;

	p->set("foo", 42);

	auto _ = c.propertyAdded().connect(
        [&](Container& container, const std::string& propertyName, const std::string& fullPropertyName)
		{
			if (propertyName == "foo")
				v = 42;
		}
	);

	c.addProvider(p);
	c.removeProvider(p);

	ASSERT_EQ(v, 42);
}

TEST_F(ContainerTest, propertyChangedWhenAdded)
{
	Container c;
	auto p = Provider::create();
	int v = 0;

	p->set("foo", 42);

	auto _ = c.propertyChanged("foo").connect(
        [&](Container& container, const std::string& propertyName, const std::string& fullPropertyName)
		{
			if (propertyName == "foo")
				v = container.get<int>("foo");
		}
	);

	c.addProvider(p);

	ASSERT_EQ(v, 42);
}

TEST_F(ContainerTest, propertyChangedWhenAddedOnProvider)
{
	Container c;
	auto p = Provider::create();
	int v = 0;

	c.addProvider(p);

	auto _ = c.propertyChanged("foo").connect(
        [&](Container& container, const std::string& propertyName, const std::string& fullPropertyName)
		{
			if (propertyName == "foo")
				v = container.get<int>("foo");
		}
	);

	p->set("foo", 42);

	ASSERT_EQ(v, 42);
}

TEST_F(ContainerTest, propertyChangedWhenSetOnProvider)
{
	Container c;
	auto p = Provider::create();
	int v = 0;

	c.addProvider(p);
	p->set("foo", 23);

	auto _ = c.propertyChanged("foo").connect(
        [&](Container& container, const std::string& propertyName, const std::string& fullPropertyName)
		{
			if (propertyName == "foo")
				v = container.get<int>("foo");
		}
	);

	p->set("foo", 42);

	ASSERT_EQ(v, 42);
}

TEST_F(ContainerTest, propertyChangedNot)
{
	Container c;
	auto p = Provider::create();
	int v = 0;

	c.addProvider(p);
	p->set("foo", 42);

	auto _ = c.propertyChanged("foo").connect(
        [&](Container& container, const std::string& propertyName, const std::string& fullPropertyName)
		{
			if (propertyName == "foo")
				v = container.get<int>("foo");
		}
	);

	p->set("foo", 42);

	ASSERT_NE(v, 42);
}

TEST_F(ContainerTest, addCollection)
{
    Container c;
    auto p = Provider::create();
    auto cc = Collection::create("test");

    cc->pushBack(p);
    c.addCollection(cc);

    ASSERT_NE(std::find(c.collections().begin(), c.collections().end(), cc), c.collections().end());
}

TEST_F(ContainerTest, addProviderToCollection)
{
    Container c;
    auto p = Provider::create();
    auto cc = Collection::create("test");

    p->set("foo", 42);
    c.addCollection(cc);
    cc->pushBack(p);

    ASSERT_NE(std::find(c.providers().begin(), c.providers().end(), p), c.providers().end());
    ASSERT_TRUE(c.hasProperty("test[0].foo"));
    ASSERT_EQ(c.get<unsigned int>("test[0].foo"), 42);
}

TEST_F(ContainerTest, removeCollection)
{
    Container c;
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto collectionRemoved = false;
    auto providerRemoved = false;

    p->set("foo", 42);
    cc->pushBack(p);
    c.addCollection(cc);
    c.removeCollection(cc);

    ASSERT_EQ(std::find(c.collections().begin(), c.collections().end(), cc), c.collections().end());
    ASSERT_EQ(std::find(c.providers().begin(), c.providers().end(), p), c.providers().end());
    ASSERT_FALSE(c.hasProperty("test[0].foo"));
}

TEST_F(ContainerTest, removeProviderFromCollection)
{
    Container c;
    auto p = Provider::create();
    auto cc = Collection::create("test");

    p->set("foo", 42);
    cc->pushBack(p);
    c.addCollection(cc);
    cc->remove(p);

    ASSERT_NE(std::find(c.collections().begin(), c.collections().end(), cc), c.collections().end());
    ASSERT_EQ(std::find(c.providers().begin(), c.providers().end(), p), c.providers().end());
    ASSERT_EQ(c.get<unsigned int>("test.length"), 0u);
}

TEST_F(ContainerTest, getCollectionNth)
{
    Container c;
    auto p0 = Provider::create();
    auto p1 = Provider::create();
    auto p2 = Provider::create();
    auto cc = Collection::create("test");

    p0->set("foo", 42);
    p1->set("foo", 4242);
    p2->set("foo", 424242);
    c.addCollection(cc);
    ASSERT_EQ(c.get<unsigned int>("test.length"), 0u);
    cc->pushBack(p0);
    ASSERT_EQ(c.get<unsigned int>("test.length"), 1u);
    cc->pushBack(p1);
    ASSERT_EQ(c.get<unsigned int>("test.length"), 2u);
    cc->pushBack(p2);
    ASSERT_EQ(c.get<unsigned int>("test.length"), 3u);

    ASSERT_EQ(c.get<int>("test[0].foo"), 42);
    ASSERT_EQ(c.get<int>("test[1].foo"), 4242);
    ASSERT_EQ(c.get<int>("test[2].foo"), 424242);
}

TEST_F(ContainerTest, collectionPropertyAdded)
{
    Container c;
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto propertyAdded = false;

    cc->pushBack(p);
    c.addCollection(cc);

    auto _ = c.propertyAdded().connect(
        [&](Container& container, const std::string& propertyName, const std::string& fullPropertyName)
        {
            propertyAdded = propertyName == "foo" && fullPropertyName == "test[0].foo"
                && container.get<int>(fullPropertyName) == 42;
        }
    );

    p->set("foo", 42);

    ASSERT_TRUE(propertyAdded);
}

TEST_F(ContainerTest, collectionPropertyChanged)
{
    Container c;
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto propertyChanged = false;

    cc->pushBack(p);
    c.addCollection(cc);
    p->set("foo", 42);

    auto _ = c.propertyChanged("test[0].foo").connect(
        [&](Container& container, const std::string& propertyName, const std::string& fullPropertyName)
        {
            propertyChanged = propertyName == "foo" && fullPropertyName == "test[0].foo"
                && container.get<int>(fullPropertyName) == 4242;
        }
    );

    p->set("foo", 4242);

    ASSERT_TRUE(propertyChanged);
}

TEST_F(ContainerTest, collectionPropertyChangedNot)
{
    Container c;
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto propertyChanged = false;

    cc->pushBack(p);
    c.addCollection(cc);
    p->set("foo", 42);

    auto _ = c.propertyChanged("test[0].foo").connect(
        [&](Container& container, const std::string& propertyName, const std::string& fullPropertyName)
        {
            propertyChanged = propertyName == "foo" && fullPropertyName == "test[0].foo"
                && container.get<int>(fullPropertyName) == 42;
        }
    );

    p->set("foo", 42);

    ASSERT_FALSE(propertyChanged);
}

TEST_F(ContainerTest, collectionPropertyRemoved)
{
    Container c;
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto propertyRemoved = false;

    p->set("foo", 42);
    cc->pushBack(p);
    c.addCollection(cc);

    auto _ = c.propertyRemoved().connect(
        [&](Container& container, const std::string& propertyName, const std::string& fullPropertyName)
        {
            propertyRemoved = propertyName == "foo" && fullPropertyName == "test[0].foo";
        }
    );

    p->unset("foo");

    ASSERT_TRUE(propertyRemoved);
}

TEST_F(ContainerTest, collectionNthPropertyChanged)
{
    Container c;
    auto p0 = Provider::create();
    auto p1 = Provider::create();
    auto cc = Collection::create("test");
    auto propertyChanged = false;

    p0->set("foo", 42);
    p1->set("foo", 4242);
    cc->pushBack(p0).pushBack(p1);
    c.addCollection(cc);

    auto _ = c.propertyChanged("test[1].foo").connect(
        [&](Container& container, const std::string& propertyName, const std::string& fullPropertyName)
        {
            propertyChanged = propertyName == "foo" &&  fullPropertyName == "test[1].foo"
                && container.get<int>(fullPropertyName) == 42;
        }
    );

    p1->set("foo", 42);

    ASSERT_TRUE(propertyChanged);
}

TEST_F(ContainerTest, collectionPropertyPointerConsistency)
{
    Container c;
    auto p0 = Provider::create();
    auto p1 = Provider::create();
    auto cc = Collection::create("test");
    auto propertyChanged = false;

    p0->set("foo", 42);
    p1->set("foo", 4242);
    cc->pushBack(p0).pushBack(p1);
    c.addCollection(cc);

    ASSERT_EQ(c.getPointer<int>("test[0].foo"), c.getPointer<int>("test[0].foo"));
    ASSERT_EQ(c.getPointer<int>("test[0].foo"), p0->getPointer<int>("foo"));
    ASSERT_EQ(*p0->getPointer<int>("foo"), 42);
    ASSERT_EQ(c.getPointer<int>("test[1].foo"), c.getPointer<int>("test[1].foo"));
    ASSERT_EQ(c.getPointer<int>("test[1].foo"), p1->getPointer<int>("foo"));
    ASSERT_EQ(*p1->getPointer<int>("foo"), 4242);
}
