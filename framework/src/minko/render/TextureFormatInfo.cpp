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

#include "minko/render/OpenGLES2Context.hpp"
#include "minko/render/TextureFormatInfo.hpp"

using namespace minko;
using namespace minko::render;

std::unordered_map<TextureFormat, TextureFormatInfo::Entry, Hash<TextureFormat>> TextureFormatInfo::_formats =
{
    { TextureFormat::RGB,               Entry("RGB",                false,    24u,   24u, false,  false) },
    { TextureFormat::RGBA,              Entry("RGBA",               false,    32u,   32u, true,   false) },

    { TextureFormat::RGB_DXT1,          Entry("RGB_DXT1",           true,     4u,    8u,  false,  false) },
    { TextureFormat::RGBA_DXT1,         Entry("RGBA_DXT1",          true,     4u,    8u,  true,   false) },
    { TextureFormat::RGBA_DXT3,         Entry("RGBA_DXT3",          true,     8u,    16u, true,   false) },
    { TextureFormat::RGBA_DXT5,         Entry("RGBA_DXT5",          true,     8u,    16u, true,   false) },

    { TextureFormat::RGB_ETC1,          Entry("RGB_ETC1",           true,     4u,    8u,  false,  false) },
    { TextureFormat::RGBA_ETC1,         Entry("RGBA_ETC1",          true,     4u,    8u,  true,   true)  },

    { TextureFormat::RGB_PVRTC1_2BPP,   Entry("RGB_PVRTC1_2BPP",    true,     2u,    32u, false,  false) },
    { TextureFormat::RGB_PVRTC1_4BPP,   Entry("RGB_PVRTC1_4BPP",    true,     4u,    32u, false,  false) },
    { TextureFormat::RGBA_PVRTC1_2BPP,  Entry("RGBA_PVRTC1_2BPP",   true,     2u,    32u, true,   false) },
    { TextureFormat::RGBA_PVRTC1_4BPP,  Entry("RGBA_PVRTC1_4BPP",   true,     4u,    32u, true,   false) },

    { TextureFormat::RGBA_PVRTC2_2BPP,  Entry("RGBA_PVRTC2_2BPP",   true,     2u,    32u, true,   false) },
    { TextureFormat::RGBA_PVRTC2_4BPP,  Entry("RGBA_PVRTC2_4BPP",   true,     4u,    32u, true,   false) },

    { TextureFormat::RGB_ATITC,         Entry("RGB_ATITC",          true,     8u,    16u, false,  false) },
    { TextureFormat::RGBA_ATITC,        Entry("RGBA_ATITC",         true,     8u,    16u, true,   false) }
};

bool
TextureFormatInfo::isSupported(TextureFormat format)
{
    const auto& availableFormats = OpenGLES2Context::availableTextureFormats();

    return availableFormats.find(format) != availableFormats.end();
}

const std::string&
TextureFormatInfo::name(TextureFormat format)
{
    return _formats.at(format)._name;
}

unsigned int
TextureFormatInfo::textureSize(TextureFormat format, unsigned int width, unsigned int height)
{
    return std::max(
        TextureFormatInfo::minimumSize(format),
        static_cast<unsigned int>(
            TextureFormatInfo::numBitsPerPixel(format) / 8.0f * width * height
        )
    );
}

bool
TextureFormatInfo::isCompressed(TextureFormat format)
{
    return _formats.at(format)._isCompressed;
}

unsigned int
TextureFormatInfo::numBitsPerPixel(TextureFormat format)
{
    return _formats.at(format)._numBitsPerPixel;
}

unsigned int
TextureFormatInfo::minimumSize(TextureFormat format)
{
    return _formats.at(format)._minimumSize;
}

bool
TextureFormatInfo::hasAlphaChannel(TextureFormat format)
{
    return _formats.at(format)._hasAlphaChannel;
}

bool
TextureFormatInfo::hasSeparateAlphaChannel(TextureFormat format)
{
    return _formats.at(format)._hasSeparateAlphaChannel;
}

std::list<TextureFormat>
TextureFormatInfo::textureFormats()
{
    auto formats = std::list<TextureFormat>();

    for (const auto& textureFormat : _formats)
        formats.push_back(textureFormat.first);

    return formats;
}
