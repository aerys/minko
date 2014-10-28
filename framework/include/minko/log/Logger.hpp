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
    namespace log
    {
        class Logger
        {
        public:
            typedef std::shared_ptr<Logger>            Ptr;

            enum Level
            {
                Debug,
                Info,
                Warning,
                Error
            };

            class Sink
            {
            public:
                typedef std::shared_ptr<Sink>        Ptr;

                virtual
                void
                write(const std::string& log, Level level) = 0;

                virtual
                ~Sink() = default;
            };

            static
            Ptr
            create(Level level, Sink::Ptr sink)
            {
                Ptr logger = std::shared_ptr<Logger>(new Logger(level, sink));

                initializing()->execute(logger);

                return logger;
            }

            void
            operator()(const std::string&            message,
                       Level                         level,
                       char const*                   function,
                       char const*                   file,
                       int                           line);

            static
            void
            defaultLogger(std::shared_ptr<Logger> logger)
            {
                _default = logger;
            }

            static
            Ptr
            defaultLogger()
            {
                return _default;
            }

            static
            std::shared_ptr<Signal<std::shared_ptr<Logger>>>
            initializing()
            {
                auto _initializing = Signal<std::shared_ptr<Logger>>::create();

                return _initializing;
            }

        private:
            Logger(Level level, Sink::Ptr sink) :
                _level(level),
                _sink(sink)
            {
            }

        private:
            Level                                                   _level;
            Sink::Ptr                                               _sink;
                                    
            static Ptr                                              _default;
        };
    }
}

// From http://stackoverflow.com/questions/8337300/c11-how-do-i-implement-convenient-logging-without-a-singleton
#define LOG(Logger_, Message_, Level_)                  \
    Logger_(                                            \
        static_cast<std::ostringstream&>(               \
            std::ostringstream().flush() << Message_    \
        ).str(),                                        \
        Level_,                                         \
        __FUNCTION__,                                   \
        __FILE__,                                       \
        __LINE__                                        \
    );

#ifdef NDEBUG
# define LOG_DEBUG(_) do {} while (0);
#else
# define LOG_DEBUG(Message_) LOG((*minko::log::Logger::defaultLogger()), Message_, minko::log::Logger::Level::Debug)
#endif

#define LOG_INFO(Message_) LOG((*minko::log::Logger::defaultLogger()), Message_, minko::log::Logger::Level::Info)
#define LOG_WARNING(Message_) LOG((*minko::log::Logger::defaultLogger()), Message_, minko::log::Logger::Level::Warning)
#define LOG_ERROR(Message_) LOG((*minko::log::Logger::defaultLogger()), Message_, minko::log::Logger::Level::Error)