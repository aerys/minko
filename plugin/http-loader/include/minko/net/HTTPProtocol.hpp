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
#include "minko/file/AbstractProtocol.hpp"
#include "minko/Any.hpp"

#include <stdarg.h>

namespace minko
{
    namespace net
    {
        class HTTPProtocol :
            public file::AbstractProtocol
        {
        public:
            typedef std::shared_ptr<HTTPProtocol>    Ptr;

        public:
            inline static
            Ptr
            create()
            {
                return std::shared_ptr<HTTPProtocol>(new HTTPProtocol());
            }

            void
            load();

        protected:
            HTTPProtocol();

            static void
            completeHandler(void*, void*, unsigned int);

#if defined(EMSCRIPTEN)
            static void
            wget2CompleteHandler(void*, void *, unsigned int*);

            static void
            wget2ErrorHandler(void*, int, const char*);

            static void
            wget2ProgressHandler(void*, int, int);
#endif

            static void
            errorHandler(void*, int code = 0, const char* = "");

            static void
            progressHandler(void*, int, int);

        protected:
            std::list<Any>
            _workerSlots;

            static
            std::list<std::shared_ptr<HTTPProtocol>>
            _runningLoaders;

            static uint
            _uid;

#if !defined(EMSCRIPTEN)
            static size_t
            curlWriteMemoryHandler(void*, size_t, size_t, void*);
#endif
        };
    }
}

namespace
{
    inline std::string format(const char* fmt, ...)
    {
        int size = 512;
        char* buffer = 0;
        buffer = new char[size];
        va_list vl;
        va_start(vl,fmt);
        int nsize = vsnprintf(buffer,size,fmt,vl);
        if(size<=nsize){//fail delete buffer and try again
            delete buffer; buffer = 0;
            buffer = new char[nsize+1];//+1 for /0
            nsize = vsnprintf(buffer,size,fmt,vl);
        }
        std::string ret(buffer);
        va_end(vl);
        delete buffer;
        return ret;
    }
}
