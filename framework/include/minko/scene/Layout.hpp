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

namespace minko
{
    namespace scene
    {
        typedef uint32_t Layout;

        struct BuiltinLayout
        {
            static const Layout DEFAULT;
            static const Layout DEBUG_ONLY;
            static const Layout STATIC;
            static const Layout IGNORE_RAYCASTING;
            static const Layout IGNORE_CULLING;
            static const Layout HIDDEN;
            static const Layout PICKING;
            static const Layout INSIDE_FRUSTUM;
            static const Layout MINOR_OBJECT;
            static const Layout PICKING_DEPTH;
        };

        struct LayoutMask
        {
            static const Layout NOTHING;
            static const Layout EVERYTHING;
            static const Layout COLLISIONS_DYNAMIC_DEFAULT;
        };
    }
}
