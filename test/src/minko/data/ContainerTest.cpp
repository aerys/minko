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

TEST_F(ContainerTest, Create)
{
	try
	{
		auto c = Container::create();
	}
	catch (...)
	{
		ASSERT_TRUE(false);
	}
}

TEST_F(ContainerTest, AddProvider)
{
	auto c = Container::create();
	auto p = Provider::create();

	p->set("foo", 42);

	c->addProvider(p);

	ASSERT_NE(std::find(c->providers().begin(), c->providers().end(), p), c->providers().end());
	ASSERT_TRUE(c->hasProperty("foo"));
	ASSERT_EQ(c->get<int>("foo"), 42);
}

TEST_F(ContainerTest, RemoveProvider)
{
	auto c = Container::create();
	auto p = Provider::create();

	p->set("foo", 42);

	c->addProvider(p);
	c->removeProvider(p);

    ASSERT_EQ(std::find(c->providers().begin(), c->providers().end(), p), c->providers().end());
	ASSERT_FALSE(c->hasProperty("foo"));
}

TEST_F(ContainerTest, PropertyAdded)
{
	auto c = Container::create();
	auto p = Provider::create();
	int v = 0;

	p->set("foo", 42);

	auto _ = c->propertyAdded()->connect(
		[&](Container::Ptr container, const std::string& propertyName, const std::string& fullPropertyName)
		{
			if (container == c && propertyName == "foo")
				v = container->get<int>("foo");
		}
	);

	c->addProvider(p);

	ASSERT_EQ(v, 42);
}

TEST_F(ContainerTest, PropertyRemoved)
{
	auto c = Container::create();
	auto p = Provider::create();
	int v = 0;

	p->set("foo", 42);

	auto _ = c->propertyAdded()->connect(
        [&](Container::Ptr container, const std::string& propertyName, const std::string& fullPropertyName)
		{
			if (container == c && propertyName == "foo")
				v = 42;
		}
	);

	c->addProvider(p);
	c->removeProvider(p);

	ASSERT_EQ(v, 42);
}

TEST_F(ContainerTest, propertyChangedWhenAdded)
{
	auto c = Container::create();
	auto p = Provider::create();
	int v = 0;

	p->set("foo", 42);

	auto _ = c->propertyChanged("foo")->connect(
        [&](Container::Ptr container, const std::string& propertyName, const std::string& fullPropertyName)
		{
			if (container == c && propertyName == "foo")
				v = container->get<int>("foo");
		}
	);

	c->addProvider(p);

	ASSERT_EQ(v, 42);
}

TEST_F(ContainerTest, propertyChangedWhenAddedOnProvider)
{
	auto c = Container::create();
	auto p = Provider::create();
	int v = 0;

	c->addProvider(p);

	auto _ = c->propertyChanged("foo")->connect(
        [&](Container::Ptr container, const std::string& propertyName, const std::string& fullPropertyName)
		{
			if (container == c && propertyName == "foo")
				v = container->get<int>("foo");
		}
	);

	p->set("foo", 42);

	ASSERT_EQ(v, 42);
}

TEST_F(ContainerTest, propertyChangedWhenSetOnProvider)
{
	auto c = Container::create();
	auto p = Provider::create();
	int v = 0;

	c->addProvider(p);
	p->set("foo", 23);

	auto _ = c->propertyChanged("foo")->connect(
        [&](Container::Ptr container, const std::string& propertyName, const std::string& fullPropertyName)
		{
			if (container == c && propertyName == "foo")
				v = container->get<int>("foo");
		}
	);

	p->set("foo", 42);

	ASSERT_EQ(v, 42);
}

TEST_F(ContainerTest, propertyChangedNot)
{
	auto c = Container::create();
	auto p = Provider::create();
	int v = 0;

	c->addProvider(p);
	p->set("foo", 42);

	auto _ = c->propertyChanged("foo")->connect(
        [&](Container::Ptr container, const std::string& propertyName, const std::string& fullPropertyName)
		{
			if (container == c && propertyName == "foo")
				v = container->get<int>("foo");
		}
	);

	p->set("foo", 42);

	ASSERT_NE(v, 42);
}

