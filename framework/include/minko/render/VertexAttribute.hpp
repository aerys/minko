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
#include "minko/Flyweight.hpp"

namespace minko
{
    namespace render
    {
        class VertexAttribute
        {
        public:
            using Type = uint32_t;

        public:
            static const Type NONE;
            static const Type POSITION;
            static const Type UV;
            static const Type NORMAL;
            static const Type TANGENT;
            static const Type COLOR;
            static const Type ANY;

            const int* resourceId;
            const uint* vertexSize;
            Flyweight<std::string> name;
            uint size;
            uint offset;

            bool
            operator==(const VertexAttribute& rhs) const
            {
                return resourceId == rhs.resourceId && vertexSize == rhs.vertexSize && name == rhs.name
                    && size == rhs.size && offset == rhs.offset;
            }
        };
    }
}
