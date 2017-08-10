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

#include "minko/file/AbstractStreamedAssetParser.hpp"
#include "minko/file/LinkedAsset.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/StreamedAssetParserScheduler.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::file;

StreamedAssetParserScheduler::Parameters::Parameters() :
    maxNumActiveParsers(20),
    useJobBasedParsing(false),
    requestAbortingEnabled(true),
    abortableRequestProgressThreshold(0.5f)
{
}

StreamedAssetParserScheduler::StreamedAssetParserScheduler(Options::Ptr         options,
                                                           const Parameters&    parameters) :
    JobManager::Job(),
    _options(options),
    _entries(),
    _activeEntries(),
    _entriesToRemove(),
    _abortedRequests(),
    _parameters(parameters),
    _complete(false),
    _active(Signal<Ptr>::create()),
    _inactive(Signal<Ptr>::create()),
    _numBytesLoaded(0),
    _numPrimitivesLoaded(0),
    _numVerticesLoaded(0),
    _numRequestsExecuted(0)
{
}

StreamedAssetParserScheduler::~StreamedAssetParserScheduler()
{
    clear();
}

void
StreamedAssetParserScheduler::clear()
{
    _entriesToRemove.insert(_activeEntries.begin(), _activeEntries.end());
    _entriesToRemove.insert(_entries.begin(), _entries.end());

    clearEntriesToRemove();

    _entries.clear();
    _activeEntries.clear();
    _pendingDataEntries.clear();
    _abortedRequests.clear();

    _active = nullptr;
    _inactive = nullptr;

    _options = nullptr;

    _numBytesLoaded = 0;
    _numPrimitivesLoaded = 0;
    _numVerticesLoaded = 0;
    _numRequestsExecuted = 0;
}

void
StreamedAssetParserScheduler::addParser(AbstractStreamedAssetParser::Ptr parser)
{
    auto entryIt = _entries.insert(new ParserEntry(parser));
    auto entry = *entryIt.first;

    entry->parserErrorSlot = parser->error()->connect(
        [this, entry](AbstractParser::Ptr parser,
                      const Error&        error)
        {
            if (_abortedRequests.erase(entry) > 0)
                return;

            _entriesToRemove.insert(entry);
        }
    );

    entry->parserCompleteSlot = parser->AbstractParser::complete()->connect(
        [this, entry](AbstractParser::Ptr parser)
        {
            _entriesToRemove.insert(entry);
        }
    );

    startListeningToEntry(entry);
}

void
StreamedAssetParserScheduler::removeParser(AbstractStreamedAssetParser::Ptr parser)
{
    auto entryIt = std::find_if(
        _entries.begin(),
        _entries.end(),
        [&parser](ParserEntryPtr entry) -> bool { return entry->parser == parser; }
    );

    if (entryIt != _entries.end())
        _entriesToRemove.insert(*entryIt);
}

void
StreamedAssetParserScheduler::priority(float value)
{
    if (_priority == value)
        return;

    const auto previousValue = _priority;

    _priority = value;

    if (_priority <= 0.f)
        inactive()->execute(shared_from_this());
    else if (previousValue <= 0.f && !_activeEntries.empty())
        active()->execute(shared_from_this());
}

bool
StreamedAssetParserScheduler::complete()
{
    return _complete;
}

float
StreamedAssetParserScheduler::priority()
{
    if (_pendingDataEntries.empty() && (!hasPendingRequest() || _activeEntries.size() >= _parameters.maxNumActiveParsers))
        return 0.f;

    return _priority;
}

void
StreamedAssetParserScheduler::step()
{
    auto numExecutedRequests = 0;

    clearEntriesToRemove();

    while (hasPendingRequest() &&
           _activeEntries.size() < _parameters.maxNumActiveParsers &&
           numExecutedRequests < _parameters.maxNumActiveParsers)
    {
        auto entry = headingParser();

        if (!entry->parser->prepareForNextLodRequest())
            continue;

        popHeadingParser();

        const int previousNumActiveEntries = _activeEntries.size();
        const int numActiveEntries = previousNumActiveEntries + 1;

        _activeEntries.insert(entry);

        entryActivated(entry, numActiveEntries, previousNumActiveEntries);

        ++numExecutedRequests;
        executeRequest(entry);

        clearEntriesToRemove();
    }

    for (auto entry : _pendingDataEntries)
    {
        entry->parser->lodRequestFetchingComplete(entry->pendingData);

        std::vector<unsigned char>().swap(entry->pendingData);
    }

    _pendingDataEntries.clear();
}

void
StreamedAssetParserScheduler::beforeFirstStep()
{
}

void
StreamedAssetParserScheduler::afterLastStep()
{
}

bool
StreamedAssetParserScheduler::hasPendingRequest() const
{
    return !_entries.empty() && headingParser()->parser->priority() > 0.f;
}

StreamedAssetParserScheduler::ParserEntryPtr
StreamedAssetParserScheduler::headingParser() const
{
    return *_entries.begin();
}

StreamedAssetParserScheduler::ParserEntryPtr
StreamedAssetParserScheduler::popHeadingParser()
{
    auto entryIt = _entries.begin();
    auto entry = *entryIt;

    _entries.erase(entryIt);

    return entry;
}

void
StreamedAssetParserScheduler::clearEntriesToRemove()
{
    if (_entriesToRemove.empty())
        return;

    for (auto entry : _entriesToRemove)
        removeEntry(entry);

    _entriesToRemove.clear();
}

