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

#include "minko/file/Loader.hpp"

#include "minko/file/AbstractProtocol.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/AbstractCache.hpp"

#include "minko/log/Logger.hpp"

using namespace minko;
using namespace minko::file;

Loader::Loader() :
    _options(Options::create()),
    _complete(Signal<Loader::Ptr>::create()),
    _buffer(Signal<Loader::Ptr>::create()),
    _progress(Signal<Loader::Ptr, float>::create()),
    _parsingProgress(Signal<Loader::Ptr, float>::create()),
    _error(Signal<Loader::Ptr, const Error&>::create()),
    _numFilesToParse(0)
{
}

Loader::Ptr
Loader::queue(const std::string& filename)
{
    return queue(filename, nullptr);
}

Loader::Ptr
Loader::queue(const std::string& filename, std::shared_ptr<Options> options)
{
    if (filename.empty())
        return std::static_pointer_cast<Loader>(shared_from_this());

    _filesQueue.push_back(filename);
    _filenameToOptions[filename] = (options ? options : _options);

    if (_filenameToNumAttempts.find(filename) == _filenameToNumAttempts.end())
        _filenameToNumAttempts[filename] = 0;

    return std::static_pointer_cast<Loader>(shared_from_this());
}

void
Loader::load()
{
    if (_filesQueue.empty())
    {
        _complete->execute(std::dynamic_pointer_cast<Loader>(shared_from_this()));

        return;
    }

    _numFiles = _filesQueue.size();
    _protocolToProgress.clear();

    auto queue = _filesQueue;

    for (auto& filename : queue)
    {
        auto options = _filenameToOptions[filename];
        auto assets = options->assetLibrary();

        const auto byteRangeRequested = options->seekedLength() > 0;

        if (!byteRangeRequested && assets && (
            assets->cubeTexture(filename) != nullptr
            || assets->texture(filename) != nullptr
            || assets->geometry(filename) != nullptr
            || assets->effect(filename) != nullptr
            || assets->symbol(filename) != nullptr
            || assets->sound(filename) != nullptr
            || assets->material(filename) != nullptr
            || assets->script(filename) != nullptr
            || assets->hasBlob(filename)))
        {
            _filesQueue.erase(std::find(_filesQueue.begin(), _filesQueue.end(), filename));
            _filenameToOptions.erase(filename);
            _filenameToNumAttempts.erase(filename);

            if (_filesQueue.empty())
                _complete->execute(std::dynamic_pointer_cast<Loader>(shared_from_this()));
            continue;
        }

        const auto& includePaths = options->includePaths();
        auto loadFile = false;
        auto resolvedFilename = options->uriFunction()(File::sanitizeFilename(filename));
        auto protocol = options->protocolFunction()(resolvedFilename);

        protocol->options(options);

        if (includePaths.empty() || protocol->isAbsolutePath(resolvedFilename))
        {
            loadFile = true;
        }
        else
        {
            for (const auto& includePath : includePaths)
            {
                resolvedFilename = options->uriFunction()(File::sanitizeFilename(*includePath + '/' + filename));

                protocol = options->protocolFunction()(resolvedFilename);

                protocol->options(options);

                if (protocol->fileExists(resolvedFilename))
                {
                    loadFile = true;

                    break;
                }
            }
        }

        if (loadFile)
        {
            _files[filename] = protocol->file();

            _filesQueue.erase(std::find(_filesQueue.begin(), _filesQueue.end(), filename));
            _loading.push_back(filename);

            auto that = shared_from_this();

            _protocolErrorSlots.emplace(
                protocol,
                protocol->error()->connect([that](AbstractProtocol::Ptr protocol)
                {
                    that->protocolErrorHandler(protocol);
                }
            ));

            _protocolCompleteSlots.emplace(
                protocol,
                protocol->complete()->connect([that](AbstractProtocol::Ptr protocol)
            {
                that->protocolCompleteHandler(protocol);
            }
            ));

            _protocolBufferSlots.emplace(
                protocol,
                protocol->buffer()->connect([that](AbstractProtocol::Ptr protocol)
            {
                that->protocolBufferHandler(protocol);
            }
            ));

            _protocolProgressSlots.emplace(
                protocol,
                protocol->progress()->connect([that](AbstractProtocol::Ptr protocol, float progress)
                {
                    that->protocolProgressHandler(protocol, progress);
                }
            ));

            protocol->load(filename, resolvedFilename, options);
        }
        else
        {
            auto error = Error(
                "ProtocolError",
                std::string("File does not exist: ") + filename +
                std::string(", include paths: ") + std::to_string(_options->includePaths(), ",")
            );

            errorThrown(error);
        }
    }
}

void
Loader::protocolErrorHandler(std::shared_ptr<AbstractProtocol> protocol)
{
    const auto& filename = protocol->file()->filename();
    auto optionsIt = _filenameToOptions.find(filename);
    auto options = optionsIt->second;

    _loading.remove(filename);

    _filenameToOptions.erase(optionsIt);

    _protocolErrorSlots.erase(protocol);
    _protocolCompleteSlots.erase(protocol);
    _protocolBufferSlots.erase(protocol);
    _protocolProgressSlots.erase(protocol);

    auto error = Error(
        "ProtocolError",
        std::string("Protocol error: ") + filename +
        std::string(", include paths: ") + std::to_string(options->includePaths(), ",")
    );

    const auto numAttempts = ++_filenameToNumAttempts[filename];

    if (options->retryOnErrorFunction() && options->retryOnErrorFunction()(filename, error, numAttempts))
    {
        queue(filename, options);
        load();

        return;
    }

    _filenameToNumAttempts.erase(filename);

    errorThrown(error);
}

