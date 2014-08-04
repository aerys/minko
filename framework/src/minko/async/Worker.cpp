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

#include "minko/async/Worker.hpp"
#include "minko/AbstractCanvas.hpp"

#if defined(MINKO_WORKER_IMPL_WEBWORKER)
# include "minko/async/WebWorkerImpl.hpp"
#elif defined(MINKO_WORKER_IMPL_THREAD)
# include "minko/async/ThreadWorkerImpl.hpp"
#endif

using namespace minko;
using namespace minko::async;

Worker::Worker(const std::string& name)
{
    _impl.reset(new WorkerImpl(this, name));
}

void
Worker::start(const std::vector<char>& input)
{
    _impl->start(input);
}

void
Worker::post(Message message)
{
    _impl->post(message);
}

void
Worker::poll()
{
    _impl->poll();
}

Signal<Worker::Ptr, Worker::Message>::Ptr
Worker::message()
{
    return _impl->message();
}

Worker::~Worker()
{
}
