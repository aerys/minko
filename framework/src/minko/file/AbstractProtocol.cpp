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

#include "minko/file/AbstractProtocol.hpp"

#include "minko/file/Options.hpp"
#include "minko/file/AbstractCache.hpp"

using namespace minko;
using namespace minko::file;

AbstractProtocol::AbstractProtocol() :
    _file(File::create()),
    _options(Options::create()),
    _complete(Signal<Ptr>::create()),
    _buffer(Signal<Ptr>::create()),
    _progress(Signal<Ptr, float>::create()),
    _error(Signal<Ptr>::create())
{
}

void
AbstractProtocol::load(const std::string&       filename,
                       const std::string&       resolvedFilename,
                       std::shared_ptr<Options> options)
{
    _options = options;
    _file->_filename = filename;
    _file->_resolvedFilename = resolvedFilename;
    _file->_loadedFromCache = options->seekedLength() > 0 && options->cache() && options->cache()->get(_file, options->seekingOffset(), options->seekedLength());

    if (_file->_loadedFromCache)
        _complete->execute(shared_from_this());
    else
        load();
}