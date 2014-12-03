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

#include "StarGeometry.hpp"

#include <minko/render/AbstractContext.hpp>
#include <minko/render/VertexBuffer.hpp>
#include <minko/render/IndexBuffer.hpp>

using namespace minko;
using namespace minko::geometry;
using namespace minko::render;

void
StarGeometry::initialize(render::AbstractContext::Ptr context,
                         unsigned int                 numBranches,
                         float                        outerRadius,
                         float                        innerRadius)
{
    if (context == nullptr)
        throw std::invalid_argument("context");

    if (numBranches < 2)
        throw std::invalid_argument("numBranches");

    const float outRadius       = fabsf(outerRadius);
    const float inRadius        = std::min(outRadius, fabsf(innerRadius));

    // vertex buffer initialization
    static const unsigned int   vertexSize    = 3; // (x y z nx ny nz)
    const unsigned int          numVertices   = 1 + 2*numBranches;
    std::vector<float>          vertexData(numVertices * vertexSize, 0.0f);

    const float        step     = float(M_PI) / float(numBranches);
    const float        cStep    = std::cos(step);
    const float        sStep    = std::sin(step);

    unsigned int       idx      = vertexSize;
    float              cAng     = 1.0f;
    float              sAng     = 0.0f;

    for (unsigned int i = 0; i < numBranches; ++i)
    {
        vertexData[idx]        = outRadius * cAng;
        vertexData[idx + 1]    = outRadius * sAng;
        idx                    += vertexSize;

        float c = cAng * cStep - sAng * sStep;
        float s = sAng * cStep + cAng * sStep;
        cAng    = c;
        sAng    = s;

        vertexData[idx]        = inRadius * cAng;
        vertexData[idx + 1]    = inRadius * sAng;
        idx                    += vertexSize;

        c       = cAng * cStep - sAng * sStep;
        s       = sAng * cStep + cAng * sStep;
        cAng    = c;
        sAng    = s;
    }

    auto vertexBuffer    = VertexBuffer::create(context, vertexData);

    vertexBuffer->addAttribute("position", 3, 0);
    addVertexBuffer(vertexBuffer);

    // index buffer initialization
    const unsigned int numTriangles = 2 * numBranches;

    std::vector<unsigned short> indexData(3 * numTriangles);

    idx = 0;

    for (unsigned int i = 0; i < numTriangles; ++i)
    {
        indexData[idx++] = 0;
        indexData[idx++] = i + 1;
        indexData[idx++] = i + 2 < numVertices ? i + 2 : 1;
    }

    indices(IndexBuffer::create(context, indexData));
}
