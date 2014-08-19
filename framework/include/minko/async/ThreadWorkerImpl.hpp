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

#include "minko/async/Worker.hpp"

#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
# error "ThreadWorkerImpl is not available under Emscripten"
#endif

namespace minko
{
    namespace async
    {
        class Worker::WorkerImpl // ThreadWorkerImpl
        {
        public:
            void
            start(const std::vector<char>& input)
            {
                //std::cout << "ThreadWorkerImpl::start()" << std::endl;;

                _input = input;

                std::thread(&Worker::run, _that->shared_from_this(), std::ref(_input)).detach();
            }

            void
            poll()
            {
                // std::cout << "ThreadWorkerImpl::poll()" << std::endl;;

                std::lock_guard<std::mutex> lock(_mutex);

                while (!_messages.empty())
                {
                    //std::cout << "ThreadWorkerImpl::poll(): message execute" << std::endl;
                    _message->execute(_that->shared_from_this(), _messages.front());
                    _messages.pop();
                }
            }

            void
            post(Message message)
            {
                //std::cout << "ThreadWorkerImpl::post(): " << message.type << std::endl;
                std::lock_guard<std::mutex> lock(_mutex);

                // FIXME: Optimize here (useless copy).
                _messages.push(message);
            }

            Signal<Ptr, Message>::Ptr
            message()
            {
                return _message;
            }

            ~WorkerImpl()
            {
                //std::cout << "ThreadWorkerImpl::~ThreadWorkerImpl()" << std::endl;
            }

            WorkerImpl(Worker* that, const std::string& name) :
                _that(that),
                _name(name),
                _message(Signal<Ptr, Message>::create())
            {
                //std::cout << "ThreadWorkerImpl::ThreadWorkerImpl()" << std::endl;
            }

        private:
            Worker*                                         _that;
            std::string                                     _name;
            std::mutex                                      _mutex;
            std::queue<Message>                             _messages;
            std::shared_ptr<Signal<Ptr, Message>>           _message;
            std::vector<char>                               _input;
        };
    }
}
