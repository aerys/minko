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

#pragma once

#include "minko/Common.hpp"
#include "minko/SerializerCommon.hpp"
#include "minko/StreamingCommon.hpp"
#include "minko/component/JobManager.hpp"

namespace minko
{
    namespace file
    {
        class StreamedAssetParserScheduler :
            public std::enable_shared_from_this<StreamedAssetParserScheduler>,
            public component::JobManager::Job
        {
        public:
            typedef std::shared_ptr<StreamedAssetParserScheduler> Ptr;

        private:
            struct ParserEntry;

            typedef std::shared_ptr<ParserEntry> ParserEntryPtr;

            struct ParserEntry
            {
                std::shared_ptr<AbstractStreamedAssetParser>                                    parser;

                Signal<std::shared_ptr<LinkedAsset>, const Error&>::Slot						loaderErrorSlot;
                Signal<std::shared_ptr<LinkedAsset>, const std::vector<unsigned char>&>::Slot	loaderCompleteSlot;

                Signal<std::shared_ptr<AbstractStreamedAssetParser>, float>::Slot               parserPriorityChangedSlot;
                Signal<std::shared_ptr<AbstractParser>>::Slot                                   parserCompleteSlot;

                inline
                explicit
                ParserEntry(std::shared_ptr<AbstractStreamedAssetParser> parser) :
                    parser(parser)
                {
                }
            };

        private:
            std::shared_ptr<Options>    _options;

            std::list<ParserEntryPtr>   _entries;
            std::list<ParserEntryPtr>   _activeEntries;

            int                         _maxNumActiveParsers;

            bool                        _complete;
            bool                        _sortingNeeded;

            Signal<Ptr>::Ptr            _active;
            Signal<Ptr>::Ptr            _inactive;

        public:
            inline
            static
            Ptr
            create(std::shared_ptr<Options> options)
            {
                return Ptr(new StreamedAssetParserScheduler(options));
            }

            void
            addParser(std::shared_ptr<AbstractStreamedAssetParser> parser);

            void
            removeParser(std::shared_ptr<AbstractStreamedAssetParser> parser);

            inline
            Signal<Ptr>::Ptr
            active()
            {
                return _active;
            }

            inline
            Signal<Ptr>::Ptr
            inactive()
            {
                return _inactive;
            }

			bool
			complete() override;

			void
			beforeFirstStep() override;

			void
			step() override;
			
			float
			priority() override;
			
			void
			afterLastStep() override;

        private:
            StreamedAssetParserScheduler(std::shared_ptr<Options> options);

            void
            sortingNeeded();

            bool
            hasPendingRequest() const;

            ParserEntryPtr
            headingParser() const;

            ParserEntryPtr
            popHeadingParser();

            void
            removeEntry(ParserEntryPtr entry);

            void
            executeRequest(ParserEntryPtr entry);

            void
            requestComplete(ParserEntryPtr entry, const std::vector<unsigned char>& data);

            void
            requestDisposed(ParserEntryPtr entry);

            void
            startListeningToEntry(ParserEntryPtr entry);

            void
            stopListeningToEntry(ParserEntryPtr entry);

            void
            entryActivated(ParserEntryPtr entry, int numActiveEntries, int previousNumActiveEntries);

            void
            entryDeactivated(ParserEntryPtr entry, int numActiveEntries, int previousNumActiveEntries);
        };
    }
}
