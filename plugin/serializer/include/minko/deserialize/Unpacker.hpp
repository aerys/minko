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

#include "msgpack.hpp"

namespace minko
{
    namespace deserialize
    {
        template <typename T>
        void
        unpack(T& result, const char* source, std::size_t length, std::size_t offset = 0)
        {
            bool referenced;
            auto neverCopy = [](msgpack::type::object_type, std::size_t, void*) -> bool { return true ; };

            msgpack::unpacked unpacked;
            std::size_t _ = 0;
            msgpack::unpack(unpacked, source + offset, length, _, referenced, neverCopy);
            msgpack::object object(unpacked.get()); // Reference semantics. Shallow copy.
            object.convert(&result);
        }

        template <typename T>
        void
        unpack(T& result, const std::vector<unsigned char>& source, std::size_t length, std::size_t offset = 0)
        {
            unpack(result, reinterpret_cast<const char*>(&source[0]), length, offset);
        }
    }
}
