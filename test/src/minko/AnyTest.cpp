/*
Copyright (c) 2023 Aerys

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

#include "minko/AnyTest.hpp"

#include "minko/Any.hpp"

#include "sparsehash/sparse_hash_map"

using namespace minko;

TEST_F(AnyTest, SetAndGet)
{
    try
    {
        const Any v(int(42));

        ASSERT_EQ(42, Any::cast<int>(v));
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }
}

TEST_F(AnyTest, AffectationOperator)
{
    try
    {
        const Any v1(int(42));
        const Any v2 = v1;

        ASSERT_EQ(42, Any::cast<int>(v1));
        ASSERT_EQ(42, Any::cast<int>(v2));
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }

    try
    {
        Any v1(int(42));
        Any v2(int(24));

        // Also ensures affectation is not replaced by constructor during
        // optimization.
        ASSERT_EQ(42, Any::cast<int>(v1));
        ASSERT_EQ(24, Any::cast<int>(v2));

        v2 = v1;

        ASSERT_EQ(42, Any::cast<int>(v1));
        ASSERT_EQ(42, Any::cast<int>(v2));
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }

    try
    {
        struct Obj
        {
            int i;
            std::string s;
        };

        Any v1(Obj{42, "42"});
        Any v2(Obj{24, "24"});

        // Also ensures affectation is not replaced by constructor during
        // optimization.
        ASSERT_EQ(42  , Any::cast<Obj>(v1).i);
        ASSERT_EQ("42", Any::cast<Obj>(v1).s);
        ASSERT_EQ(24  , Any::cast<Obj>(v2).i);
        ASSERT_EQ("24", Any::cast<Obj>(v2).s);

        v2 = v1;

        ASSERT_EQ(42  , Any::cast<Obj>(v1).i);
        ASSERT_EQ("42", Any::cast<Obj>(v1).s);
        ASSERT_EQ(42  , Any::cast<Obj>(v2).i);
        ASSERT_EQ("42", Any::cast<Obj>(v2).s);
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }
}