void
StreamedAssetParserScheduler::removeEntry(ParserEntryPtr entry)
{
    stopListeningToEntry(entry);

    entry->parserErrorSlot = nullptr;
    entry->parserCompleteSlot = nullptr;

    auto numErasedEntries = _entries.erase(entry);

    const int previousNumActiveEntries = _activeEntries.size();

    numErasedEntries += _activeEntries.erase(entry);

    const int numActiveEntries = _activeEntries.size();

    entryDeactivated(entry, numActiveEntries, previousNumActiveEntries);

    if (_entries.empty() && _activeEntries.empty())
    {
        _complete = true;
    }

    if (numErasedEntries > 0)
        delete entry;
}

void
StreamedAssetParserScheduler::executeRequest(ParserEntryPtr entry)
{
    stopListeningToEntry(entry);

    ++_numRequestsExecuted;

    auto parser = entry->parser;

    auto offset = 0;
    auto size = 0;

    parser->getNextLodRequestInfo(offset, size);

    auto linkedAsset = parser->linkedAsset();

    const auto& filename = linkedAsset->filename();

    auto options = _options->clone()
        ->parserFunction([](const std::string& extension) -> AbstractParser::Ptr
        {
            return nullptr;
        })
        ->seekingOffset(offset)
        ->seekedLength(size)
        ->loadAsynchronously(true)
        ->storeDataIfNotParsed(false);

    if (_parameters.requestAbortingEnabled)
    {
        options
            ->fileStatusFunction([this, entry](File::Ptr file, float progress) -> Options::FileStatus
            {
                auto status = Options::FileStatus::Pending;

                if (progress < 1.f && entry->parser->priority() <= 0.f)
                {
                    status = Options::FileStatus::Aborted;
                }
                else if (progress < _parameters.abortableRequestProgressThreshold)
                {
                    if (_priority <= 0.f)
                    {
                        status = Options::FileStatus::Aborted;
                    }
                    else
                    {
                        auto priorityRank = 0;

                        for (auto inactiveEntry : _entries)
                        {
                            if (priorityRank >= _parameters.maxNumActiveParsers ||
                                inactiveEntry->parser->priority() - entry->parser->priority() < 1e-3f)
                                break;

                            ++priorityRank;
                        }

                        if (priorityRank >= _parameters.maxNumActiveParsers)
                            status = Options::FileStatus::Aborted;
                    }
                }

                if (status == Options::FileStatus::Aborted)
                    _abortedRequests.insert(entry);

                return status;
            });
    }

    entry->loaderErrorSlot = linkedAsset->error()->connect(
        [this, entry, parser, filename](LinkedAsset::Ptr    loaderThis,
                                        const Error&        error) -> void
    {
        parser->lodRequestFetchingError(Error("StreamedAssetLoadingError", std::string("Failed to load streamed asset ") + filename));

        requestDisposed(entry);
    });

    entry->loaderCompleteSlot = linkedAsset->complete()->connect(
        [this, entry](LinkedAsset::Ptr                    loaderThis,
                      const std::vector<unsigned char>&   data) -> void
    {
        requestComplete(entry, data);
    });

    parser->lodRequestFetchingBegin();

    linkedAsset->resolve(options);
}

void
StreamedAssetParserScheduler::requestComplete(ParserEntryPtr entry, const std::vector<unsigned char>& data)
{
    _numBytesLoaded += data.size();

    entry->parserLodRequestCompleteSlot = entry->parser->lodRequestComplete()->connect(
        [this, entry](AbstractStreamedAssetParser::Ptr parser)
        {
            _numPrimitivesLoaded += parser->lodRequestNumPrimitivesLoaded();
            _numVerticesLoaded += parser->lodRequestNumVerticesLoaded();

            requestDisposed(entry);
        }
    );

    entry->parser->useJobBasedParsing(_parameters.useJobBasedParsing ? jobManager() : nullptr);

    if (_priority > 0.f)
        entry->parser->lodRequestFetchingComplete(data);
    else
    {
        _pendingDataEntries.insert(entry);

        entry->pendingData = data;
    }
}

void
StreamedAssetParserScheduler::requestDisposed(ParserEntryPtr entry)
{
    entry->loaderErrorSlot = nullptr;
    entry->loaderCompleteSlot = nullptr;
    entry->parserLodRequestCompleteSlot = nullptr;

    const int previousNumActiveEntries = _activeEntries.size();
    const int numActiveEntries = previousNumActiveEntries - 1;

    if (_activeEntries.erase(entry) == 0)
        return;

    entryDeactivated(entry, numActiveEntries, previousNumActiveEntries);

    _entries.insert(entry);

    startListeningToEntry(entry);
}

void
StreamedAssetParserScheduler::startListeningToEntry(ParserEntryPtr entry)
{
    entry->parserBeforePriorityChangedSlot = entry->parser->beforePriorityChanged()->connect(
        [this, entry](AbstractStreamedAssetParser::Ptr  parser,
               float                                    priority)
        {
            _entries.erase(entry);
        }
    );

    entry->parserPriorityChangedSlot = entry->parser->priorityChanged()->connect(
        [this, entry](AbstractStreamedAssetParser::Ptr  parser,
               float                                    priority)
        {
            _entries.insert(entry);
        }
    );
}

void
StreamedAssetParserScheduler::stopListeningToEntry(ParserEntryPtr entry)
{
    entry->parserBeforePriorityChangedSlot = nullptr;
    entry->parserPriorityChangedSlot = nullptr;
}

void
StreamedAssetParserScheduler::entryActivated(const ParserEntryPtr entry, int numActiveEntries, int previousNumActiveEntries)
{
    if (previousNumActiveEntries == 0 && numActiveEntries > 0)
        active()->execute(shared_from_this());
}

void
StreamedAssetParserScheduler::entryDeactivated(const ParserEntryPtr entry, int numActiveEntries, int previousNumActiveEntries)
{
    if (previousNumActiveEntries > 0 && numActiveEntries == 0)
        inactive()->execute(shared_from_this());
}
