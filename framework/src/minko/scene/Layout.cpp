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

#include "minko/scene/Layout.hpp"

using namespace minko;
using namespace minko::scene;

const Layout BuiltinLayout::DEFAULT             = 1 << 0;
const Layout BuiltinLayout::DEBUG_ONLY          = 1 << 1;
const Layout BuiltinLayout::STATIC              = 1 << 2;
const Layout BuiltinLayout::IGNORE_RAYCASTING   = 1 << 3;
const Layout BuiltinLayout::IGNORE_CULLING      = 1 << 4;
const Layout BuiltinLayout::HIDDEN              = 1 << 5;
const Layout BuiltinLayout::PICKING             = 1 << 6;
const Layout BuiltinLayout::INSIDE_FRUSTUM      = 1 << 7;
const Layout BuiltinLayout::MINOR_OBJECT        = 1 << 8;
const Layout BuiltinLayout::PICKING_DEPTH       = 1 << 9;

const Layout LayoutMask::NOTHING                    = 0;
const Layout LayoutMask::COLLISIONS_DYNAMIC_DEFAULT = EVERYTHING & ~BuiltinLayout::STATIC;
const Layout LayoutMask::EVERYTHING                 = 0xffffffff;
