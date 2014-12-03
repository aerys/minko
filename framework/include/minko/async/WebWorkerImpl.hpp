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

#if !defined(EMSCRIPTEN)
# error "WebWorkerImpl is only available under Emscripten"
#endif

#include "emscripten/emscripten.h"

namespace minko
{
    namespace async
    {
        class Worker::WorkerImpl
        {
        public:
            void
            start(const std::vector<char>& input)
            {
                std::cout << "WebWorkerImpl::start()" << std::endl;;

                char* data = const_cast<char*>(&*input.begin());

                emscripten_call_worker(_handle, "minkoWorkerEntryPoint", data, input.size(), &messageHandler, this);
            }

            void
            poll()
            {
                // std::cout << "WebWorkerImpl::poll()" << std::endl;;

                if (!_messages.empty())
                {
                    std::cout << "WebWorkerImpl::poll(): message execute" << std::endl;
                    _message->execute(_that->shared_from_this(), _messages.front());
                    _messages.pop();
                }
            }

            void
            post(Message message)
            {
                emscripten_worker_respond(const_cast<char*>(message.type.c_str()), message.type.size());
                emscripten_worker_respond(&*message.data.begin(), message.data.size());
            }

            Signal<Ptr, Message>::Ptr
            message()
            {
                return _message;
            }

            ~WorkerImpl()
            {
                emscripten_destroy_worker(_handle);
            }

            WorkerImpl(Worker* that, const std::string& name) :
                _that(that),
                _message(Signal<Ptr, Message>::create())
            {
                std::string path = "minko-worker-" + name + ".js";
                _handle = emscripten_create_worker(path.c_str());
            }

        private:
            Worker*                                     _that;
            std::string                                 _messageType;
            std::queue<Message>                         _messages;
            std::shared_ptr<Signal<Ptr, Message>>       _message;
            int                                         _handle;

            static
            void
            messageHandler(char* data, int size, void* arg)
            {
                WorkerImpl* worker = static_cast<WorkerImpl*>(arg);

                std::cout << "WebWorkerImpl::messageHandler(): " << size << std::endl;;

                if (worker->_messageType.empty())
                {
                    std::cout << "WebWorkerImpl::messageHandler(): reading type" << std::endl;
                    worker->_messageType.assign(data, size);
                }
                else
                {
                    std::cout << "WebWorkerImpl::messageHandler(): reading data" << std::endl;

                    worker->_messages.push(Message { worker->_messageType }.set(std::vector<char>(data, data + size)));
                    worker->_messageType.erase();
                }
            }
        };
    }
}
