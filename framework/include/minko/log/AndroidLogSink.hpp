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
#include <cstdio>
#include <unistd.h>

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
            int     _fd[2];
            char    _buffer[1024];

            AndroidLogSink()
            {
                // From https://codelab.wordpress.com/2014/11/03/how-to-use-standard-output-streams-for-logging-in-android-apps/.

                // Make stdout line-buffered and stderr unbuffered.
                setvbuf(stdout, 0, _IOLBF, 0);
                setvbuf(stderr, 0, _IONBF, 0);

                // Create the pipe and redirect stdout and stderr.
                pipe(_fd);
                dup2(_fd[1], 1);
                dup2(_fd[1], 2);

                // Spawn the logging thread.
                std::thread(&AndroidLogSink::run, this).detach();
            }

            void
            run()
            {
                std::size_t r;

                while ((r = read(_fd[0], _buffer, sizeof _buffer)) > 0)
                {
                    int i = 0;

                    for (int j = i; j < r; ++j)
                    {
                        if (_buffer[j] == '\n')
                        {
                            write(std::string(_buffer + i, j - i), Logger::Level::Debug);
                            i = j + 1;
                        }
                    }

                    if (i < r)
                        write(std::string(_buffer + i, r - i), Logger::Level::Debug);
                }
            }
        };
    }
}
