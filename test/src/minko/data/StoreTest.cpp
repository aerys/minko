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

#include "StoreTest.hpp"

using namespace minko::data;

TEST_F(StoreTest, AddProvider)
{
	Store c;
	auto p = Provider::create();

	p->set("foo", 42);

	c.addProvider(p);

	ASSERT_NE(std::find(c.providers().begin(), c.providers().end(), p), c.providers().end());
	ASSERT_TRUE(c.hasProperty("foo"));
	ASSERT_EQ(c.get<int>("foo"), 42);
}

TEST_F(StoreTest, RemoveProvider)
{
	Store c;
	auto p = Provider::create();

	p->set("foo", 42);

	c.addProvider(p);
	c.removeProvider(p);

    ASSERT_EQ(std::find(c.providers().begin(), c.providers().end(), p), c.providers().end());
	ASSERT_FALSE(c.hasProperty("foo"));
}

TEST_F(StoreTest, PropertyAdded)
{
	Store c;
	auto p = Provider::create();
	int v = 0;

	p->set("foo", 42);

	auto _ = c.propertyAdded().connect(
		[&](Store& container, Provider::Ptr provider, const Provider::PropertyName& propertyName)
		{
			if (*propertyName == "foo")
				v = container.get<int>("foo");
		}
	);

	c.addProvider(p);

	ASSERT_EQ(v, 42);
}

TEST_F(StoreTest, PropertyRemoved)
{
	Store c;
	auto p = Provider::create();
	int v = 0;

	p->set("foo", 42);

	auto _ = c.propertyAdded().connect(
        [&](Store& container, Provider::Ptr provider, const Provider::PropertyName& propertyName)
		{
			if (*propertyName == "foo")
				v = 42;
		}
	);

	c.addProvider(p);
	c.removeProvider(p);

	ASSERT_EQ(v, 42);
}

TEST_F(StoreTest, propertyChangedWhenAdded)
{
	Store c;
	auto p = Provider::create();
	int v = 0;

	p->set("foo", 42);

	auto _ = c.propertyChanged("foo").connect(
        [&](Store& container, Provider::Ptr provider, const Provider::PropertyName& propertyName)
		{
			if (*propertyName == "foo")
				v = container.get<int>("foo");
		}
	);

	c.addProvider(p);

	ASSERT_EQ(v, 42);
}

TEST_F(StoreTest, propertyChangedWhenAddedOnProvider)
{
	Store c;
	auto p = Provider::create();
	int v = 0;

	c.addProvider(p);

	auto _ = c.propertyChanged("foo").connect(
        [&](Store& container, Provider::Ptr provider, const Provider::PropertyName& propertyName)
		{
			if (*propertyName == "foo")
				v = container.get<int>("foo");
		}
	);

	p->set("foo", 42);

	ASSERT_EQ(v, 42);
}

TEST_F(StoreTest, propertyChangedWhenSetOnProvider)
{
	Store c;
	auto p = Provider::create();
	int v = 0;

	c.addProvider(p);
	p->set("foo", 23);

	auto _ = c.propertyChanged("foo").connect(
        [&](Store& container, Provider::Ptr provider, const Provider::PropertyName& propertyName)
		{
			if (*propertyName == "foo")
				v = container.get<int>("foo");
		}
	);

	p->set("foo", 42);

	ASSERT_EQ(v, 42);
}

TEST_F(StoreTest, propertyChangedNot)
{
	Store c;
	auto p = Provider::create();
	int v = 0;

	c.addProvider(p);
	p->set("foo", 42);

	auto _ = c.propertyChanged("foo").connect(
        [&](Store& container, Provider::Ptr provider, const Provider::PropertyName& propertyName)
		{
			if (*propertyName == "foo")
				v = container.get<int>("foo");
		}
	);

	p->set("foo", 42);

	ASSERT_NE(v, 42);
}

TEST_F(StoreTest, addCollection)
{
    Store c;
    auto p = Provider::create();
    auto cc = Collection::create("test");

    cc->pushBack(p);
    c.addCollection(cc);

    ASSERT_NE(std::find(c.collections().begin(), c.collections().end(), cc), c.collections().end());
}

