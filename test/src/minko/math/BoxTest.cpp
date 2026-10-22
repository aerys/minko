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

#include <cmath>

#include "gtest/gtest.h"
#include "minko/math/Box.hpp"

TEST(BoxTest, UpdateFromRotationKeepsAxisAlignedBounds)
{
    using namespace minko::math;

    auto box = Box::create(vec3(1.f, 1.f, 0.5f), vec3(-1.f, -1.f, -0.5f));
    const auto rotation = rotate(mat4(1.f), radians(45.f), vec3(0.f, 0.f, 1.f));

    box->updateFromMatrix(rotation);

    const auto expectedExtent = std::sqrt(2.f);
    EXPECT_NEAR(box->bottomLeft().x, -expectedExtent, 1e-4f);
    EXPECT_NEAR(box->bottomLeft().y, -expectedExtent, 1e-4f);
    EXPECT_NEAR(box->bottomLeft().z, -0.5f, 1e-4f);
    EXPECT_NEAR(box->topRight().x, expectedExtent, 1e-4f);
    EXPECT_NEAR(box->topRight().y, expectedExtent, 1e-4f);
    EXPECT_NEAR(box->topRight().z, 0.5f, 1e-4f);
}
