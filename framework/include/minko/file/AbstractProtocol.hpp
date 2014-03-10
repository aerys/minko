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

namespace minko
{
    namespace file
    {
        class AbstractProtocol :
            public std::enable_shared_from_this<AbstractProtocol>
        {
        public:
            typedef std::shared_ptr<AbstractProtocol> Ptr;

        protected:
            std::string                         _filename;
            std::shared_ptr<Options>            _options;

            std::vector<unsigned char>          _data;
            std::string                         _resolvedFilename;

            std::shared_ptr<Signal<Ptr, float>> _progress;
            std::shared_ptr<Signal<Ptr>>        _complete;
            std::shared_ptr<Signal<Ptr>>        _error;

        public:
            static Ptr
            create();

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

            inline
            const std::vector<unsigned char>&
            data()
            {
                return _data;
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
            load(const std::string& filename, std::shared_ptr<Options> options)
            {
                _filename = filename;
                _options = options;

                load();
            }

            virtual
            void
            load() = 0;

        protected:
            AbstractProtocol();

            std::string
            sanitizeFilename(const std::string& filename);
        };

    }
}
