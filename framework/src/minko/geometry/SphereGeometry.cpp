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

#include "minko/geometry/SphereGeometry.hpp"

#include "minko/render/VertexBuffer.hpp"
#include "minko/render/IndexBuffer.hpp"

using namespace minko;
using namespace minko::geometry;
using namespace minko::render;
using namespace minko::render;

void
SphereGeometry::initializeVertices(std::shared_ptr<AbstractContext>    context,
                                   unsigned int                        numParallels,
                                   unsigned int                        numMeridians,
                                   bool                                withNormals)
{
    unsigned int numVertices = (numParallels - 2) * (numMeridians + 1) + 2;
    unsigned int c = 0;
    unsigned int k = 0;
    std::vector<float> data;

    for (unsigned int j = 1; j < numParallels - 1; j++)
    {
        for (unsigned int i = 0; i < numMeridians + 1; i++, c += 3, k += 2)
        {
            float theta    = j / ((float)numParallels - 1.f) * float(M_PI);
            float phi     = i / (float)numMeridians * 2.f * float(M_PI);
            float x        = sinf(theta) * cosf(phi) * .5f;
            float y        = cosf(theta) * .5f;
            float z        = -sinf(theta) * sinf(phi) * .5f;

            // x, y, z
            data.push_back(x);
            data.push_back(y);
            data.push_back(z);

            // u, v
            data.push_back(1.f - i / float(numMeridians));
            data.push_back(j / (float(numParallels) - 1.f));

            // normal
            if (withNormals)
            {
                data.push_back(x * 2.f);
                data.push_back(y * 2.f);
                data.push_back(z * 2.f);
            }
        }
    }

    // north pole
    data.push_back(0.f);
    data.push_back(.5f);
    data.push_back(0.f);

    data.push_back(.5f);
    data.push_back(0.f);

    if (withNormals)
    {
        data.push_back(0.f);
        data.push_back(1.f);
        data.push_back(0.f);
    }

    // south pole
    data.push_back(0.f);
    data.push_back(-.5f);
    data.push_back(0.f);

    data.push_back(.5f);
    data.push_back(1.f);

    if (withNormals)
    {
        data.push_back(0.f);
        data.push_back(-1.f);
        data.push_back(0.f);
    }

    auto stream = VertexBuffer::create(context, data);

    stream->addAttribute("position", 3, 0);
    stream->addAttribute("uv", 2, 3);
    if (withNormals)
        stream->addAttribute("normal", 3, 5);

    addVertexBuffer(stream);

    computeCenterPosition();
}

void
SphereGeometry::initializeIndices(std::shared_ptr<AbstractContext>    context,
                                  unsigned int                        numParallels,
                                  unsigned int                        numMeridians)
{
    //std::vector<unsigned short>    data(numParallels * numMeridians * 6);
    std::vector<unsigned short>    data((numParallels - 2) * numMeridians * 6);
    unsigned int c = 0;

    numMeridians++;
    for (unsigned int j = 0; j < numParallels - 3; j++)
    {
        for (unsigned int i = 0; i < numMeridians - 1; i++)
        {
            data[c++] = j * numMeridians + i;
            data[c++] = (j + 1) * numMeridians + i + 1;
            data[c++] = j * numMeridians + i + 1;

            data[c++] = j * numMeridians + i;
            data[c++] = (j + 1) * numMeridians + i;
            data[c++] = (j + 1) * numMeridians + i + 1;
        }
    }

    for (unsigned int i = 0; i < numMeridians - 1; i++)
    {
        data[c++] = (numParallels - 2) * numMeridians;
        data[c++] = i;
        data[c++] = i + 1;


        data[c++] = (numParallels - 2) * numMeridians + 1;
        data[c++] = (numParallels - 3) * numMeridians + i + 1;
        data[c++] = (numParallels - 3) * numMeridians + i;
    }

    indices(IndexBuffer::create(context, data));
}
