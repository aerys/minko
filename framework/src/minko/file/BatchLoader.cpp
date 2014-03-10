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

#include "minko/file/BatchLoader.hpp"

#include "minko/file/AbstractSingleLoader.hpp"
#include "minko/file/Options.hpp"

using namespace minko;
using namespace minko::file;

BatchLoader::Ptr
BatchLoader::queue(const std::string& filename)
{
    return queue(filename, nullptr, nullptr);
}

BatchLoader::Ptr
BatchLoader::queue(const std::string& filename, std::shared_ptr<Options> options)
{
    return queue(filename, options, nullptr);
}

BatchLoader::Ptr
BatchLoader::queue(const std::string&				        filename,
                   std::shared_ptr<Options>			        options,
                   std::shared_ptr<AbstractSingleLoader>	loader)
{
    _filesQueue.push_back(filename);

    if (options)
        _filenameToOptions[filename] = Options::create(options);

    if (loader)
        _filenameToLoader[filename] = loader;

    return std::dynamic_pointer_cast<BatchLoader>(shared_from_this());
}


void
BatchLoader::load()
{
    if (_filesQueue.empty())
    {
        _complete->execute(std::dynamic_pointer_cast<BatchLoader>(shared_from_this()));
    }
    else
    {
        auto queue = _filesQueue;

        for (auto& filename : queue)
        {
            auto options = _filenameToOptions.count(filename)
                ? _filenameToOptions[filename]
                : _filenameToOptions[filename] = Options::create(this->options());
            auto loader = _filenameToLoader.count(filename)
                ? _filenameToLoader[filename]
                : _filenameToLoader[filename] = options->loaderFunction()(filename);

            _filesQueue.erase(std::find(_filesQueue.begin(), _filesQueue.end(), filename));
            _loading.push_back(filename);

            _loaderSlots.push_back(loader->error()->connect(std::bind(
                &BatchLoader::loaderErrorHandler,
                std::dynamic_pointer_cast<BatchLoader>(shared_from_this()),
                std::placeholders::_1
            )));
            _loaderSlots.push_back(loader->complete()->connect(std::bind(
                &BatchLoader::loaderCompleteHandler,
                std::dynamic_pointer_cast<BatchLoader>(shared_from_this()),
                std::placeholders::_1
            )));
#ifdef DEBUG
            std::cout << "BatchLoader::load(): before " << filename << std::endl;
#endif // defined(DEBUG)

            loader->load(filename, options);

#ifdef DEBUG
            std::cout << "BatchLoader::load(): after " << filename << std::endl;
#endif // defined(DEBUG)
        }
    }
}

void
BatchLoader::loaderErrorHandler(std::shared_ptr<AbstractLoader> l)
{
    auto loader = std::dynamic_pointer_cast<AbstractSingleLoader>(l);

#ifdef DEBUG
    std::cerr << "error: BatchLoader::loaderErrorHandler(): " << loader->filename() << std::endl;
#endif // defined(DEBUG)

    throw std::invalid_argument(loader->filename());
}

void
BatchLoader::loaderCompleteHandler(std::shared_ptr<AbstractLoader> l)
{
    auto loader = std::dynamic_pointer_cast<AbstractSingleLoader>(l);

#ifdef DEBUG
    std::cerr << "BatchLoader::loaderCompleteHandler(): " << std::endl;
#endif // defined(DEBUG)

    _progress->execute(
        std::dynamic_pointer_cast<BatchLoader>(shared_from_this()),
        (float)_loading.size() / (float)_loaderSlots.size()
    );

    finalize(loader->filename());
}

void
BatchLoader::finalize(const std::string& filename)
{
    _loading.erase(std::find(_loading.begin(), _loading.end(), filename));
    _filenameToLoader.erase(filename);
    _filenameToOptions.erase(filename);
    
#ifdef DEBUG
    std::cerr << "BatchLoader::finalize(): " << _loading.size() << " file(s) still loading, "
        << _filesQueue.size() << " file(s) in the queue" << std::endl;
#endif // defined(DEBUG)

    if (_loading.size() == 0 && _filesQueue.size() == 0)
    {
        _loaderSlots.clear();
        _filenameToLoader.clear();
        _filenameToOptions.clear();

        _complete->execute(std::dynamic_pointer_cast<BatchLoader>(shared_from_this()));
    }
}