TEST_F(ContainerTest, addCollection)
{
    auto c = Container::create();
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto collectionAdded = false;
    auto providerAdded = false;

    cc->pushBack(p);

    auto _ = c->collectionAdded()->connect([&](Container::Ptr container, Collection::Ptr collection)
    {
        collectionAdded = collection == cc;
    });

    auto __ = c->providerAdded()->connect([&](Container::Ptr container, Provider::Ptr provider)
    {
        providerAdded = provider == p;
    });

    c->addCollection(cc);

    ASSERT_NE(std::find(c->collections().begin(), c->collections().end(), cc), c->collections().end());
    ASSERT_TRUE(collectionAdded);
    ASSERT_TRUE(providerAdded);
}

TEST_F(ContainerTest, addProviderToCollection)
{
    auto c = Container::create();
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto providerAdded = false;

    c->addCollection(cc);

    auto _ = c->providerAdded()->connect([&](Container::Ptr container, Provider::Ptr provider)
    {
        providerAdded = provider == p;
    });

    cc->pushBack(p);

    ASSERT_TRUE(providerAdded);
}

TEST_F(ContainerTest, removeCollection)
{
    auto c = Container::create();
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto collectionRemoved = false;
    auto providerRemoved = false;

    cc->pushBack(p);
    c->addCollection(cc);

    auto _ = c->collectionRemoved()->connect([&](Container::Ptr container, Collection::Ptr collection)
    {
        collectionRemoved = collection == cc;
    });

    auto __ = c->providerRemoved()->connect([&](Container::Ptr container, Provider::Ptr provider)
    {
        providerRemoved = provider == p;
    });

    c->removeCollection(cc);

    ASSERT_EQ(std::find(c->collections().begin(), c->collections().end(), cc), c->collections().end());
    ASSERT_TRUE(collectionRemoved);
    ASSERT_TRUE(providerRemoved);
}

TEST_F(ContainerTest, removeProviderFromCollection)
{
    auto c = Container::create();
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto providerRemoved = false;

    cc->pushBack(p);
    c->addCollection(cc);

    auto _ = c->providerRemoved()->connect([&](Container::Ptr container, Provider::Ptr provider)
    {
        providerRemoved = provider == p;
    });

    cc->remove(p);

    ASSERT_TRUE(providerRemoved);
    ASSERT_EQ(c->get<unsigned int>("test.length"), 0u);
}

TEST_F(ContainerTest, getCollectionNth)
{
    auto c = Container::create();
    auto p0 = Provider::create();
    auto p1 = Provider::create();
    auto p2 = Provider::create();
    auto cc = Collection::create("test");

    p0->set("foo", 42);
    p1->set("foo", 4242);
    p2->set("foo", 424242);
    c->addCollection(cc);
    ASSERT_EQ(c->get<unsigned int>("test.length"), 0u);
    cc->pushBack(p0);
    ASSERT_EQ(c->get<unsigned int>("test.length"), 1u);
    cc->pushBack(p1);
    ASSERT_EQ(c->get<unsigned int>("test.length"), 2u);
    cc->pushBack(p2);
    ASSERT_EQ(c->get<unsigned int>("test.length"), 3u);

    ASSERT_EQ(c->get<int>("test[0].foo"), 42);
    ASSERT_EQ(c->get<int>("test[1].foo"), 4242);
    ASSERT_EQ(c->get<int>("test[2].foo"), 424242);
}

TEST_F(ContainerTest, collectionPropertyAdded)
{
    auto c = Container::create();
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto propertyAdded = false;

    cc->pushBack(p);
    c->addCollection(cc);

    auto _ = c->propertyAdded()->connect(
        [&](Container::Ptr container, const std::string& propertyName, const std::string& fullPropertyName)
        {
            propertyAdded = container == c && propertyName == "foo" && fullPropertyName == "test[0].foo"
                && container->get<int>(fullPropertyName) == 42;
        }
    );

    p->set("foo", 42);

    ASSERT_TRUE(propertyAdded);
}