void
Loader::protocolProgressHandler(std::shared_ptr<AbstractProtocol> protocol, float progress)
{
    if (_protocolToProgress[protocol] == progress)
        return;

    _protocolToProgress[protocol] = progress;

    float newTotalProgress = 0.f;

    for (auto protocolAndProgress : _protocolToProgress)
        newTotalProgress += protocolAndProgress.second / _numFiles;

    if (newTotalProgress > 1.0f)
        newTotalProgress = 1.0f;

    _progress->execute(
        std::dynamic_pointer_cast<Loader>(shared_from_this()),
        newTotalProgress
    );
}

void
Loader::protocolBufferHandler(std::shared_ptr<AbstractProtocol> protocol)
{
    _buffer->execute(std::dynamic_pointer_cast<Loader>(shared_from_this()));
}

void
Loader::protocolCompleteHandler(std::shared_ptr<AbstractProtocol> protocol)
{
    _protocolToProgress[protocol] = 1.f;

    auto filename = protocol->file()->filename();

    _loading.remove(filename);

    _filenameToOptions.erase(filename);
    _filenameToNumAttempts.erase(filename);

    _protocolErrorSlots.erase(protocol);
    _protocolCompleteSlots.erase(protocol);
    _protocolBufferSlots.erase(protocol);
    _protocolProgressSlots.erase(protocol);

    _numFilesToParse++;

    LOG_DEBUG("file '" << protocol->file()->filename() << "' loaded, "
        << _loading.size() << " file(s) still loading, "
        << _filesQueue.size() << " file(s) in the queue");

    const auto byteRangeRequested = protocol->options()->seekedLength() > 0;

    if (!protocol->file()->loadedFromCache() && protocol->options()->cache() && byteRangeRequested)
        protocol->options()->cache()->set(protocol->file(), protocol->options()->seekingOffset());

    auto parsed = processData(
        filename,
        protocol->file()->resolvedFilename(),
        protocol->options(),
        protocol->file()->data()
    );

    if (!parsed)
    {
        --_numFilesToParse;

        finalize();
    }
}

bool
Loader::processData(const std::string&                      filename,
                         const std::string&                 resolvedFilename,
                         Options::Ptr                       options,
                         const std::vector<unsigned char>&  data)
{
    auto extension = filename.substr(filename.find_last_of('.') + 1);

    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    auto parser = options->getParser(extension);

    if (parser)
    {
        _parserProgressSlots[parser] = parser->progress()->connect(std::bind(
            &Loader::parserProgressHandler,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2
        ));

        _parserCompleteSlots[parser] = parser->complete()->connect(std::bind(
            &Loader::parserCompleteHandler,
            shared_from_this(),
            std::placeholders::_1
        ));

        _parserErrorSlots[parser] = parser->error()->connect(std::bind(
                                                                       &Loader::parserErrorHandler,
                                                                       shared_from_this(),
                                                                       std::placeholders::_1,
                                                                       std::placeholders::_2
                                                                       ));

        parser->parse(filename, resolvedFilename, options, data, options->assetLibrary());
    }
    else
    {
        if (options->storeDataIfNotParsed())
        {
            if (extension != "glsl")
                LOG_DEBUG("no parser found for extension '" << extension << "'");

            options->assetLibrary()->blob(filename, data);
        }
    }

    return parser != nullptr;
}

void
Loader::parserProgressHandler(AbstractParser::Ptr parser, float progress)
{
    _parserToProgress[parser] = progress;

    float newTotalProgress = 0.f;

    for (auto parserAndProgress : _parserToProgress)
        newTotalProgress += parserAndProgress.second / _numFiles;

    if (newTotalProgress > 1.0f)
        newTotalProgress = 1.0f;

    _parsingProgress->execute(
        std::dynamic_pointer_cast<Loader>(shared_from_this()),
        newTotalProgress
    );
}

void
Loader::parserCompleteHandler(AbstractParser::Ptr parser)
{
    --_numFilesToParse;
    _parserCompleteSlots.erase(parser);

    _parserToProgress[parser] = 1.f;

    finalize();
}

void
Loader::parserErrorHandler(AbstractParser::Ptr parser, const Error& error)
{
    errorThrown(error);
}

void
Loader::finalize()
{
    if (_loading.size() == 0 && _filesQueue.size() == 0 && _numFilesToParse == 0)
    {
        _protocolErrorSlots.clear();
        _protocolCompleteSlots.clear();
        _protocolBufferSlots.clear();
        _protocolProgressSlots.clear();
        _parserErrorSlots.clear();
        _filenameToOptions.clear();
        _filenameToNumAttempts.clear();

        _complete->execute(shared_from_this());

        _protocolToProgress.clear();
        _files.clear();
    }
}

void
Loader::errorThrown(const Error& error)
{
    if (_error->numCallbacks() > 0)
    {
        _error->execute(shared_from_this(), error);
    }
    else
    {
        LOG_ERROR(error.type() << ": " << error.what());

        throw error;
    }
}
