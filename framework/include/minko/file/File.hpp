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

namespace minko
{
    namespace file
    {
        class File
        {
            friend class AbstractProtocol;
            friend class AbstractCache;

        public:
            typedef std::shared_ptr<File>       Ptr;

        private:
            std::string                         _filename;

            std::vector<unsigned char>          _data;
            std::vector<unsigned char>          _buffer;
            std::string                         _resolvedFilename;
            bool                                _loadedFromCache;

        public:
            inline static
            Ptr
            create()
            {
                return std::shared_ptr<File>(new File());
            }

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
            const std::vector<unsigned char>&
            buffer()
            {
                return _buffer;
            }

            inline
            const bool
            loadedFromCache()
            {
                return _loadedFromCache;
            }

            static
            std::string
            getCurrentWorkingDirectory();

            static
            std::string
            getBinaryDirectory();

            static
            std::string
            sanitizeFilename(const std::string& filename);

            static
            std::string
            canonizeFilename(const std::string& filename);

            static
            std::string
            removePrefixPathFromFilename(const std::string& filename);

            static
            std::string
            extractPrefixPathFromFilename(const std::string& filename);

            static
            std::string
            getExtension(const std::string& filename);

            static
            std::string
            replaceExtension(const std::string& filename, const std::string& extension);

        private:
            File() = default;
        };
    }
}
