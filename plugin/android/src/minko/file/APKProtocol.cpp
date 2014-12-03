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

#include "minko/file/APKProtocol.hpp"

#include "minko/file/Options.hpp"
#include "minko/Signal.hpp"

#include "SDL_rwops.h"

using namespace minko;
using namespace minko::file;

APKProtocol::APKProtocol()
{
}

void
APKProtocol::load()
{
    auto filename = _file->filename();
    auto options = _options;

    auto protocolPrefixPosition = filename.find("://");

    if (protocolPrefixPosition != std::string::npos)
    {
        filename = filename.substr(protocolPrefixPosition + 3);
    }

    _options = options;

    auto resolvedFilename = options->uriFunction()(File::sanitizeFilename(filename));

    SDL_RWops* file = SDL_RWFromFile(resolvedFilename.c_str(), "rb");

    if (!file)
        for (auto path : _options->includePaths())
        {
            resolvedFilename = options->uriFunction()(File::sanitizeFilename(path + '/' + filename));

            file = SDL_RWFromFile(std::string(resolvedFilename).c_str(), "rb");

            if (file)
                break;
        }

    auto loader = shared_from_this();

    if (file)
    {
        this->resolvedFilename(resolvedFilename);

        auto offset = options->seekingOffset();
        auto size = options->seekedLength() > 0 ? options->seekedLength() : file->size(file);

        _progress->execute(shared_from_this(), 0.0);

        data().resize(size);

        file->seek(file, offset, RW_SEEK_SET);
        file->read(file, (char*) &data()[0], size, 1);
        file->close(file);

        _progress->execute(loader, 1.0);

        _complete->execute(shared_from_this());
    }
    else
    {
        _error->execute(shared_from_this());
    }
}
