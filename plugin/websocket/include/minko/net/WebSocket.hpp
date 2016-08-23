/*
Copyright (c) 2016 Aerys

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

#include "minko/Signal.hpp"
#include "minko/AbstractCanvas.hpp"

namespace minko
{
    namespace net
    {
        class WebSocketImpl;

        class WebSocket : public std::enable_shared_from_this<WebSocket>
        {
        public:
            typedef std::shared_ptr<WebSocket> Ptr;
            typedef std::weak_ptr<WebSocket> WeakPtr;

            Signal<std::weak_ptr<WebSocket>>::Ptr
            connected();

            Signal<std::weak_ptr<WebSocket>>::Ptr
            disconnected();

            Signal<std::weak_ptr<WebSocket>, std::vector<uint8_t>>::Ptr
            messageReceived();

            void
            connect(const std::string& uri);

            bool
            poll();

            void
            disconnect();

            bool
            isConnected();

            void
            sendMessage(const std::string& message);

            void
            sendMessage(const std::vector<uint8_t>& payload);

            void
            sendMessage(const void* payload, size_t s);

            WebSocket();

            WebSocket(AbstractCanvas::Ptr canvas);

            ~WebSocket();

        private:
            void
            startPollingOnEnterFrame();

            void
            stopPollingOnEnterFrame();

        private:
            WebSocketImpl* _impl;
            AbstractCanvas::Ptr _canvas;

            Signal<AbstractCanvas::Ptr, float, float>::Slot _enterFrameSlot;
        };
    }
}
