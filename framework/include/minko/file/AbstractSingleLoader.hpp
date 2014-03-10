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

#pragma once

#include "minko/Common.hpp"

#include "minko/Signal.hpp"
#include "minko/file/AbstractLoader.hpp"

namespace minko
{
    namespace file
    {
        class AbstractSingleLoader :
            public AbstractLoader
        {
        public:
            typedef std::shared_ptr<AbstractSingleLoader> Ptr;

        private:
            typedef std::shared_ptr<AbstractParser>                     AbsParserPtr;
            typedef Signal<std::shared_ptr<file::AbstractParser>>::Slot AbsParserPtrSlot;

        protected:
            std::string                     _filename;
            std::string                     _resolvedFilename;

            AbsParserPtr                    _parser;

            AbsParserPtrSlot	            _parserCompleteSlot;

        public:
            inline
            const std::string&
            filename()
            {
                return _filename;
            }

            inline
            const std::string&
            resolvedFilename()
            {
                return _resolvedFilename;
            }

            void
            load(const std::string& filename, std::shared_ptr<Options> options = nullptr)
            {
                _filename = filename;
                if (options)
                    this->options(options);

                load();
            }

            virtual
            void
            load() = 0;

        protected:
            void
            selectParser();

            virtual
            void
            processData();

        private:
            void
            parserCompleteHandler(std::shared_ptr<AbstractParser> parser);
        };
    }
}
