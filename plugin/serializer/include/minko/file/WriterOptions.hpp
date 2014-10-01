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
#include "minko/SerializerCommon.hpp"
#include "minko/Types.hpp"

namespace minko
{
    namespace file
    {
        class WriterOptions:
            public std::enable_shared_from_this<WriterOptions>
        {
        public:
            typedef std::shared_ptr<WriterOptions>                              Ptr;

        private:
            typedef std::function<const std::string(const std::string&)>        UriFunction;

        private:
            bool                    _embedAll;
            bool                    _addBoundingBoxes;

            UriFunction             _outputAssetUriFunction;

            serialize::ImageFormat  _imageFormat;

        public:
            inline static
			Ptr
			create()
			{
                auto writerOptions = Ptr(new WriterOptions());

                return writerOptions;
			}

            inline
			bool
			embedAll() const
			{
				return _embedAll;
			}

			inline
			Ptr
			embedAll(bool value)
			{
				_embedAll = value;

				return shared_from_this();
			}

            inline
			bool
			addBoundingBoxes() const
			{
				return _addBoundingBoxes;
			}

			inline
			Ptr
			addBoundingBoxes(bool value)
			{
				_addBoundingBoxes = value;

				return shared_from_this();
			}

            inline
            const UriFunction&
            outputAssetUriFunction() const
            {
                return _outputAssetUriFunction;
            }

            inline
            Ptr
            outputAssetUriFunction(const UriFunction& func)
            {
                _outputAssetUriFunction = func;

                return shared_from_this();
            }

            inline
			serialize::ImageFormat
			imageFormat() const
			{
				return _imageFormat;
			}

			inline
			Ptr
			imageFormat(serialize::ImageFormat value)
			{
				_imageFormat = value;

				return shared_from_this();
			}

        private:
            WriterOptions();
        };
    }
}
