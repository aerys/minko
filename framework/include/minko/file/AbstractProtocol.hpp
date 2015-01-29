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
#include "minko/file/File.hpp"

namespace minko
{
    namespace file
    {
        class AbstractProtocol :
            public std::enable_shared_from_this<AbstractProtocol>
        {
        public:
            typedef std::shared_ptr<AbstractProtocol>   Ptr;

        protected:
            std::shared_ptr<File>                       _file;
            std::shared_ptr<Options>                    _options;

            std::shared_ptr<Signal<Ptr, float>>         _progress;
            std::shared_ptr<Signal<Ptr>>                _complete;
            std::shared_ptr<Signal<Ptr>>                _error;

        public:
            virtual
            ~AbstractProtocol() = default;

            static Ptr
            create();

            inline
            std::shared_ptr<File>
            file()
            {
                return _file;
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
            std::shared_ptr<Signal<Ptr>>
            error()
            {
                return _error;
            }

            inline
            void
            load(const std::string&         filename,
                 const std::string&         resolvedFilename,
                 std::shared_ptr<Options>   options)
            {
                _options = options;
                _file->_filename = filename;
                _file->_resolvedFilename = resolvedFilename;

                load();
            }

            virtual
            void
            load() = 0;

            virtual
            bool
            fileExists(const std::string& filename) = 0;

            virtual
            bool
            isAbsolutePath(const std::string& filename) const = 0;

        protected:
            AbstractProtocol();

            inline
            const std::string&
            resolvedFilename()
            {
                return _file->_resolvedFilename;
            }

            inline
            std::vector<unsigned char>&
            data()
            {
                return _file->_data;
            }
        };

    }
}
