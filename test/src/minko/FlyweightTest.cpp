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

#include "minko/FlyweightTest.hpp"

#include "minko/Flyweight.hpp"

#include "sparsehash/sparse_hash_map"

using namespace minko;

TEST_F(FlyweightTest, IntegerFlyweight)
{
    Flyweight<int> a = 42;
    Flyweight<int> b = 42;

    ASSERT_TRUE(a == b);

    ASSERT_EQ(a, b);
    ASSERT_EQ(*a, 42);
    ASSERT_EQ(*b, 42);
    ASSERT_EQ(a.value(), b.value());
}

TEST_F(FlyweightTest, PointerDoesNotChangeOnGrow)
{
    Flyweight<int> a = 42;

    for (auto i = 0; i < 1000000; ++i)
        Flyweight<int> b = 23;

    ASSERT_EQ(*a, 42);
}

TEST_F(FlyweightTest, FlyweightMapKey)
{
    google::sparse_hash_map<Flyweight<std::string>, int> map;

    map["foo"] = 42;
    map["bar"] = 24;

    ASSERT_EQ(map.size(), 2);
    ASSERT_EQ(map["foo"], 42);
    ASSERT_EQ(map["bar"], 24);
}
