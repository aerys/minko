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

#include "minko/geometry/Geometry.hpp"
#include "minko/geometry/CubeGeometry.hpp"
#include "minko/geometry/QuadGeometry.hpp"
#include "minko/geometry/SphereGeometry.hpp"
#include "minko/geometry/LineGeometry.hpp"

#include "minko/LuaWrapper.hpp"

namespace minko
{
    namespace geometry
    {
        class LuaGeometry
        {
        public:
            static
            void
            bind(LuaGlue& state)
            {
                state
                    .Class<Geometry>("Geometry")
                        .method("create",        &Geometry::create)
                    .end()
                    .Class<CubeGeometry>("CubeGeometry")
                        .method("create",         &CubeGeometry::create)
                    .end()
                    .Class<SphereGeometry>("SphereGeometry")
                        .method("create",         &SphereGeometry::create)
                    .end()
                    .Class<QuadGeometry>("QuadGeometry")
                        .method("create",         &QuadGeometry::create)
                    .end()
                    .Class<LineGeometry>("LineGeometry")
                        .method("create",         &LineGeometry::create)
                        .method("moveTo",        static_cast<LineGeometry::Ptr (LineGeometry::*)(float, float, float)>(&LineGeometry::moveTo))
                        .method("lineTo",        static_cast<LineGeometry::Ptr (LineGeometry::*)(float, float, float, uint)>(&LineGeometry::lineTo))
                        .property("numLines",    &LineGeometry::numLines)
                    .end();
            }
        };
    }
}
