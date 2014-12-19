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

#include "minko/file/PNGWriter.hpp"

#include "lodepng.h"

using namespace minko::file;

void
PNGWriter::write(const std::string&                 filename,
                 const std::vector<unsigned char>&  data,
                 minko::uint                        width,
                 minko::uint                        height)
{
    unsigned status = lodepng::encode(filename, data, width, height);

    if (status != 0)
        throw std::runtime_error(std::string("PNGWriter::write: ") + lodepng_error_text(status));
}

void
PNGWriter::writeToStream(std::vector<unsigned char>&       destination,
                         const std::vector<unsigned char>& source,
                         minko::uint                       width,
                         minko::uint                       height)

{
    unsigned status = lodepng::encode(destination, source, width, height);

    if (status != 0)
        throw std::runtime_error(std::string("PNGWriter::writeToStream: ") + lodepng_error_text(status));
}