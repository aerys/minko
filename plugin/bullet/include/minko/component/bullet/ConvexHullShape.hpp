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

#pragma once

#include "minko/Common.hpp"
#include "minko/BulletCommon.hpp"
#include "minko/component/bullet/AbstractPhysicsShape.hpp"

class btConvexHullShape;

namespace minko
{
    namespace component
    {
        namespace bullet
        {
            class ConvexHullShape:
                public AbstractPhysicsShape
            {
            public:
                typedef std::shared_ptr<ConvexHullShape>                    Ptr;

            private:
                typedef std::shared_ptr<geometry::LineGeometry>         LineGeometryPtr;
                typedef std::shared_ptr<geometry::Geometry>             GeometryPtr;
                typedef std::shared_ptr<render::AbstractContext>        AbsContextPtr;
                typedef std::shared_ptr<minko::scene::Node>             NodePtr;
                typedef std::shared_ptr<btConvexHullShape>           btConvexHullShapePtr;

            private:
                GeometryPtr                                             _geometry;
                btConvexHullShapePtr                                    _btShape;


            public:
                inline static
                Ptr
                create(GeometryPtr geom)
                {
                    ConvexHullShape::Ptr shape = std::shared_ptr<ConvexHullShape>(new ConvexHullShape(geom));
                    return shape;
                }

                inline static
                Ptr
                create(btConvexHullShapePtr btshape)
                {
                    ConvexHullShape::Ptr shape = std::shared_ptr<ConvexHullShape>(new ConvexHullShape(btshape));
                    return shape;
                }

                inline
                void
                btShape(btConvexHullShapePtr btShape)
                {
                    _btShape = btShape;
                }

                inline
                btConvexHullShapePtr
                getBtShape()
                {
                    return _btShape;
                }

                inline
                GeometryPtr
                geometry() const
                {
                    return _geometry;
                }

                void
                initializeGeometry();
                 
                inline
                float
                volume() const
                {
                    const float volume = 8.0f
                        /** _localScaling->x() * (_halfExtentX + _margin)
                        * _localScaling->y() * (_halfExtentY + _margin)
                        * _localScaling->z() * (_halfExtentZ + _margin)*/;

                    return volume;
                }

                LineGeometryPtr
                getGeometry(AbsContextPtr) const;

            private:
                ConvexHullShape(GeometryPtr geom):
                    AbstractPhysicsShape(CONVEXHULL),
                    _geometry(geom)                   
                {
                }

                ConvexHullShape(btConvexHullShapePtr btshape):
                    AbstractPhysicsShape(CONVEXHULL),
                    _btShape(btshape)                   
                {
                }
            };
        }
    }
}
