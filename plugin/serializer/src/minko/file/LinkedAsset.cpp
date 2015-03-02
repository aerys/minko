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

#include "minko/file/LinkedAsset.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"

using namespace minko;
using namespace minko::file;

void
LinkedAsset::resolve(Options::Ptr options)
{
    switch (linkType())
    {
    case LinkType::Copy:
        complete()->execute(shared_from_this(), data());
        break;

    case LinkType::Internal:
    case LinkType::External:
    {
        auto loader = Loader::create();
        auto loaderOptions = options;

        loader->options(loaderOptions);

        loaderOptions
            ->seekingOffset(loaderOptions->seekingOffset() + offset())
            ->storeDataIfNotParsed(false);

        _loaderErrorSlot = loader->error()->connect(
            [=](Loader::Ptr     loaderThis,
                const Error&    error)
            {
                this->error()->execute(shared_from_this(), error);
            }
        );

        _loaderCompleteSlot = loader->complete()->connect(
            [=](Loader::Ptr loaderThis)
            {
                _loaderErrorSlot = nullptr;
                _loaderCompleteSlot = nullptr;

                complete()->execute(
                    shared_from_this(),
                    loaderThis->files().at(filename())->data()
                );
            }
        );

        loader
            ->queue(filename())
            ->load();

        break;
    }

    default:
        break;
    }
}
