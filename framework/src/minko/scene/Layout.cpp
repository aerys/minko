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

/*static*/ const Layouts Layout::Group::DEFAULT						= 1 << 0;
///*static*/ const Layouts Layout::Group::STATIC						= 1 << 1;
/*static*/ const Layouts Layout::Group::IGNORE_RAYCASTING			= 1 << 16;
/*static*/ const Layouts Layout::Group::CULLING						= 1 << 17;

/*static*/ const Layouts Layout::Mask::NOTHING						= 0;
/*static*/ const Layouts Layout::Mask::EVERYTHING					= -1;
/*static*/ const Layouts Layout::Mask::COLLISIONS_DYNAMIC_DEFAULT	= EVERYTHING;
///*static*/ const Layouts Layout::Mask::COLLISIONS_STATIC_DEFAULT	= EVERYTHING ^ Group::STATIC;
/*static*/ const Layouts Layout::Mask::RAYCASTING_DEFAULT			= EVERYTHING ^ Group::IGNORE_RAYCASTING;
