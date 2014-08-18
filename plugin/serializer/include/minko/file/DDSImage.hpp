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
        class DDSImage
        {
        public:
            typedef std::shared_ptr<render::AbstractTexture> AbstractTexturePtr;

        private:
            struct DDSHeader
            {
                unsigned int        _size;
                unsigned int        _flags;
                unsigned int        _height;
                unsigned int        _width;
                unsigned int        _pitchOrLinearSize;
                unsigned int        _depth;
                unsigned int        _mipMapCount;
                unsigned int        _reserved1[11];
                struct DDSPixelFormat
                {
                    unsigned int    _size;
                    unsigned int    _flags;
                    unsigned int    _fourCC;
                    unsigned int    _rgbBitCount;
                    unsigned int    _rBitMask;
                    unsigned int    _gBitMask;
                    unsigned int    _bBitMask;
                    unsigned int    _aBitMask;
                }                   _pixelFormat;
                unsigned int        _caps1;
                unsigned int        _caps2;
                unsigned int        _reserved2[3];

            };

        private:
            DDSHeader _header;

        public:
            DDSImage();

            const std::vector<unsigned char>&
            data();

            bool
            load(const std::vector<unsigned char>& src,
                 std::vector<unsigned char>& dst,
                 unsigned int& width,
                 unsigned int& height,
                 unsigned int& size,
                 render::TextureType& textureType,
                 render::TextureFormat& textureFormat);

            void
            save(std::ostream& outputStream,
                 AbstractTexturePtr texture,
                 render::TextureFormat textureFormat,
                 const std::vector<unsigned char>& compressedData);

        private:
            static
            unsigned int
            dxtcSize(unsigned int width, unsigned int height, render::TextureFormat textureFormat);
        };
    }
}