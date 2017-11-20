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

#include "minko/log/Logger.hpp"
#include "minko/log/ConsoleSink.hpp"

#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
# include "minko/log/AndroidLogSink.hpp"
#endif

using namespace minko;
using namespace minko::log;

#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
  Logger::Ptr
  Logger::_default = Logger::create(Logger::Level::Debug, AndroidLogSink::create());
#else
  Logger::Ptr
  Logger::_default = Logger::create(Logger::Level::Debug, ConsoleSink::create());
#endif

void
Logger::operator()(const std::string&	message,
                   Level 				level,
                   const char*          function,
                   const char*          file,
                   int                  line)
{
    if (static_cast<int>(level) < static_cast<int>(_level))
        return;

    std::string filename = std::string(file);
    filename = filename.substr(filename.find_last_of("\\/") + 1);

    std::ostringstream os;

    os << filename << ":" << line << "\t" << function << "(): " << message;

    _sink->write(os.str(), level);
}
