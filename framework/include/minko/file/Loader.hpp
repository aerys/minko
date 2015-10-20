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

#include "minko/Signal.hpp"

namespace minko
{
    namespace file
    {
        class Loader :
            public std::enable_shared_from_this<Loader>
        {
        public:
            typedef std::shared_ptr<Loader>        Ptr;

        private:
            typedef std::shared_ptr<AbstractParser>                                         AbsParserPtr;
            typedef std::unordered_map<std::string, std::shared_ptr<Options>>               FilenameToOptions;
            typedef std::unordered_map<std::string, std::shared_ptr<File>>                  FilenameToFile;
            typedef std::unordered_map<std::shared_ptr<AbstractProtocol>, float>            ProtocolToProgress;
            typedef std::vector<Signal<std::shared_ptr<AbstractProtocol>>::Slot>            ProtocolSlots;
            typedef std::vector<Signal<std::shared_ptr<AbstractProtocol>, float>::Slot>     ProtocolProgressSlots;
            typedef std::unordered_map<std::shared_ptr<AbstractParser>, float>              ParserToProgress;
            typedef std::unordered_map<AbsParserPtr, Signal<AbsParserPtr, float>::Slot>     ParserProgressSlots;
            typedef std::unordered_map<AbsParserPtr, Signal<AbsParserPtr>::Slot>            ParserCompleteSlots;
            
            typedef std::unordered_map<AbsParserPtr, Signal<AbsParserPtr, const Error&>::Slot>            ParserErrorSlots;

        protected:
            std::shared_ptr<Options>                            _options;

            std::list<std::string>                              _filesQueue;
            std::list<std::string>                              _loading;
            FilenameToOptions                                   _filenameToOptions;
            FilenameToFile                                      _files;

            std::shared_ptr<Signal<Ptr, float>>                 _progress;
            std::shared_ptr<Signal<Ptr, float>>                 _parsingProgress;
            std::shared_ptr<Signal<Ptr>>                        _complete;
            std::shared_ptr<Signal<Ptr, const Error&>>          _error;

            ProtocolSlots                                       _protocolSlots;
            ProtocolProgressSlots                               _protocolProgressSlots;
            ParserProgressSlots                                 _parserProgressSlots;
            ParserCompleteSlots                                 _parserCompleteSlots;
            ParserErrorSlots                                    _parserErrorSlots;

            ProtocolToProgress                                  _protocolToProgress;
            ParserToProgress                                    _parserToProgress;

            int                                                 _numFiles;
        
        private:
            int                                                 _numFilesToParse;

        public:
            virtual ~Loader() = default;

            inline static
            Ptr
            create()
            {
                return std::shared_ptr<Loader>(new Loader());
            }

            inline static
            Ptr
            create(std::shared_ptr<Options> options)
            {
                auto copy = Loader::create();

                copy->_options = options;

                return copy;
            }

            inline static
            Ptr
            create(Ptr loader)
            {
                auto copy = Loader::create();

                copy->_options = loader->_options;

                return copy;
            }

            inline
            std::shared_ptr<Options>
            options()
            {
                return _options;
            }

            inline
            void
            options(std::shared_ptr<Options> options)
            {
                _options = options;
            }

            inline
            std::shared_ptr<Signal<Ptr>>
            complete()
            {
                return _complete;
            }

            inline
            std::shared_ptr<Signal<Ptr, float>>
            progress()
            {
                return _progress;
            }

            inline
            std::shared_ptr<Signal<Ptr, float>>
            parsingProgress()
            {
                return _parsingProgress;
            }

            inline
            std::shared_ptr<Signal<Ptr, const Error&>>
            error()
            {
                return _error;
            }

            inline
            const std::list<std::string>&
            filesQueue()
            {
                return _filesQueue;
            }

            inline
            bool
            loading() const
            {
                return _filesQueue.size() > 0 || _loading.size() > 0;
            }

            Ptr
            queue(const std::string& filename);

            Ptr
            queue(const std::string& filename, std::shared_ptr<Options> options);

            void
            load();

            inline
            const FilenameToFile&
            files()
            {
                return _files;
            }

        protected:
            Loader();

            void
            protocolErrorHandler(std::shared_ptr<AbstractProtocol> protocol);

            void
            protocolCompleteHandler(std::shared_ptr<AbstractProtocol> protocol);
            
            void
            protocolProgressHandler(std::shared_ptr<AbstractProtocol> protocol, float);

            void
            finalize();

            bool
            processData(const std::string&                 filename,
                        const std::string&                 resolvedFilename,
                        std::shared_ptr<Options>           options,
                        const std::vector<unsigned char>&  data);

            void
            parserProgressHandler(std::shared_ptr<AbstractParser> parser, float);

            void
            parserCompleteHandler(std::shared_ptr<AbstractParser> parser);
            
            void
            parserErrorHandler(std::shared_ptr<AbstractParser> parser, const Error& error);

            void
            errorThrown(const Error& error);
        };
    }
}
