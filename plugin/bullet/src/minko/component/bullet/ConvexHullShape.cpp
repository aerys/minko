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

#include "minko/component/bullet/ConvexHullShape.hpp"
#include "btBulletDynamicsCommon.h"

#include "minko/render/AbstractContext.hpp"
#include "minko/geometry/LineGeometry.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::geometry;

LineGeometry::Ptr
bullet::ConvexHullShape::getGeometry(render::AbstractContext::Ptr context) const
{
    auto lines = LineGeometry::create(context);

    for (int i = 0; i < _btShape->getNumEdges(); i++)
    {
        btVector3 pa;
        btVector3 pb;
        _btShape->getEdge(i, pa, pb);
        //std::cout << "(" <<vtx->getX()<<","<<vtx->getY()<<","<<vtx->getZ()<<")"<<std::endl;
        lines->moveTo(pa.getX(),pa.getY(),pa.getZ());
        lines->lineTo(pb.getX(),pb.getY(),pb.getZ());
    }
    
    return lines;
}