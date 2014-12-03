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

// Possible implementations:
// - Web Worker-based (web, sync)
// - Callback-based (web, async eg. xhr)
// - Thread-based (native)
// - Fork-based (native)

// Actual implementation loaded from Worker.cpp.

#if !defined(MINKO_WORKER_IMPL_WEBWORKER) && !defined(MINKO_WORKER_IMPL_WEBWORKER)
# if defined(EMSCRIPTEN)
#  define MINKO_WORKER_IMPL_WEBWORKER
# else
#  define MINKO_WORKER_IMPL_THREAD
# endif
#endif

#if defined(MINKO_WORKER_IMPL_WEBWORKER)
# define MINKO_DEFINE_WORKER(Class, Code)                                       \
    void minkoWorkerEntryPoint(char* data, int size)                            \
    {                                                                           \
        auto worker = Class ::create("");                                       \
        std::vector<char> input(data, data + size);                             \
        worker->run(input);                                                     \
    }                                                                           \
                                                                                \
    void Class ::run(const std::vector<char>& input)                            \
    {                                                                           \
        Code ;                                                                  \
    }
#endif

#if defined(MINKO_WORKER_IMPL_THREAD)
# define MINKO_DEFINE_WORKER(Class, Code)                                       \
    void Class ::run(const std::vector<char>& input)                            \
    {                                                                           \
        Code ;                                                                  \
    }
#endif

// Not specific to any implementation (at the moment).
#define MINKO_DECLARE_WORKER(Class)                                             \
    class Class : public ::minko::async::Worker                                 \
    {                                                                           \
    public:                                                                     \
        static Ptr create(const std::string& name)                              \
        {                                                                       \
            return std::shared_ptr<Class>(new Class(name));                     \
        }                                                                       \
                                                                                \
        void run(const std::vector<char>& input);                               \
                                                                                \
    private:                                                                    \
        Class(const std::string& name) : Worker(name)                           \
        {                                                                       \
        }                                                                       \
    };
