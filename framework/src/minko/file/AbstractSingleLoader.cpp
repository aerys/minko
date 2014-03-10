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

#include "minko/file/AbstractSingleLoader.hpp"

#include "minko/file/Options.hpp"
#include "minko/file/AssetLibrary.hpp"

using namespace minko;
using namespace minko::file;

void
AbstractSingleLoader::selectParser()
{
    auto extension = _filename.substr(_filename.find_last_of('.') + 1);

    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    _parser = _options->getParser(extension);

#ifdef DEBUG
    if (!_parser)
        std::cerr << "warning: no parser found for file extension '" << extension << "'" << std::endl;
#endif // defined(DEBUG)
}

void
AbstractSingleLoader::processData()
{
    if (_parser)
    {
        _parserCompleteSlot = _parser->complete()->connect(std::bind(
            &AbstractSingleLoader::parserCompleteHandler,
            std::enable_shared_from_this<AbstractSingleLoader>::shared_from_this(),
            std::placeholders::_1
        ));

        try
        {
            _parser->parse(
                _filename,
                _resolvedFilename,
                _options,
                _data,
                _options->assetLibrary()
            );
        }
        catch (ParserError parserError)
        {
            /*
            if (_parseError->numCallbacks() != 0)
                _parseError->execute(std::enable_shared_from_this<AbstractSingleLoader>::shared_from_this(), _parser);
#ifdef DEBUG
            else
                std::cerr << parserError.what() << std::endl;
#endif // defined(DEBUG)
            */
        }
    }
    else
    {
        _options->assetLibrary()->blob(_filename, _data);
    }
}

void
AbstractSingleLoader::parserCompleteHandler(AbstractParser::Ptr parser)
{
    _parserCompleteSlot = nullptr;
    _complete->execute(std::enable_shared_from_this<AbstractLoader>::shared_from_this());
}
