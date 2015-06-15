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
        class LinkedAsset :
            public std::enable_shared_from_this<LinkedAsset>
        {
        public:
            typedef std::shared_ptr<LinkedAsset> Ptr;

            enum class LinkType
            {
                Copy,
                Internal,
                External
            };

        private:
            int                                                     _offset;
            int                                                     _length;

            std::string                                             _filename;
            std::string                                             _lastResolvedFilename;
            std::vector<unsigned char>                              _data;

            LinkType                                                _linkType;

            Signal<Ptr, const std::vector<unsigned char>&>::Ptr     _complete;
            Signal<Ptr, const Error&>::Ptr                          _error;

            Signal<std::shared_ptr<Loader>>::Slot                   _loaderCompleteSlot;
            Signal<std::shared_ptr<Loader>, const Error&>::Slot     _loaderErrorSlot;

        public:
            inline
            static
            Ptr
            create()
            {
                auto instance = Ptr(new LinkedAsset());

                return instance;
            }

            inline
            int
            offset() const
            {
                return _offset;
            }

            inline
            Ptr
            offset(int value)
            {
                _offset = value;

                return shared_from_this();
            }

            inline
            int
            length() const
            {
                return _length;
            }

            inline
            Ptr
            length(int value)
            {
                _length = value;

                return shared_from_this();
            }

            inline
            const std::string&
            filename() const
            {
                return _filename;
            }

            inline
            Ptr
            filename(const std::string& value)
            {
                _filename = value;

                return shared_from_this();
            }

            inline
            const std::string&
            lastResolvedFilename() const
            {
                return _lastResolvedFilename;
            }

            inline
            const std::vector<unsigned char>&
            data() const
            {
                return _data;
            }

            inline
            Ptr
            data(const std::vector<unsigned char>& data)
            {
                _data = data;

                return shared_from_this();
            }

            inline
            LinkType
            linkType() const
            {
                return _linkType;
            }

            inline
            Ptr
            linkType(LinkType value)
            {
                _linkType = value;

                return shared_from_this();
            }

            inline
            Signal<Ptr, const std::vector<unsigned char>&>::Ptr
            complete() const
            {
                return _complete;
            }

            inline
            Signal<Ptr, const Error&>::Ptr
            error() const
            {
                return _error;
            }

            void
            resolve(std::shared_ptr<Options> options);

        private:
            LinkedAsset() :
                _offset(0),
                _length(0),
                _filename(),
                _lastResolvedFilename(),
                _data(),
                _linkType(LinkType::Internal),
                _complete(Signal<Ptr, const std::vector<unsigned char>&>::create()),
                _error(Signal<Ptr, const Error&>::create())
            {
            }
        };
    }
}
