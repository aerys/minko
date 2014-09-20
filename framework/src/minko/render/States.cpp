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

#include "minko/render/States.hpp"

using namespace minko;
using namespace minko::render;

const float				    States::DEFAULT_PRIORITY                = 0.f;
const bool					States::DEFAULT_ZSORTED                 = false;
const Blending::Source		States::DEFAULT_BLENDING_SOURCE         = Blending::Source::ONE;
const Blending::Destination States::DEFAULT_BLENDING_DESTINATION    = Blending::Destination::ZERO;
const bool					States::DEFAULT_COLOR_MASK              = true;
const bool					States::DEFAULT_DEPTH_MASK              = true;
const CompareMode		    States::DEFAULT_DEPTH_FUNC              = CompareMode::LESS;
const TriangleCulling       States::DEFAULT_TRIANGLE_CULLING        = TriangleCulling::BACK;
const CompareMode			States::DEFAULT_STENCIL_FUNCTION        = CompareMode::ALWAYS;
const int					States::DEFAULT_STENCIL_REFERENCE       = 0;
const uint					States::DEFAULT_STENCIL_MASK            = 1;
const StencilOperation		States::DEFAULT_STENCIL_FAIL_OP         = StencilOperation::KEEP;
const StencilOperation		States::DEFAULT_STENCIL_ZFAIL_OP        = StencilOperation::KEEP;
const StencilOperation		States::DEFAULT_STENCIL_ZPASS_OP        = StencilOperation::KEEP;
const bool					States::DEFAULT_SCISSOR_TEST            = false;
const ScissorBox			States::DEFAULT_SCISSOR_BOX             = ScissorBox();
