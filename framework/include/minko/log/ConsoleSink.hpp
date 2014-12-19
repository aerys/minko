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

#include <iostream>

namespace minko
{
    namespace log
    {
        class ConsoleSink :
            public Logger::Sink
        {
        public:
            static
            Ptr
            create()
            {
                return std::shared_ptr<ConsoleSink>(new ConsoleSink());
            }

            void
            write(const std::string& log, Logger::Level level) override
            {
                std::cout << getLevelName(level) << ": " << log << std::endl;
            }

            std::string
            getLevelName(Logger::Level level) const
            {
                switch (level)
                {
                case Logger::Level::Debug:
                    return "debug";
                case Logger::Level::Info:
                    return "info";
                case Logger::Level::Warning:
                    return "warning";
                case Logger::Level::Error:
                    return "error";
                }

                return "info";
            }

        private:
            ConsoleSink() = default;
        };
    }
}
