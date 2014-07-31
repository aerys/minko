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

#include "FileTest.hpp"

#include "minko/MinkoTests.hpp"

using namespace minko;
using namespace minko::file;

TEST_F(FileTest, Create)
{
	try
	{
		auto g = File::create();
	}
	catch (...)
	{
		ASSERT_TRUE(false);
	}
}

TEST_F(FileTest, CanonizeFilename)
{
	ASSERT_EQ("foo", File::canonizeFilename("foo"));
	ASSERT_EQ("foo", File::canonizeFilename("./foo"));
	ASSERT_EQ("../foo", File::canonizeFilename("../foo"));
	ASSERT_EQ("foo/bar", File::canonizeFilename("foo/bar"));
	ASSERT_EQ("foo/bar/qux", File::canonizeFilename("foo/bar/qux"));
	ASSERT_EQ("foo/bar", File::canonizeFilename("./foo/bar"));
	ASSERT_EQ("foo/bar", File::canonizeFilename("foo/./bar"));
	ASSERT_EQ("../foo", File::canonizeFilename("./../foo"));
	ASSERT_EQ("foo", File::canonizeFilename("foo/bar/.."));
	ASSERT_EQ("foo", File::canonizeFilename("./foo/../foo"));
	ASSERT_EQ("..", File::canonizeFilename("../"));
	ASSERT_EQ("/", File::canonizeFilename("/"));
	ASSERT_EQ("/", File::canonizeFilename("//"));
	ASSERT_EQ("/", File::canonizeFilename("///./"));
	ASSERT_EQ(".", File::canonizeFilename("foo/../foo/.."));
	ASSERT_EQ(".", File::canonizeFilename("foo/foo/../.."));
	ASSERT_EQ("../..", File::canonizeFilename("../.."));
	ASSERT_EQ("../../..", File::canonizeFilename("../.././.."));
	ASSERT_EQ(".", File::canonizeFilename(""));
}

TEST_F(FileTest, SanitizeFilename)
{
	ASSERT_EQ("../foo", File::sanitizeFilename("../foo"));
	ASSERT_EQ("../foo", File::sanitizeFilename("..\\foo"));
}
