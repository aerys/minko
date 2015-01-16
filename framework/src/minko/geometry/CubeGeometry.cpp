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

#include "minko/geometry/CubeGeometry.hpp"

using namespace minko;
using namespace minko::geometry;
using namespace minko::render;
using namespace minko::render;

void
CubeGeometry::initialize(std::shared_ptr<AbstractContext> context)
{
    float xyzData[] = {
        // top
        0.5, 0.5, -0.5,     0.f, 1.f, 0.f,      1.f, 0.f,
        -0.5, 0.5, 0.5,     0.f, 1.f, 0.f,      0.f, 1.f,
        0.5, 0.5, 0.5,      0.f, 1.f, 0.f,      1.f, 1.f,
        0.5, 0.5, -0.5,     0.f, 1.f, 0.f,      1.f, 0.f,
        -0.5, 0.5, -0.5,    0.f, 1.f, 0.f,      0.f, 0.f,
        -0.5, 0.5, 0.5,     0.f, 1.f, 0.f,      0.f, 1.f,
        // bottom
        -0.5, -0.5, 0.5,    0.f, -1.f, 0.f,     0.f, 0.f,
        0.5, -0.5, -0.5,    0.f, -1.f, 0.f,     1., 1.,
        0.5, -0.5, 0.5,     0.f, -1.f, 0.f,     1.f, 0.,
        -0.5, -0.5, -0.5,   0.f, -1.f, 0.f,     0.f, 1.f,
        0.5, -0.5, -0.5,    0.f, -1.f, 0.f,     1.f, 1.f,
        -0.5, -0.5, 0.5,    0.f, -1.f, 0.f,     0.f, 0.f,
        // front
        0.5, -0.5, -0.5,    0.f, 0.f, -1.f,     0.f, 1.f,
        -0.5, 0.5, -0.5,    0.f, 0.f, -1.f,     1.f, 0.f,
        0.5, 0.5, -0.5,     0.f, 0.f, -1.f,     0.f, 0.f,
        -0.5, 0.5, -0.5,    0.f, 0.f, -1.f,     1.f, 0.f,
        0.5, -0.5, -0.5,    0.f, 0.f, -1.f,     0.f, 1.f,
        -0.5, -0.5, -0.5,   0.f, 0.f, -1.f,     1.f, 1.f,
        // back
        -0.5, 0.5, 0.5,     0.f, 0.f, 1.f,      0.f, 0.f,
        -0.5, -0.5, 0.5,    0.f, 0.f, 1.f,      0.f, 1.f,
        0.5, 0.5, 0.5,      0.f, 0.f, 1.f,      1.f, 0.f,
        -0.5, -0.5, 0.5,    0.f, 0.f, 1.f,      0.f, 1.f,
        0.5, -0.5, 0.5,     0.f, 0.f, 1.f,      1.f, 1.f,
        0.5, 0.5, 0.5,      0.f, 0.f, 1.f,      1.f, 0.f,
        // left
        -0.5, -0.5, 0.5,    -1.f, 0.f, 0.f,     1.f, 1.f,
        -0.5, 0.5, -0.5,    -1.f, 0.f, 0.f,     0.f, 0.f,
        -0.5, -0.5, -0.5,   -1.f, 0.f, 0.f,     0.f, 1.f,
        -0.5, 0.5, -0.5,    -1.f, 0.f, 0.f,        0.f, 0.f,
        -0.5, -0.5, 0.5,    -1.f, 0.f, 0.f,     1.f, 1.f,
        -0.5, 0.5, 0.5,     -1.f, 0.f, 0.f,     1.f, 0.f,
        // right
        0.5, -0.5, -0.5,    1.f, 0.f, 0.f,      1.f, 1.f,
        0.5, 0.5, -0.5,     1.f, 0.f, 0.f,      1.f, 0.f,
        0.5, 0.5, 0.5,      1.f, 0.f, 0.f,      0.f, 0.f,
        0.5, 0.5, 0.5,        1.f, 0.f, 0.f,      0.f, 0.f,
        0.5, -0.5, 0.5,     1.f, 0.f, 0.f,      0.f, 1.f,
        0.5, -0.5, -0.5,    1.f, 0.f, 0.f,      1.f, 1.f
    };

    unsigned short i[] = {
        0, 1, 2, 3, 4, 5,
        6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35
    };

    auto vertexBuffer = VertexBuffer::create(context, std::begin(xyzData), std::end(xyzData));

    vertexBuffer->addAttribute("position", 3, 0);
    vertexBuffer->addAttribute("normal", 3, 3);
    vertexBuffer->addAttribute("uv", 2, 6);
    addVertexBuffer(vertexBuffer);

    indices(IndexBuffer::create(context, std::begin(i), std::end(i)));

    computeCenterPosition();
}