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

AbstractStreamedAssetParser::AbstractStreamedAssetParser() :
    AbstractSerializerParser(),
    _linkedAsset(),
    _filename(),
    _resolvedFilename(),
    _fileOffset(0),
    _deferParsing(false),
    _dependencyId(0u),
    _headerIsRead(false),
    _readingHeader(false),
    _previousLod(-1),
    _currentLod(-1),
    _nextLodOffset(0),
    _nextLodSize(0),
    _loaderErrorSlot(),
    _loaderCompleteSlot(),
    _complete(false),
    _data(),
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
    if (this->data())
    {
        _dataPropertyChangedSlot = this->data()->propertyChanged().connect(
            [this](Provider::Ptr                    provider,
                   const Provider::PropertyName&    propertyName)
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
    }

    _assetLibrary = assetLibrary;
    _options = options;

    _filename = filename;
    _resolvedFilename = resolvedFilename;

    if (useDescriptor(filename, options, data, assetLibrary))
    {
        terminate();

        return;
    }

    if (_deferParsing)
        return;

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
    if (_readingHeader)
        return 0.f;

    return _priority;
}

bool
AbstractStreamedAssetParser::prepareForNextLodRequest()
{
    if (_deferParsing && !_headerIsRead)
    {
        if (!_readingHeader)
            parseStreamedAssetHeader();

        return false;
    }

    return true;
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
    if (!_jobManager.expired())
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

        _jobManager.lock()->pushJob(parsingJob);
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

    if (!_headerIsRead)
        return;

    nextLod(_previousLod, _requiredLod, _currentLod, _nextLodOffset, _nextLodSize);
}

void
AbstractStreamedAssetParser::priority(float priority)
{
    if (_priority == priority)
        return;

    if (_readingHeader)
    {
        _priority = priority;

        return;
    }

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

void
AbstractStreamedAssetParser::nextLod(int      previousLod,
                                     int      requiredLod,
                                     int&     nextLod,
                                     int&     nextLodOffset,
                                     int&     nextLodSize)
{
    auto lodRangeMinSize = 1;
    auto lodRangeMaxSize = 0;
    auto lodRangeRequestMinSize = 0;
    auto lodRangeRequestMaxSize = 0;

    lodRangeFetchingBound(
        previousLod,
        requiredLod,
        lodRangeMinSize,
        lodRangeMaxSize,
        lodRangeRequestMinSize,
        lodRangeRequestMaxSize
    );

    auto lowerLod = previousLod + 1;
    auto upperLod = lowerLod;

    auto requirementIsFulfilled = false;

    do
    {
        if (upperLod >= maxLod())
            break;

        const auto lodRangeSize = upperLod - lowerLod;

        if (lodRangeMinSize > 0 &&
            lodRangeSize < lodRangeMinSize)
        {
            ++upperLod;

            continue;
        }

        if (lodRangeMaxSize > 0 &&
            lodRangeSize >= lodRangeMaxSize)
            break;

        auto lodRangeRequestOffset = 0;
        auto lodRangeRequestSize = 0;

        lodRangeRequestByteRange(lowerLod, upperLod, lodRangeRequestOffset, lodRangeRequestSize);

        if (lodRangeRequestMaxSize > 0 &&
            lodRangeRequestSize >= lodRangeRequestMaxSize)
            break;

        if (lodRangeRequestMinSize == 0 || lodRangeRequestSize >= lodRangeRequestMinSize)
        {
            requirementIsFulfilled = true;
        }
        else
        {
            ++upperLod;
        }
    } while (!requirementIsFulfilled);

    lowerLod = std::min(maxLod(), lowerLod);
    upperLod = std::min(maxLod(), upperLod);

    nextLod = lodLowerBound(upperLod);
    lodRangeRequestByteRange(lowerLod, upperLod, nextLodOffset, nextLodSize);
}

void
AbstractStreamedAssetParser::parseStreamedAssetHeader()
{
    beforePriorityChanged()->execute(
        std::static_pointer_cast<AbstractStreamedAssetParser>(shared_from_this()),
        priority()
    );

    _readingHeader = true;

    priorityChanged()->execute(
        std::static_pointer_cast<AbstractStreamedAssetParser>(shared_from_this()),
        priority()
    );

    const auto assetHeaderSize = MINKO_SCENE_HEADER_SIZE + 2;

    auto headerOptions = _options->clone()
        ->loadAsynchronously(true)
        ->seekingOffset(0)
        ->seekedLength(assetHeaderSize)
        ->storeDataIfNotParsed(false)
        ->parserFunction([](const std::string&) -> AbstractParser::Ptr { return nullptr; });

    _loaderErrorSlot = _linkedAsset->error()->connect(
        [&](LinkedAsset::Ptr    linkedAssetThis,
            const Error&        error)
        {
            _loaderErrorSlot = nullptr;
            _loaderCompleteSlot = nullptr;

            _readingHeader = false;

            this->error()->execute(shared_from_this(), error);
        }
    );

    _loaderCompleteSlot = _linkedAsset->complete()->connect(
        [=](LinkedAsset::Ptr                    linkedAsset,
            const std::vector<unsigned char>&   linkedAssetData)
        {
            linkedAsset->filename(linkedAsset->lastResolvedFilename());

            const auto streamedAssetHeaderSizeOffset = assetHeaderSize - 2;

            const auto streamedAssetHeaderSize = assetHeaderSize +
                (linkedAssetData[streamedAssetHeaderSizeOffset] << 8) +
                linkedAssetData[streamedAssetHeaderSizeOffset + 1];

            headerOptions->seekedLength(streamedAssetHeaderSize);

            _loaderCompleteSlot = _linkedAsset->complete()->connect(
                [=](LinkedAsset::Ptr                    linkedAsset,
                    const std::vector<unsigned char>&   linkedAssetData)
                {
                    _loaderErrorSlot = nullptr;
                    _loaderCompleteSlot = nullptr;

                    _linkedAsset->offset(linkedAsset->offset() + streamedAssetHeaderSize + 2);

                    _headerIsRead = true;

                    beforePriorityChanged()->execute(
                        std::static_pointer_cast<AbstractStreamedAssetParser>(shared_from_this()),
                        priority()
                    );

                    _readingHeader = false;

                    priorityChanged()->execute(
                        std::static_pointer_cast<AbstractStreamedAssetParser>(shared_from_this()),
                        priority()
                    );

                    parseHeader(linkedAssetData, _options);

                    prepareNextLod();

                    parsed(_filename, _resolvedFilename, _options, linkedAssetData, _assetLibrary);

                    ready()->execute(std::static_pointer_cast<AbstractStreamedAssetParser>(shared_from_this()));
                }
            );

            _linkedAsset->resolve(headerOptions);
        }
    );

    _linkedAsset->resolve(headerOptions);
}
