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

#include "minko/component/bullet/TriangleMeshShape.hpp"
#include "btBulletDynamicsCommon.h"

#include "minko/render/AbstractContext.hpp"
#include "minko/geometry/LineGeometry.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::geometry;

LineGeometry::Ptr
bullet::TriangleMeshShape::getGeometry(render::AbstractContext::Ptr context) const
{
    auto lines = LineGeometry::create(context);

    const auto numTriangles = _indexData.size() / 3;

    for (auto i = 0; i < numTriangles; ++i)
    {
        auto p0 = math::make_vec3(&_vertexData.at(_indexData.at(i * 3)));
        auto p1 = math::make_vec3(&_vertexData.at(_indexData.at(i * 3 + 1)));
        auto p2 = math::make_vec3(&_vertexData.at(_indexData.at(i * 3 + 2)));

        lines->moveTo(p0);
        lines->lineTo(p1);

        lines->moveTo(p0);
        lines->lineTo(p2);

        lines->moveTo(p1);
        lines->lineTo(p2);
    }

    return lines;
}
