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

#include "minko/render/TextureFormatInfo.hpp"

using namespace minko;
using namespace minko::render;

std::map<TextureFormat, TextureFormatInfo::Format> TextureFormatInfo::_formats =
{
    { TextureFormat::RGB,               { false,    24 } },
    { TextureFormat::RGBA,              { false,    32 } },
        
    { TextureFormat::RGB_DXT1,          { true,     4 } },
    { TextureFormat::RGBA_DXT3,         { true,     8 } },
    { TextureFormat::RGBA_DXT5,         { true,     8 } },
        
    { TextureFormat::RGB_ETC1,          { true,     4 } },

    { TextureFormat::RGB_PVRTC1_2BPP,   { true,     2 } },
    { TextureFormat::RGB_PVRTC1_4BPP,   { true,     4 } },
    { TextureFormat::RGBA_PVRTC1_2BPP,  { true,     2 } },
    { TextureFormat::RGBA_PVRTC1_4BPP,  { true,     4 } },

    { TextureFormat::RGBA_PVRTC2_2BPP,  { true,     2 } },
    { TextureFormat::RGBA_PVRTC2_4BPP,  { true,     4 } }
};

bool
TextureFormatInfo::isCompressed(TextureFormat format)
{
    return _formats.at(format)._isCompressed;
}

int
TextureFormatInfo::numBitsPerPixel(TextureFormat format)
{
    return _formats.at(format)._numBitsPerPixel;
}
