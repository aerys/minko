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

#include "minko/file/JPEGWriter.hpp"

#include "jpge.h"

using namespace minko;
using namespace minko::file;

JPEGWriter::JPEGWriter() :
    _error(Signal<Ptr, const Error&>::create())
{
}

void
JPEGWriter::encode(std::vector<unsigned char>&       out,
                   const std::vector<unsigned char>& in,
                   minko::uint                       width,
                   minko::uint                       height,
                   minko::uint                       numComponents)
{
    static const auto minBufferSize = 1024;

    int bufferSize = math::max<int>(width * height * numComponents, minBufferSize);
    out.resize(bufferSize);

    auto params = jpge::params();
    params.m_quality = 90;

    const auto success = jpge::compress_image_to_jpeg_file_in_memory(
        out.data(),
        bufferSize,
        width,
        height,
        numComponents,
        in.data(),
        params
    );

    if (!success)
    {
        error()->execute(shared_from_this(), Error("ParseError", "Failed to encode image to JPEG"));

        return;
    }

    out.resize(bufferSize);
}
