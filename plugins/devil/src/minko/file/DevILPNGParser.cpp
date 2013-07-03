/*
Copyright (c) 2013 Aerys

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

#include "DevILPNGParser.hpp"

#include "minko/resource/Texture.hpp"
#include "minko/file/Options.hpp"

#include "il.h"

using namespace minko::file;

void
DevILPNGParser::parse(const std::string&		filename,
				      std::shared_ptr<Options>	options,
				      const std::vector<char>&	data)
{
    ilInit();
    ilLoadL(IL_PNG, &data[0], data.size());
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    _texture = resource::Texture::create(
        options->context(), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT)
    );
    _texture->data(ilGetData());
    _texture->upload();
}
