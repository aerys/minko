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

StreamedAssetParserScheduler::StreamedAssetParserScheduler(Options::Ptr options,
                                                           int          maxNumActiveParsers,
                                                           bool         useJobBasedParsing) :
    JobManager::Job(),
    _options(options),
    _entries(),
    _activeEntries(),
    _complete(false),
    _maxNumActiveParsers(maxNumActiveParsers),
    _useJobBasedParsing(useJobBasedParsing),
    _sortingNeeded(false),
    _active(Signal<Ptr>::create()),
    _inactive(Signal<Ptr>::create())
{
}

void
StreamedAssetParserScheduler::addParser(AbstractStreamedAssetParser::Ptr parser)
{
    auto entry = ParserEntryPtr(new ParserEntry(parser));

    _entries.push_back(entry);

    entry->parserCompleteSlot = parser->AbstractParser::complete()->connect(
        [=](AbstractParser::Ptr parserThis)
        {
            removeEntry(entry);
        }
    );

    startListeningToEntry(entry);

    sortingNeeded();
}

void
StreamedAssetParserScheduler::removeParser(AbstractStreamedAssetParser::Ptr parser)
{
    auto entryIt = std::find_if(
        _entries.begin(),
        _entries.end(),
        [&](ParserEntryPtr entry) -> bool { return entry->parser == parser; }
    );

    if (entryIt != _entries.end())
        removeEntry(*entryIt);
}

bool
StreamedAssetParserScheduler::complete()
{
    return _complete;
}

float
StreamedAssetParserScheduler::priority()
{
    sortEntries();

    if (!hasPendingRequest() || _activeEntries.size() >= _maxNumActiveParsers)
        return 0.f;

    return 10.f;
}

void
StreamedAssetParserScheduler::step()
{
    sortEntries();

    while (hasPendingRequest() && _activeEntries.size() < _maxNumActiveParsers)
    {
        auto entry = popHeadingParser();

        const int previousNumActiveEntries = _activeEntries.size();
        const int numActiveEntries = previousNumActiveEntries + 1;

        _activeEntries.push_back(entry);

        entryActivated(entry, numActiveEntries, previousNumActiveEntries);

        executeRequest(entry);
    }
}

void
StreamedAssetParserScheduler::beforeFirstStep()
{
}

void
StreamedAssetParserScheduler::afterLastStep()
{
}

void
StreamedAssetParserScheduler::sortingNeeded()
{
    _sortingNeeded = true;
}

void
StreamedAssetParserScheduler::sortEntries()
{
    if (_sortingNeeded)
    {
        _sortingNeeded = false;

        _entries.sort([](ParserEntryPtr left, ParserEntryPtr right) -> bool
        {
            return left->parser->priority() < right->parser->priority();
        });
    }
}

bool
StreamedAssetParserScheduler::hasPendingRequest() const
{
    return !_entries.empty() && headingParser()->parser->priority() > 0.f;
}

StreamedAssetParserScheduler::ParserEntryPtr
StreamedAssetParserScheduler::headingParser() const
{
    return _entries.back();
}

StreamedAssetParserScheduler::ParserEntryPtr
StreamedAssetParserScheduler::popHeadingParser()
{
    auto entry = headingParser();

    _entries.pop_back();

    return entry;
}

void
StreamedAssetParserScheduler::removeEntry(ParserEntryPtr entry)
{
    entry->parserCompleteSlot = nullptr;

    _entries.remove(entry);

    const int previousNumActiveEntries = _activeEntries.size();

    _activeEntries.remove(entry);

    const int numActiveEntries = _activeEntries.size();

    entryDeactivated(entry, numActiveEntries, previousNumActiveEntries);

    if (_entries.empty() && _activeEntries.empty())
    {
        _complete = true;
    }
}

void
StreamedAssetParserScheduler::executeRequest(ParserEntryPtr entry)
{
    stopListeningToEntry(entry);

    auto parser = entry->parser;

    auto offset = 0;
    auto size = 0;

    parser->getNextLodRequestInfo(offset, size);

    auto linkedAsset = parser->linkedAsset();

    const auto& filename = linkedAsset->filename();

    auto options = _options
        ->parserFunction([](const std::string& extension) -> AbstractParser::Ptr
        {
            return nullptr;
        })
        ->seekingOffset(offset)
        ->seekedLength(size)
        ->loadAsynchronously(true)
        ->storeDataIfNotParsed(false);

    entry->loaderErrorSlot = linkedAsset->error()->connect(
        [=](LinkedAsset::Ptr    loaderThis,
            const Error&        error) -> void
    {
        parser->error()->execute(
            parser,
            Error("StreamedAssetLoadingError", std::string("Failed to load streamed asset ") + filename)
        );

        requestDisposed(entry);
    });

    entry->loaderCompleteSlot = linkedAsset->complete()->connect(
        [=](LinkedAsset::Ptr                    loaderThis,
            const std::vector<unsigned char>&   data) -> void
    {
        requestComplete(entry, data);
    });

    linkedAsset->resolve(options);
}

void
StreamedAssetParserScheduler::requestComplete(ParserEntryPtr entry, const std::vector<unsigned char>& data)
{
    entry->parserLodRequestCompleteSlot = entry->parser->lodRequestComplete()->connect(
        [this, entry](AbstractStreamedAssetParser::Ptr parser)
        {
            requestDisposed(entry);
        }
    );

    entry->parser->useJobBasedParsing(_useJobBasedParsing ? jobManager() : nullptr);

    entry->parser->parseLodRequest(data);
}

void
StreamedAssetParserScheduler::requestDisposed(ParserEntryPtr entry)
{
    entry->loaderErrorSlot = nullptr;
    entry->loaderCompleteSlot = nullptr;
    entry->parserLodRequestCompleteSlot = nullptr;

    const int previousNumActiveEntries = _activeEntries.size();
    const int numActiveEntries = previousNumActiveEntries - 1;

    _activeEntries.remove(entry);

    entryDeactivated(entry, numActiveEntries, previousNumActiveEntries);

    _entries.push_back(entry);

    startListeningToEntry(entry);

    sortingNeeded();
}

void
StreamedAssetParserScheduler::startListeningToEntry(ParserEntryPtr entry)
{
    entry->parserPriorityChangedSlot = entry->parser->priorityChanged()->connect(
        [this](AbstractStreamedAssetParser::Ptr parser,
               float                            priority)
        {
            sortingNeeded();
        }
    );
}

void
StreamedAssetParserScheduler::stopListeningToEntry(ParserEntryPtr entry)
{
    entry->parserPriorityChangedSlot = nullptr;
}

void
StreamedAssetParserScheduler::entryActivated(ParserEntryPtr entry, int numActiveEntries, int previousNumActiveEntries)
{
    if (previousNumActiveEntries == 0 && numActiveEntries > 0)
        active()->execute(shared_from_this());
}

void
StreamedAssetParserScheduler::entryDeactivated(ParserEntryPtr entry, int numActiveEntries, int previousNumActiveEntries)
{
    if (previousNumActiveEntries > 0 && numActiveEntries == 0)
        inactive()->execute(shared_from_this());
}
