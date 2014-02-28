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

#include "SignalTest.hpp"

using namespace minko;

TEST_F(SignalTest, Create)
{
	try
	{
		auto s = Signal<int>::create();
	}
	catch (...)
	{
		ASSERT_TRUE(false);
	}
}

TEST_F(SignalTest, Connect)
{
	auto s = Signal<int>::create();
	auto v = 0;
	auto _ = s->connect([&](int i) { v = i; });

	s->execute(42);

	ASSERT_EQ(s->numCallbacks(), 1);
	ASSERT_EQ(v, 42);
}

TEST_F(SignalTest, Disconnect)
{
	auto s = Signal<int>::create();
	auto v = 0;
	auto _ = s->connect([&](int i) { v = i; });

	_ = nullptr;
	s->execute(42);

	ASSERT_EQ(s->numCallbacks(), 0);
	ASSERT_EQ(v, 0);
}

TEST_F(SignalTest, ExecuteSameCallback)
{
	auto s = Signal<>::create();
	auto v = 0;
	auto c = [&]() { v++; };
	auto slot1 = s->connect(c);
	auto slot2 = s->connect(c);
	auto slot3 = s->connect(c);

	s->execute();

	ASSERT_EQ(s->numCallbacks(), 3);
	ASSERT_EQ(v, 3);
}

TEST_F(SignalTest, ExecuteMultipleCallbacks)
{
	auto s = Signal<>::create();
	auto v = 0;
	auto slot1 = s->connect([&]() { v += 1; });
	auto slot2 = s->connect([&]() { v += 2; });
	auto slot3 = s->connect([&]() { v += 3; });

	s->execute();

	ASSERT_EQ(s->numCallbacks(), 3);
	ASSERT_EQ(v, 6);
}

TEST_F(SignalTest, DisconnectFirst)
{
	auto s = Signal<>::create();
	auto v1 = 0;
	auto v2 = 0;
	auto v3 = 0;
	auto slot1 = s->connect([&]() { v1 = 42; });
	auto slot2 = s->connect([&]() { v2 = 42; });
	auto slot3 = s->connect([&]() { v3 = 42; });

	slot1 = nullptr;
	s->execute();

	ASSERT_EQ(s->numCallbacks(), 2);
	ASSERT_EQ(v1, 0);
	ASSERT_EQ(v2, 42);
	ASSERT_EQ(v3, 42);
}

TEST_F(SignalTest, DisconnectAny)
{
	auto s = Signal<>::create();
	auto v1 = 0;
	auto v2 = 0;
	auto v3 = 0;
	auto slot1 = s->connect([&]() { v1 = 42; });
	auto slot2 = s->connect([&]() { v2 = 42; });
	auto slot3 = s->connect([&]() { v3 = 42; });

	slot2 = nullptr;
	s->execute();

	ASSERT_EQ(s->numCallbacks(), 2);
	ASSERT_EQ(v1, 42);
	ASSERT_EQ(v2, 0);
	ASSERT_EQ(v3, 42);
}

TEST_F(SignalTest, DisconnectLast)
{
	auto s = Signal<>::create();
	auto v1 = 0;
	auto v2 = 0;
	auto v3 = 0;
	auto slot1 = s->connect([&]() { v1 = 42; });
	auto slot2 = s->connect([&]() { v2 = 42; });
	auto slot3 = s->connect([&]() { v3 = 42; });

	slot3 = nullptr;
	s->execute();

	ASSERT_EQ(s->numCallbacks(), 2);
	ASSERT_EQ(v1, 42);
	ASSERT_EQ(v2, 42);
	ASSERT_EQ(v3, 0);
}

TEST_F(SignalTest, LockAdd)
{
	auto s = Signal<int>::create();
	auto v = 0;

	Signal<int>::Slot slot2;
	auto slot1 = s->connect([&](int i)
	{
		slot2 = s->connect([&](int i)
		{
			v = i;
		});
	});

	s->execute(42);

	ASSERT_EQ(s->numCallbacks(), 2);
	ASSERT_EQ(v, 0);
}

TEST_F(SignalTest, LockRemove)
{
	auto s = Signal<int>::create();
	auto v = 0;
	auto w = 0;
	Signal<int>::Slot slot2;
	auto slot1 = s->connect([&](int i)
	{
		v = i;
		slot2 = nullptr;
	});
	
	slot2 = s->connect([&](int i)
	{
		w = i;
	});

	s->execute(42);

	ASSERT_EQ(s->numCallbacks(), 1);
	ASSERT_EQ(v, 42);
	ASSERT_EQ(w, 42);
}
