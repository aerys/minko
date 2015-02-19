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

#ifdef FAR // The macro is defined by guiddef.h
# undef FAR
#endif

#ifdef NEAR // The macro is defined by guiddef.h
# undef NEAR
#endif

namespace minko
{
    namespace math
    {
        enum class ShapePosition
        {
            AROUND    = -2,
            INSIDE    = -1,
            LEFT      = 0,
            TOP       = 1,
            RIGHT     = 2,
            BOTTOM    = 3,
            NEAR      = 4,
            FAR       = 5
        };

        enum class PlanePosition
        {
            LEFT      = 0,
            TOP       = 1,
            RIGHT     = 2,
            BOTTOM    = 3,
            NEAR      = 4,
            FAR       = 5
        };
    }
}

namespace std
{
    template<>
    struct hash<minko::math::ShapePosition>
    {
        inline
        size_t
        operator()(const minko::math::ShapePosition& p) const
        {
            return static_cast<int>(p);
        }
    };
}

namespace minko
{
    namespace math
    {
        class AbstractShape
        {
        public:
            typedef std::shared_ptr<AbstractShape>    Ptr;

        public:
            virtual
            bool
            cast(std::shared_ptr<Ray> ray, float& distance) = 0;

            virtual
            ShapePosition
            testBoundingBox(std::shared_ptr<math::Box> box) = 0;

            virtual
            void
			updateFromMatrix(const math::mat4& matrix) = 0;
        };
    }
}
