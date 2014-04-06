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

#include "minko/file/Loader.hpp"

#include "minko/file/AbstractProtocol.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/AssetLibrary.hpp"

using namespace minko;
using namespace minko::file;

Loader::Loader() :
    _options(Options::create()),
    _complete(Signal<Loader::Ptr>::create()),
    _progress(Signal<Loader::Ptr, float>::create()),
    _error(Signal<Loader::Ptr>::create())
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
    _filesQueue.push_back(filename);

    if (options)
        _filenameToOptions[filename] = Options::create(options);

    return std::dynamic_pointer_cast<Loader>(shared_from_this());
}


void
Loader::load()
{
    if (_filesQueue.empty())
    {
        _complete->execute(std::dynamic_pointer_cast<Loader>(shared_from_this()));
    }
    else
    {
        auto queue = _filesQueue;

        for (auto& filename : queue)
        {
            auto options = _filenameToOptions.count(filename)
                ? _filenameToOptions[filename]
                : _filenameToOptions[filename] = Options::create(_options);
            auto protocol =  options->protocolFunction()(filename);

            _files[filename] = protocol->file();

            _filesQueue.erase(std::find(_filesQueue.begin(), _filesQueue.end(), filename));
            _loading.push_back(filename);

            _protocolSlots.push_back(protocol->error()->connect(std::bind(
                &Loader::protocolErrorHandler,
                shared_from_this(),
                std::placeholders::_1
            )));
            _protocolSlots.push_back(protocol->complete()->connect(std::bind(
                &Loader::protocolCompleteHandler,
                shared_from_this(),
                std::placeholders::_1
            )));
#ifdef DEBUG
            std::cout << "Batchprotocol::load(): before " << filename << std::endl;
#endif // defined(DEBUG)

            protocol->load(filename, options);

#ifdef DEBUG
            std::cout << "Loader::load(): after " << filename << std::endl;
#endif // defined(DEBUG)
        }
    }
}

void
Loader::protocolErrorHandler(std::shared_ptr<AbstractProtocol> protocol)
{
#ifdef DEBUG
    std::cerr << "error: Loader::protocolErrorHandler(): " << protocol->file()->filename() << std::endl;
#endif // defined(DEBUG)

    throw std::invalid_argument(protocol->file()->filename());
}

void
Loader::protocolCompleteHandler(std::shared_ptr<AbstractProtocol> protocol)
{
#ifdef DEBUG
    std::cerr << "Loader::protocolCompleteHandler(): " << std::endl;
#endif // defined(DEBUG)

    _progress->execute(
        std::dynamic_pointer_cast<Loader>(shared_from_this()),
        (float)_loading.size() / (float)_protocolSlots.size()
    );

    auto filename = protocol->file()->filename();

    _loading.erase(std::find(_loading.begin(), _loading.end(), filename));
    //_filenameToProtocol.erase(protocol->filename());
    _filenameToOptions.erase(filename);
    
#ifdef DEBUG
    std::cerr << "Loader::finalize(): " << _loading.size() << " file(s) still loading, "
        << _filesQueue.size() << " file(s) in the queue" << std::endl;
#endif // defined(DEBUG)

    auto parsed = processData(
        filename,
        protocol->file()->resolvedFilename(),
        protocol->options(),
        protocol->file()->data()
    );

    if (!parsed)
        finalize();
}

bool
Loader::processData(const std::string&                      filename,
                         const std::string&                 resolvedFilename,
                         Options::Ptr                       options,
                         const std::vector<unsigned char>&  data)
{
    auto extension = filename.substr(filename.find_last_of('.') + 1);

    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    auto parser = _options->getParser(extension);

    if (parser)
    {
        _parserSlots[parser] = parser->complete()->connect(std::bind(
            &Loader::parserCompleteHandler,
            shared_from_this(),
            std::placeholders::_1
        ));

        try
        {
            parser->parse(
                filename,
                resolvedFilename,
                options,
                data,
                options->assetLibrary()
            );
        }
        catch (ParserError parserError)
        {
            if (_error->numCallbacks() != 0)
                _error->execute(shared_from_this());
#ifdef DEBUG
            else
                throw parserError;
#endif // defined(DEBUG)
        }
    }
    else
    {
        options->assetLibrary()->blob(filename, data);
    }

    return parser != nullptr;
}

void
Loader::parserCompleteHandler(AbstractParser::Ptr parser)
{
    _parserSlots.erase(parser);

    finalize();
}

void
Loader::finalize()
{
    if (_loading.size() == 0 && _filesQueue.size() == 0 && _parserSlots.size() == 0)
    {
        _protocolSlots.clear();
        _filenameToOptions.clear();

        _complete->execute(shared_from_this());
    }
}
