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
#include "minko/file/AbstractLoader.hpp"

namespace minko
{
	namespace file
	{
		class HTTPLoader :
		    public AbstractLoader
		{
		public:
		    inline static
            Ptr
            create()
            {
                return std::shared_ptr<HTTPLoader>(new HTTPLoader());
            }

			typedef std::shared_ptr<HTTPLoader>	Ptr;
			void
			load(const std::string& filename, std::shared_ptr<Options> options);

		protected:
			HTTPLoader();

			static void
			completeHandler(void*, void*, int);

			static void
			errorHandler(void*);

			#ifndef EMSCRIPTEN
			static size_t
			curlWriteMemoryHandler(void*, size_t, size_t, void*);
			#endif

			static
			std::list<std::shared_ptr<HTTPLoader>>
			_runningLoaders;
		};
	}
}
