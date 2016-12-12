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

#include "WebSocketImpl.hpp"

#include "minko/AbstractCanvas.hpp"

#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_client.hpp"

namespace minko
{
    namespace net
    {
        class WebSocket;

        class NativeWebSocketImpl : public WebSocketImpl
        {
        public:
            void
            connect(const std::string& uri, const std::string &cookie = "") override;

            void
            disconnect() override;

            void
            sendMessage(const void* payload, size_t s) override;

            bool
            isConnected() override;

            bool
            poll(std::weak_ptr<WebSocket> webSocket) override;

            NativeWebSocketImpl();

            ~NativeWebSocketImpl();

        private:
            typedef websocketpp::client<websocketpp::config::asio_client> client;
            typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
            typedef websocketpp::client<websocketpp::config::asio_tls_client> tls_client;
            typedef websocketpp::config::asio_tls_client::message_type::ptr tls_message_ptr;

        private:
            void
            tlsConnect(const std::string& uri, const std::string& cookie = "");

            void
            pushCallback(std::function<void(std::weak_ptr<WebSocket>)> callback);

        private:
            client _client;
            client::connection_ptr _connection;
            tls_client _tlsClient;
            tls_client::connection_ptr _tlsConnection;
            websocketpp::lib::shared_ptr<websocketpp::lib::thread> _thread;
            std::mutex _connectionMutex;

            std::mutex _callbackMutex;
            std::list<std::function<void(std::weak_ptr<WebSocket>)>> _callbacks;
        };
    }
}
