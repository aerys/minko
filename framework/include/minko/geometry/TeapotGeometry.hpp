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

namespace minko
{
    namespace geometry
    {
        class TeapotGeometry :
            public Geometry
        {
        public:
            typedef std::shared_ptr<TeapotGeometry>    Ptr;

        public:
            inline static
            Ptr
            create(std::shared_ptr<render::AbstractContext> context, uint divs = 10)
            {
                auto tp = std::shared_ptr<TeapotGeometry>(new TeapotGeometry());

                tp->initialize(context, divs);

                return tp;
            }

        private:
            static const uint     _patches[32][16];
            static const float    _points[306][3];

        private:
            TeapotGeometry();

            void
            initialize(std::shared_ptr<render::AbstractContext> context, const uint divs);

            void
            genPatchVertices(std::vector<std::vector<float>>& patch, const uint divs, std::vector<float>& vertices);

            void
            genPatchIndices(const uint offset, const uint divs, std::vector<unsigned short>& indices);

            void
            bernstein(float                  u,
                      std::vector<float>&    p0,
                      std::vector<float>&    p1,
                      std::vector<float>&    p2,
                      std::vector<float>&    p3,
                      std::vector<float>&    out);
        };
    }
}
