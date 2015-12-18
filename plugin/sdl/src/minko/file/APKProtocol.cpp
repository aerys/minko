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

#include "minko/AbstractCanvas.hpp"
#include "minko/Signal.hpp"
#include "minko/file/APKProtocol.hpp"

#include "minko/file/Options.hpp"

#include "SDL_rwops.h"

using namespace minko;
using namespace minko::file;

std::unordered_set<APKProtocol::Ptr> APKProtocol::_activeInstances;

APKProtocol::APKProtocol()
{
}

void
APKProtocol::load()
{
    auto resolvedFilename = this->resolvedFilename();

    auto options = _options;

    auto protocolPrefixPosition = resolvedFilename.find("://");

    if (protocolPrefixPosition != std::string::npos)
    {
        resolvedFilename = resolvedFilename.substr(protocolPrefixPosition + 3);
    }

    _options = options;

    SDL_RWops* file = SDL_RWFromFile(resolvedFilename.c_str(), "rb");

    auto loader = shared_from_this();

    if (file)
    {
        if (_options->loadAsynchronously() &&
            AbstractCanvas::defaultCanvas() != nullptr &&
            AbstractCanvas::defaultCanvas()->isWorkerRegistered("apk-protocol"))
        {
            file->close(file);

            auto worker = AbstractCanvas::defaultCanvas()->getWorker("apk-protocol");
            auto instance = std::static_pointer_cast<APKProtocol>(shared_from_this());

            _workerSlot = worker->message()->connect(
                [this, instance](async::Worker::Ptr, async::Worker::Message message)
                {
                    if (message.type == "complete")
                    {
                        data().assign(message.data.begin(), message.data.end());

                        complete()->execute(instance);

                        _activeInstances.erase(instance);

                        _workerSlot = nullptr;
                    }
                    else if (message.type == "progress")
                    {
                        // FIXME
                    }
                    else if (message.type == "error")
                    {
                        error()->execute(instance);

                        _activeInstances.erase(instance);

                        _workerSlot = nullptr;
                    }
                }
            );

            auto offset = options->seekingOffset();
            auto length = options->seekedLength();

            std::stringstream inputStream;

            inputStream.write(reinterpret_cast<const char*>(&offset), 4u);
            inputStream.write(reinterpret_cast<const char*>(&length), 4u);
            inputStream.write(resolvedFilename.data(), resolvedFilename.size());

            const auto input = inputStream.str();

            worker->start(std::vector<char>(input.begin(), input.end()));
        }
        else
        {
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
    }
    else
    {
        _error->execute(shared_from_this());
    }
}

bool
APKProtocol::fileExists(const std::string& filename)
{
    auto resolvedFilename = filename;

    auto protocolPrefixPosition = resolvedFilename.find("://");

    if (protocolPrefixPosition != std::string::npos)
    {
        resolvedFilename = filename.substr(protocolPrefixPosition + 3);
    }

    auto file = SDL_RWFromFile(resolvedFilename.c_str(), "rb");

    return file != nullptr;
}

bool
APKProtocol::isAbsolutePath(const std::string& filename) const
{
    return filename.find("://") != std::string::npos ||
           filename.find_first_of("/") == 0u;
}
