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
    namespace file
    {
        class TextureContainer
        {
        private:
            struct Header
            {
                unsigned int _magicNumber;

                unsigned int _width;
                unsigned int _height;

                unsigned int _size;
                unsigned int _pixelFormat;
                unsigned int _numBitsPerPixel;

                unsigned int _rBitMask;
                unsigned int _gBitMask;
                unsigned int _bBitMask;
                unsigned int _aBitMask;

                unsigned int _numFaces;
                unsigned int _numMipMaps;
            };

        public:
            static
            bool
            load(const std::vector<unsigned char>&  in,
                 std::vector<unsigned char>&        out,
                 unsigned int&                      width,
                 unsigned int&                      height,
                 unsigned int&                      size,
                 render::TextureType&               textureType,
                 render::TextureFormat&             textureFormat);

            static
            bool
            save(std::shared_ptr<render::AbstractTexture>   texture,
                 render::TextureFormat                      format,
                 const std::vector<unsigned char>&          in,
                 std::vector<unsigned char>&                out);

        private:
            TextureContainer() = delete;
        };
    }
}
