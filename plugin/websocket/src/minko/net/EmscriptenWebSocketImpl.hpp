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

#if defined(EMSCRIPTEN)

#include "WebSocketImpl.hpp"

#include "minko/AbstractCanvas.hpp"

#include "emscripten.h"

namespace minko
{
    namespace net{
        class WebSocket;

        class EmscriptenWebSocketImpl : public WebSocketImpl
        {
        public:
            void
            connect(const std::string& uri) override;

            void
            disconnect() override;

            void
            sendMessage(const void* payload, size_t s) override;

            inline
            bool
            isConnected() override
            {
                return _connected;
            }

            bool
            poll(std::weak_ptr<WebSocket> webSocket) override;

            EmscriptenWebSocketImpl();

            ~EmscriptenWebSocketImpl();

        private:

            class SocketCallbackBroker
            {
            private:
                static std::list<std::pair<int, EmscriptenWebSocketImpl*>> _sockets;
                static bool _initialized;

            public:
                static
                void
                registerSocket(int fd, EmscriptenWebSocketImpl* socket);

                static
                void
                unregisterSocket(int fd, EmscriptenWebSocketImpl* socket);

            private:
                static
                void
                initializeHandlers();

                static
                void
                openCallback(int fd, void* userData);

                static
                void
                listenCallback(int fd, void* userData);

                static
                void
                connectionCallback(int fd, void* userData);

                static
                void
                messageCallback(int fd, void* userData);

                static
                void
                closeCallback(int fd, void* userData);

                static
                EmscriptenWebSocketImpl*
                getSocketByFd(int fd);
            };

            int
            hostnameToIp(const char* hostname , char* ip);

            void
            pushCallback(std::function<void(std::weak_ptr<WebSocket>)> callback);

            void
            openCallback(int fd);

            void
            listenCallback(int fd);

            void
            connectionCallback(int fd);

            void
            messageCallback(int fd);

            void
            closeCallback(int fd);

        private:
            bool _connected;
            int _fd;

            std::list<std::function<void(std::weak_ptr<WebSocket>)>> _callbacks;
        };
    }
}


#endif // defined(EMSCRIPTEN)
