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

#include "minko/log/Logger.hpp"
#include <android/log.h>

namespace minko
{
    namespace log
    {
        class AndroidLogSink :
            public Logger::Sink
        {
        public:
            static
            Ptr
            create()
            {
                return std::shared_ptr<AndroidLogSink>(new AndroidLogSink());
            }

            void
            write(const std::string& log, Logger::Level level)
            {
                auto logLevel = ANDROID_LOG_INFO;

                if (level == Logger::Level::Debug)
                    logLevel = ANDROID_LOG_DEBUG;
                else if (level == Logger::Level::Info)
                    logLevel = ANDROID_LOG_INFO;
                else if (level == Logger::Level::Warning)
                    logLevel = ANDROID_LOG_WARN;
                else if (level == Logger::Level::Error)
                    logLevel = ANDROID_LOG_ERROR;

                __android_log_print(logLevel, "minko-cpp", log.c_str());
            }

        private:
            AndroidLogSink() = default;
        };
    }
}
