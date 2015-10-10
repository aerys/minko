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

#include "minko/Hash.hpp"

namespace minko
{
    namespace render
    {
        class TextureFormatInfo
        {
        private:
            struct Entry
            {
                std::string     _name;

                bool            _isCompressed;

                unsigned int    _numBitsPerPixel;

                unsigned int    _minimumSize;

                bool            _hasAlphaChannel;
                bool            _hasSeparateAlphaChannel;

                Entry(const std::string&    name,
                      bool                  isCompressed,
                      unsigned int          numBitsPerPixel,
                      unsigned int          minimumSize,
                      bool                  hasAlphaChannel,
                      bool                  hasSeparateAlphaChannel) :
                    _name(name),
                    _isCompressed(isCompressed),
                    _numBitsPerPixel(numBitsPerPixel),
                    _minimumSize(minimumSize),
                    _hasAlphaChannel(hasAlphaChannel),
                    _hasSeparateAlphaChannel(hasSeparateAlphaChannel)
                {
                }
            };

        private:
            static std::unordered_map<TextureFormat, Entry, Hash<TextureFormat>> _formats;

        public:
            static
            bool
            isSupported(TextureFormat format);

            static
            unsigned int
            textureSize(TextureFormat format, unsigned int width, unsigned int height);

            static
            const std::string&
            name(TextureFormat format);

            static
            bool
            isCompressed(TextureFormat format);

            static
            unsigned int
            numBitsPerPixel(TextureFormat format);

            static
            unsigned int
            minimumSize(TextureFormat format);

            static
            bool
            hasAlphaChannel(TextureFormat format);

            static
            bool
            hasSeparateAlphaChannel(TextureFormat format);

            static
            std::list<TextureFormat>
            textureFormats();
        };
    }
}
