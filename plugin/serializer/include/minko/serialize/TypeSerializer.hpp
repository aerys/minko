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
    namespace serialize
    {
        class TypeSerializer
        {
        private :
            template <typename T, typename ST = T>
            static void
            write(std::stringstream& stream, const T& value)
            {
                stream.write(reinterpret_cast<const char*>(&value), sizeof (ST));
            }

        public:

            template <typename T, typename ST = T>
            static
            std::string
            serializeVector(std::vector<T> vect)
            {
                std::stringstream            stream;

                for (T value : vect)
                    write<T, ST>(stream, value);

                return stream.str();
            }

            static
            std::tuple<uint, std::string>
            serializeVector4(Any value);

            static
            std::tuple<uint, std::string>
            serializeVector3(Any value);

            static
            std::tuple<uint, std::string>
            serializeVector2(Any value);

            static
            std::tuple<uint, std::string>
            serializeMatrix4x4(Any value);

            static
            std::tuple<uint, std::string>
            serializeBlending(Any value);

            static
            std::tuple<uint, std::string>
            serializeCulling(Any value);

            static
            std::tuple<uint, std::string>
            serializeTexture(Any value);
        };
    }
}
