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

#include "NativeWebSocketImpl.hpp"

#include "minko/AbstractCanvas.hpp"

using namespace minko::net;

NativeWebSocketImpl::NativeWebSocketImpl()
    : WebSocketImpl()
{
}

NativeWebSocketImpl::~NativeWebSocketImpl()
{
    disconnect();
}

void
NativeWebSocketImpl::tlsConnect(const std::string &uri)
{
    _tlsClient.set_access_channels(websocketpp::log::alevel::none);
    _tlsClient.clear_access_channels(websocketpp::log::alevel::all);

    _tlsClient.init_asio();
    _tlsClient.set_message_handler([&](websocketpp::connection_hdl c, tls_message_ptr msg)
    {
        auto payloadString = msg->get_payload();
        std::vector<uint8_t> payload(payloadString.begin(), payloadString.end());

        pushCallback([=](std::weak_ptr<WebSocket> s) { this->messageReceived()->execute(s, payload); });
    });

    _tlsClient.set_tls_init_handler([&](websocketpp::connection_hdl hdl)
    {
        websocketpp::lib::shared_ptr<asio::ssl::context> ctx(new asio::ssl::context(asio::ssl::context::tlsv12));

        return ctx;
    });

    _tlsClient.set_open_handler([&](websocketpp::connection_hdl hdl)
    {
        pushCallback([=](std::weak_ptr<WebSocket> s) { this->connected()->execute(s); });
    });

    websocketpp::lib::error_code ec;
    _tlsConnection = _tlsClient.get_connection(uri, ec);
    if (ec)
    {
        std::cerr << "could not create connection because: " << ec.message() << std::endl;
        return;
    }

    _tlsConnection->add_subprotocol("binary");

    _tlsClient.connect(_tlsConnection);
    _thread.reset(new websocketpp::lib::thread(&tls_client::run, &_tlsClient));
}

void
NativeWebSocketImpl::connect(const std::string &uri)
{
    if (uri.find("wss") == 0)
        return tlsConnect(uri);

    _client.set_access_channels(websocketpp::log::alevel::none);
    _client.clear_access_channels(websocketpp::log::alevel::all);

    _client.init_asio();
    _client.set_message_handler([&](websocketpp::connection_hdl c, message_ptr msg)
    {
        auto payloadString = msg->get_payload();
        std::vector<uint8_t> payload(payloadString.begin(), payloadString.end());

        pushCallback([=](std::weak_ptr<WebSocket> s) { this->messageReceived()->execute(s, payload); });
    });

    _client.set_open_handler([&](websocketpp::connection_hdl hdl)
    {
        pushCallback([=](std::weak_ptr<WebSocket> s) { this->connected()->execute(s); });
    });

    websocketpp::lib::error_code ec;
    _connection = _client.get_connection(uri, ec);
    if (ec)
    {
        std::cerr << "could not create connection because: " << ec.message() << std::endl;
        return;
    }

    _connection->add_subprotocol("binary");

    _client.connect(_connection);
    _thread.reset(new websocketpp::lib::thread(&client::run, &_client));
}

void
NativeWebSocketImpl::disconnect()
{
    if (!!_connection && _connection->get_state() != websocketpp::session::state::closed)
        _connection->close(0, "disconnect() called");
    else if (!!_tlsConnection && _tlsConnection->get_state() != websocketpp::session::state::closed)
        _tlsConnection->close(0, "disconnect() called");
    if (_thread->joinable())
        _thread->join();
}

void
NativeWebSocketImpl::sendMessage(const void* payload, size_t s)
{
    if (_connection)
        _client.send(_connection, payload, s, websocketpp::frame::opcode::BINARY);
    else if (_tlsConnection)
        _tlsClient.send(_tlsConnection, payload, s, websocketpp::frame::opcode::BINARY);
}

bool
NativeWebSocketImpl::isConnected()
{
    return !!_connection
        ? _connection->get_state() == websocketpp::session::state::open
        : !!_tlsConnection
            ? _tlsConnection->get_state() == websocketpp::session::state::open
            : false;
}

bool
NativeWebSocketImpl::poll(std::weak_ptr<WebSocket> webSocket)
{
    bool read = !_callbacks.empty();

    std::lock_guard<std::mutex> guard(_callbackMutex);
    for (auto& callback : _callbacks)
        callback(webSocket);
    _callbacks.clear();

    return read;
}

void
NativeWebSocketImpl::pushCallback(std::function<void(std::weak_ptr<WebSocket>)> callback)
{
    std::lock_guard<std::mutex> guard(_callbackMutex);
    _callbacks.push_back(callback);
}
