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

#include "minko/async/WorkerImpl.hpp"

namespace minko
{
    namespace async
    {
        class Worker :
            public std::enable_shared_from_this<Worker>
        {
        public:
            typedef std::shared_ptr<Worker>                                            Ptr;
            typedef std::function<void (Worker::Ptr, const std::vector<char>&)>        EntryPoint;

            struct Message
            {
                std::string type;
                std::vector<char> data;

                template<typename T>
                Message&
                set(T value)
                {
                    data.resize(sizeof(T));
                    T* dest = reinterpret_cast<T*>(&*data.begin());
                    *dest = value;
                    return *this;
                }

                Message&
                set(const std::vector<char>& value)
                {
                    data = value;
                    return *this;
                }
            };

        public:
            // Starts the worker.
            void
            start(const std::vector<char>& input);

            // Must be called. Register on this signal to get updates from the worker.
            Signal<Ptr, Message>::Ptr
            message();

            // Can be called from the worker code to send data back to the application.
            void
            post(Message message);

        public: // For compilation purposes. Anything below must not be called manually.

            // Automatically overloaded when calling MINKO_DEFINE_WORKER.
            virtual
            void
            run(const std::vector<char>& input) = 0;

            // Automated by the canvas.
            void
            poll();

            ~Worker();

        protected:
            Worker(const std::string& name);

        private:
            class WorkerImpl;
            std::unique_ptr<WorkerImpl> _impl;
        };
    }
}