TEST_F(StoreTest, addProviderToCollection)
{
    Store c;
    auto p = Provider::create();
    auto cc = Collection::create("test");

    p->set("foo", 42);
    c.addCollection(cc);
    cc->pushBack(p);

    ASSERT_NE(std::find(c.providers().begin(), c.providers().end(), p), c.providers().end());
    ASSERT_TRUE(c.hasProperty("test[0].foo"));
    ASSERT_EQ(c.get<unsigned int>("test[0].foo"), 42);
}

TEST_F(StoreTest, removeCollection)
{
    Store c;
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

TEST_F(StoreTest, removeProviderFromCollection)
{
    Store c;
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

TEST_F(StoreTest, getCollectionNth)
{
    Store c;
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

TEST_F(StoreTest, collectionPropertyAdded)
{
    Store c;
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto propertyAdded = false;

    cc->pushBack(p);
    c.addCollection(cc);

    auto _ = c.propertyAdded().connect(
        [&](Store& container, Provider::Ptr provider, const Provider::PropertyName& propertyName)
        {
            propertyAdded = *propertyName == "foo" && provider->get<int>(propertyName) == 42;
        }
    );

    p->set("foo", 42);

    ASSERT_TRUE(propertyAdded);
}

TEST_F(StoreTest, collectionPropertyChanged)
{
    Store c;
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto propertyChanged = false;

    cc->pushBack(p);
    c.addCollection(cc);
    p->set("foo", 42);

    auto _ = c.propertyChanged("test[0].foo").connect(
        [&](Store& container, Provider::Ptr provider, const Provider::PropertyName& propertyName)
        {
          propertyChanged = *propertyName == "foo" && provider->get<int>(propertyName) == 4242;
        }
    );

    p->set("foo", 4242);

    ASSERT_TRUE(propertyChanged);
}

TEST_F(StoreTest, collectionPropertyChangedNot)
{
    Store c;
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto propertyChanged = false;

    cc->pushBack(p);
    c.addCollection(cc);
    p->set("foo", 42);

    auto _ = c.propertyChanged("test[0].foo").connect(
        [&](Store& container, Provider::Ptr provider, const Provider::PropertyName& propertyName)
        {
            propertyChanged = *propertyName == "foo" && provider->get<int>(propertyName) == 42;
        }
    );

    p->set("foo", 42);

    ASSERT_FALSE(propertyChanged);
}

TEST_F(StoreTest, collectionPropertyRemoved)
{
    Store c;
    auto p = Provider::create();
    auto cc = Collection::create("test");
    auto propertyRemoved = false;

    p->set("foo", 42);
    cc->pushBack(p);
    c.addCollection(cc);

    auto _ = c.propertyRemoved().connect(
        [&](Store& container, Provider::Ptr provider, const Provider::PropertyName& propertyName)
        {
            propertyRemoved = *propertyName == "foo";
        }
    );

    p->unset("foo");

    ASSERT_TRUE(propertyRemoved);
}

TEST_F(StoreTest, collectionNthPropertyChanged)
{
    Store c;
    auto p0 = Provider::create();
    auto p1 = Provider::create();
    auto cc = Collection::create("test");
    auto propertyChanged = false;

    p0->set("foo", 42);
    p1->set("foo", 4242);
    cc->pushBack(p0).pushBack(p1);
    c.addCollection(cc);

    auto _ = c.propertyChanged("test[1].foo").connect(
        [&](Store& container, Provider::Ptr provider, const Provider::PropertyName& propertyName)
        {
            propertyChanged = *propertyName == "foo" && provider->get<int>(propertyName) == 42;
        }
    );

    p1->set("foo", 42);

    ASSERT_TRUE(propertyChanged);
}

TEST_F(StoreTest, collectionPropertyPointerConsistency)
{
    Store c;
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

TEST_F(StoreTest, providerAddedTwiceRemovedOnce)
{
    Store c;
    auto p = Provider::create();
	int propertyAdded = 0;
	bool propertyRemoved = false;

	p->set("foo", 42);

	auto addedSlod = c.propertyAdded("foo").connect([&](Store&, Provider::Ptr, const Provider::PropertyName&)
	{
		propertyAdded++;
	});
	auto removedSlot = c.propertyRemoved("foo").connect([&](Store&, Provider::Ptr, const Provider::PropertyName&)
	{
		ASSERT_FALSE(propertyRemoved);
		propertyRemoved = true;
	});

	c.addProvider(p);
	c.addProvider(p);

	c.removeProvider(p);

	ASSERT_EQ(propertyAdded, 2);
	ASSERT_TRUE(c.hasProperty("foo"));
}

TEST_F(StoreTest, providerAddedTwiceInCollectionRemovedOnce)
{
    Store c;
    auto p = Provider::create();
	int propertyAdded = 0;
	bool propertyRemoved = false;

	p->set("foo", 42);

	auto addedSlod = c.propertyAdded("bar[0].foo").connect([&](Store&, Provider::Ptr, const Provider::PropertyName&)
	{
		propertyAdded++;
	});
	auto removedSlot = c.propertyRemoved("bar[0].foo").connect([&](Store&, Provider::Ptr, const Provider::PropertyName&)
	{
		ASSERT_FALSE(propertyRemoved);
		propertyRemoved = true;
	});

	c.addProvider(p, "bar");
	c.addProvider(p, "bar");

	c.removeProvider(p ,"bar");

	ASSERT_EQ(propertyAdded, 2);
	ASSERT_TRUE(c.hasProperty("bar[0].foo"));
}

TEST_F(StoreTest, specificPropertyAddedSignal)
{
    Store s;

    ASSERT_FALSE(s.hasPropertyAddedSignal("test"));

    auto& s1 = s.propertyAdded("test");

    ASSERT_TRUE(s.hasPropertyAddedSignal("test"));
    ASSERT_EQ(s1.numCallbacks(), 0);

    Store::PropertyName callbackPropertyName;
    Store* callbackStore;
    Provider::Ptr callbackProvider;
    bool executed = false;
    auto _ = s1.connect([&](Store& store, Provider::Ptr provider, Store::PropertyName propertyName)
    {
        executed = true;
        callbackStore = &store;
        callbackProvider = provider;
        callbackPropertyName = propertyName;
    });

    ASSERT_EQ(s1.numCallbacks(), 1);

    auto p = Provider::create();

    p->set("test", 42);
    ASSERT_FALSE(executed);
    s.addProvider(p);

    ASSERT_TRUE(executed);
    ASSERT_EQ(callbackStore, &s);
    ASSERT_EQ(callbackProvider, p);
    ASSERT_EQ(callbackPropertyName, "test");
}

TEST_F(StoreTest, specificPropertyRemovedSignal)
{
    Store s;

    ASSERT_FALSE(s.hasPropertyAddedSignal("test"));

    auto& s1 = s.propertyRemoved("test");

    ASSERT_TRUE(s.hasPropertyRemovedSignal("test"));
    ASSERT_EQ(s1.numCallbacks(), 0);

    Store::PropertyName callbackPropertyName;
    Store* callbackStore;
    Provider::Ptr callbackProvider;
    bool executed = false;
    auto _ = s1.connect([&](Store& store, Provider::Ptr provider, Store::PropertyName propertyName)
    {
        executed = true;
        callbackStore = &store;
        callbackProvider = provider;
        callbackPropertyName = propertyName;
    });

    ASSERT_EQ(s1.numCallbacks(), 1);

    auto p = Provider::create();

    p->set("test", 42);
    s.addProvider(p);
    ASSERT_FALSE(executed);
    s.removeProvider(p);

    ASSERT_TRUE(executed);
    ASSERT_EQ(callbackStore, &s);
    ASSERT_EQ(callbackProvider, p);
    ASSERT_EQ(callbackPropertyName, "test");
}

TEST_F(StoreTest, specificPropertyChangedSignal)
{
    Store s;
    auto p = Provider::create();

    p->set("test", 42);
    s.addProvider(p);

    ASSERT_FALSE(s.hasPropertyChangedSignal("test"));

    auto& s1 = s.propertyChanged("test");

    ASSERT_TRUE(s.hasPropertyChangedSignal("test"));
    ASSERT_EQ(s1.numCallbacks(), 0);

    Store::PropertyName callbackPropertyName;
    Store* callbackStore;
    Provider::Ptr callbackProvider;
    bool executed = false;
    auto _ = s1.connect([&](Store& store, Provider::Ptr provider, Store::PropertyName propertyName)
    {
        executed = true;
        callbackStore = &store;
        callbackProvider = provider;
        callbackPropertyName = propertyName;
    });

    ASSERT_EQ(s1.numCallbacks(), 1);

    ASSERT_FALSE(executed);
    p->set("test", 24);

    ASSERT_TRUE(executed);
    ASSERT_EQ(callbackStore, &s);
    ASSERT_EQ(callbackProvider, p);
    ASSERT_EQ(callbackPropertyName, "test");
}

TEST_F(StoreTest, doNotFreeUsedPropertyAddedSignals)
{
    Store s;
    auto p = Provider::create();

    p->set("test", 42);
    s.addProvider(p);

    ASSERT_FALSE(s.hasPropertyAddedSignal("test"));

    auto& s1 = s.propertyAdded("test");

    ASSERT_EQ(s1.numCallbacks(), 0);
    ASSERT_TRUE(s.hasPropertyAddedSignal("test"));

    auto _ = s1.connect([&](Store& store, Provider::Ptr provider, Store::PropertyName propertyName)
    {
        // nothing
    });

    ASSERT_EQ(s1.numCallbacks(), 1);

    s.removeProvider(p);

    ASSERT_TRUE(s.hasPropertyAddedSignal("test"));
}

TEST_F(StoreTest, doNotFreeUsedPropertyChangedSignals)
{
    Store s;
    auto p = Provider::create();

    p->set("test", 42);
    s.addProvider(p);

    ASSERT_FALSE(s.hasPropertyChangedSignal("test"));

    auto& s1 = s.propertyChanged("test");

    ASSERT_EQ(s1.numCallbacks(), 0);
    ASSERT_TRUE(s.hasPropertyChangedSignal("test"));

    auto _ = s1.connect([&](Store& store, Provider::Ptr provider, Store::PropertyName propertyName)
    {
        // nothing
    });

    ASSERT_EQ(s1.numCallbacks(), 1);

    s.removeProvider(p);

    ASSERT_TRUE(s.hasPropertyChangedSignal("test"));
}

TEST_F(StoreTest, doNotFreeUsedPropertyRemovedSignals)
{
    Store s;
    auto p = Provider::create();

    p->set("test", 42);
    s.addProvider(p);

    ASSERT_FALSE(s.hasPropertyRemovedSignal("test"));

    auto& s1 = s.propertyRemoved("test");

    ASSERT_EQ(s1.numCallbacks(), 0);
    ASSERT_TRUE(s.hasPropertyRemovedSignal("test"));

    auto _ = s1.connect([&](Store& store, Provider::Ptr provider, Store::PropertyName propertyName)
    {
        // nothing
    });

    ASSERT_EQ(s1.numCallbacks(), 1);

    s.removeProvider(p);

    ASSERT_TRUE(s.hasPropertyRemovedSignal("test"));
}

TEST_F(StoreTest, freeUnusedPropertyAddedSignals)
{
    Store s;
    auto p = Provider::create();

    p->set("test", 42);
    s.addProvider(p);

    ASSERT_FALSE(s.hasPropertyAddedSignal("test"));

    auto& s1 = s.propertyAdded("test");

    ASSERT_EQ(s1.numCallbacks(), 0);
    ASSERT_TRUE(s.hasPropertyAddedSignal("test"));

    s.removeProvider(p);

    ASSERT_FALSE(s.hasPropertyAddedSignal("test"));
}

TEST_F(StoreTest, freeUnusedPropertyChangedSignals)
{
    Store s;
    auto p = Provider::create();

    p->set("test", 42);
    s.addProvider(p);

    ASSERT_FALSE(s.hasPropertyChangedSignal("test"));

    auto& s1 = s.propertyChanged("test");

    ASSERT_EQ(s1.numCallbacks(), 0);
    ASSERT_TRUE(s.hasPropertyChangedSignal("test"));

    s.removeProvider(p);

    ASSERT_FALSE(s.hasPropertyChangedSignal("test"));
}

TEST_F(StoreTest, freeUnusedPropertyRemovedSignals)
{
    Store s;
    auto p = Provider::create();

    p->set("test", 42);
    s.addProvider(p);

    ASSERT_FALSE(s.hasPropertyRemovedSignal("test"));

    auto& s1 = s.propertyRemoved("test");

    ASSERT_EQ(s1.numCallbacks(), 0);
    ASSERT_TRUE(s.hasPropertyRemovedSignal("test"));

    s.removeProvider(p);

    ASSERT_FALSE(s.hasPropertyRemovedSignal("test"));
}
