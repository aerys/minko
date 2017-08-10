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
        class WebSocket;
        typedef std::weak_ptr<WebSocket> WebSocketWPtr;

        class WebSocketImpl
        {
        public:
            inline
            minko::Signal<WebSocketWPtr>::Ptr
            connected()
            {
                return _connected;
            }

            inline
            minko::Signal<WebSocketWPtr>::Ptr
            disconnected()
            {
                return _disconnected;
            }

            inline
            minko::Signal<WebSocketWPtr, const std::vector<uint8_t>&>::Ptr
            messageReceived()
            {
                return _messageReceived;
            }

            virtual
            void
            connect(const std::string& uri, const std::string &cookie = "") = 0;

            virtual
            void
            disconnect() = 0;

            virtual
            void
            sendMessage(const void* payload, size_t s) = 0;

            virtual
            bool
            isConnected() =  0;

            virtual
            bool
            poll(std::weak_ptr<WebSocket> webSocket) = 0;

        protected:
            WebSocketImpl() :
                _connected(minko::Signal<WebSocketWPtr>::create()),
                _disconnected(minko::Signal<WebSocketWPtr>::create()),
                _messageReceived(minko::Signal<WebSocketWPtr, const std::vector<uint8_t>&>::create())
            {}

        private:
            minko::Signal<WebSocketWPtr>::Ptr _connected;
            minko::Signal<WebSocketWPtr>::Ptr _disconnected;
            minko::Signal<WebSocketWPtr, const std::vector<uint8_t>&>::Ptr _messageReceived;
        };
    }
}