TEST_F(ContainerTest, collectionPropertyChanged)
{
    auto c = Container::create();
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto propertyChanged = false;

    cc->pushBack(p);
    c->addCollection(cc);
    p->set("foo", 42);

    auto _ = c->propertyChanged("test[0].foo")->connect(
        [&](Container::Ptr container, const std::string& propertyName, const std::string& fullPropertyName)
        {
            propertyChanged = container == c && propertyName == "foo" && fullPropertyName == "test[0].foo"
                && container->get<int>(fullPropertyName) == 4242;
        }
    );

    p->set("foo", 4242);

    ASSERT_TRUE(propertyChanged);
}

TEST_F(ContainerTest, collectionPropertyChangedNot)
{
    auto c = Container::create();
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto propertyChanged = false;

    cc->pushBack(p);
    c->addCollection(cc);
    p->set("foo", 42);

    auto _ = c->propertyChanged("test[0].foo")->connect(
        [&](Container::Ptr container, const std::string& propertyName, const std::string& fullPropertyName)
        {
            propertyChanged = c == container && propertyName == "foo" && fullPropertyName == "test[0].foo"
                && container->get<int>(fullPropertyName) == 42;
        }
    );

    p->set("foo", 42);

    ASSERT_FALSE(propertyChanged);
}

TEST_F(ContainerTest, collectionPropertyRemoved)
{
    auto c = Container::create();
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto propertyRemoved = false;

    p->set("foo", 42);
    cc->pushBack(p);
    c->addCollection(cc);

    auto _ = c->propertyRemoved()->connect(
        [&](Container::Ptr container, const std::string& propertyName, const std::string& fullPropertyName)
        {
            propertyRemoved = container == c && propertyName == "foo" && fullPropertyName == "test[0].foo";
        }
    );

    p->unset("foo");

    ASSERT_TRUE(propertyRemoved);
}

TEST_F(ContainerTest, collectionNthPropertyChanged)
{
    auto c = Container::create();
    auto p0 = Provider::create();
    auto p1 = Provider::create();
    auto cc = Collection::create("test");
    auto propertyChanged = false;

    p0->set("foo", 42);
    p1->set("foo", 4242);
    cc->pushBack(p0).pushBack(p1);
    c->addCollection(cc);

    auto _ = c->propertyChanged("test[1].foo")->connect(
        [&](Container::Ptr container, const std::string& propertyName, const std::string& fullPropertyName)
        {
            propertyChanged = container == c && propertyName == "foo" &&  fullPropertyName == "test[1].foo"
                && container->get<int>(fullPropertyName) == 42;
        }
    );

    p1->set("foo", 42);

    ASSERT_TRUE(propertyChanged);
}

TEST_F(ContainerTest, collectionPropertyPointerConsistency)
{
    auto c = Container::create();
    auto p0 = Provider::create();
    auto p1 = Provider::create();
    auto cc = Collection::create("test");
    auto propertyChanged = false;

    p0->set("foo", 42);
    p1->set("foo", 4242);
    cc->pushBack(p0).pushBack(p1);
    c->addCollection(cc);

    ASSERT_EQ(c->getPointer<int>("test[0].foo"), c->getPointer<int>("test[0].foo"));
    ASSERT_EQ(c->getPointer<int>("test[0].foo"), p0->getPointer<int>("foo"));
    ASSERT_EQ(*p0->getPointer<int>("foo"), 42);
    ASSERT_EQ(c->getPointer<int>("test[1].foo"), c->getPointer<int>("test[1].foo"));
    ASSERT_EQ(c->getPointer<int>("test[1].foo"), p1->getPointer<int>("foo"));
    ASSERT_EQ(*p1->getPointer<int>("foo"), 4242);
}
