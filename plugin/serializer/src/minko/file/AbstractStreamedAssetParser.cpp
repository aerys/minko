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

#include "minko/component/JobManager.hpp"
#include "minko/data/Provider.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/file/AbstractStreamedAssetParser.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/file/LinkedAsset.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/StreamingOptions.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/VertexBuffer.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::data;
using namespace minko::deserialize;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::render;

AbstractStreamedAssetParser::AbstractStreamedAssetParser(Provider::Ptr data) :
    AbstractSerializerParser(),
    _linkedAsset(),
    _filename(),
    _resolvedFilename(),
    _fileOffset(0),
    _headerIsRead(false),
    _previousLod(-1),
    _currentLod(-1),
    _nextLodOffset(0),
    _nextLodSize(0),
    _loaderErrorSlot(),
    _loaderCompleteSlot(),
    _complete(false),
    _data(data),
    _dataPropertyChangedSlot(),
    _requiredLod(0),
    _priority(0.f),
    _priorityChanged(Signal<Ptr, float>::create()),
    _beforePriorityChanged(Signal<Ptr, float>::create()),
    _lodRequestComplete(Signal<Ptr>::create()),
    _ready(Signal<Ptr>::create()),
    _progress(Signal<Ptr, float>::create())
{
}

void
AbstractStreamedAssetParser::parse(const std::string&                 filename,
                                   const std::string&                 resolvedFilename,
                                   Options::Ptr                       options,
                                   const std::vector<unsigned char>&  data,
                                   AssetLibrary::Ptr                  assetLibrary)
{
    _dataPropertyChangedSlot = this->data()->propertyChanged().connect(
        [this](Provider::Ptr        provider,
               const Provider::PropertyName& propertyName)
        {
            if (*propertyName == "requiredLod")
            {
                requiredLod(provider->get<int>(propertyName));
            }
            else if (*propertyName == "priority")
            {
                priority(provider->get<float>(propertyName));
            }
        }
    );

    _assetLibrary = assetLibrary;
    _options = options;

    _filename = filename;
    _resolvedFilename = resolvedFilename;

    if (!_headerIsRead)
    {
        _headerIsRead = true;

        parseHeader(data, options);

        prepareNextLod();
    }

    parsed(filename, resolvedFilename, options, data, assetLibrary);

    ready()->execute(std::static_pointer_cast<AbstractStreamedAssetParser>(shared_from_this()));
}

float
AbstractStreamedAssetParser::priority()
{
    return _priority;
}

void
AbstractStreamedAssetParser::getNextLodRequestInfo(int& offset, int& size)
{
    offset = _nextLodOffset;
    size = _nextLodSize;
}

void
AbstractStreamedAssetParser::lodRequestFetchingBegin()
{
}

void
AbstractStreamedAssetParser::lodRequestFetchingProgress(float progressRate)
{
}

void
AbstractStreamedAssetParser::lodRequestFetchingError(const Error& error)
{
    this->error()->execute(shared_from_this(), error);
}

void
AbstractStreamedAssetParser::lodRequestFetchingComplete(const std::vector<unsigned char>& data)
{
    if (_jobManager)
    {
        auto parsingJob = ParsingJob::create(
            [this, data]() -> void
            {
                parseLod(_previousLod, _currentLod, data, _options);
            },
            [this]()
            {
                lodRequestComplete()->execute(
                    std::static_pointer_cast<AbstractStreamedAssetParser>(shared_from_this())
                );

                prepareNextLod();
            }
        );

        _jobManager->pushJob(parsingJob);
    }
    else
    {
        parseLod(_previousLod, _currentLod, data, _options);

        lodRequestComplete()->execute(
            std::static_pointer_cast<AbstractStreamedAssetParser>(shared_from_this())
        );

        prepareNextLod();
    }
}

void
AbstractStreamedAssetParser::parseLod(int                                previousLod,
                                      int                                currentLod,
                                      const std::vector<unsigned char>&  data,
                                      std::shared_ptr<Options>           options)
{
    lodParsed(previousLod, currentLod, data, options);
}

void
AbstractStreamedAssetParser::parseHeader(const std::vector<unsigned char>&   data,
                                         std::shared_ptr<Options>            options)
{
    readHeader(_filename, data, _assetExtension);

    const auto streamedAssetHeaderData = std::vector<unsigned char>(
        data.begin() + streamedAssetHeaderOffset(),
        data.end()
    );

    auto linkedAssetId = 0u;

    headerParsed(streamedAssetHeaderData, options, linkedAssetId);

    if (_linkedAsset == nullptr)
    {
        _linkedAsset = _dependency->getLinkedAssetReference(linkedAssetId);
    }
}

void
AbstractStreamedAssetParser::prepareNextLod()
{
    if (complete(_currentLod))
    {
        terminate();
    }
    else
    {
        _previousLod = _currentLod;

        nextLod(_previousLod, _requiredLod, _currentLod, _nextLodOffset, _nextLodSize);
    }
}

void
AbstractStreamedAssetParser::terminate()
{
    _dataPropertyChangedSlot = nullptr;

    completed();

    this->AbstractParser::complete()->execute(shared_from_this());
}

void
AbstractStreamedAssetParser::requiredLod(int requiredLod)
{
    if (_requiredLod == requiredLod)
        return;

    _requiredLod = requiredLod;

    nextLod(_previousLod, _requiredLod, _currentLod, _nextLodOffset, _nextLodSize);
}

void
AbstractStreamedAssetParser::priority(float priority)
{
    beforePriorityChanged()->execute(
        std::static_pointer_cast<AbstractStreamedAssetParser>(shared_from_this()),
        _priority
    );

    _priority = priority;

    priorityChanged()->execute(
        std::static_pointer_cast<AbstractStreamedAssetParser>(shared_from_this()),
        priority
    );
}
