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

#include "EmscriptenWebSocketImpl.hpp"

#if defined(EMSCRIPTEN)
#include "emscripten.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

using namespace minko::net;

EmscriptenWebSocketImpl::EmscriptenWebSocketImpl()
    : WebSocketImpl()
{
    emscripten_set_socket_open_callback(this, EmscriptenWebSocketImpl::openCallback);
    emscripten_set_socket_listen_callback(this, EmscriptenWebSocketImpl::listenCallback);
    emscripten_set_socket_connection_callback(this, EmscriptenWebSocketImpl::connectionCallback);
    emscripten_set_socket_message_callback(this, EmscriptenWebSocketImpl::messageCallback);
    emscripten_set_socket_close_callback(this, EmscriptenWebSocketImpl::closeCallback);
}

EmscriptenWebSocketImpl::~EmscriptenWebSocketImpl()
{
    disconnect();
}

void
EmscriptenWebSocketImpl::connect(const std::string& uri)
{
    _fd = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_fd == -1)
    {
        std::cerr << "failed to create client socket" << std::endl;
        return ;
    }
    fcntl(_fd, F_SETFL, O_NONBLOCK);

    std::regex uriRegex("^ws://(.*):(.*)/?.*$");
    std::smatch uriMatch;

    std::regex_search(uri, uriMatch, uriRegex);
    std::string host = uriMatch[1].str();
    int port = std::stoi(uriMatch[2].str());

    sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_addr.s_addr = inet_addr(host.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    auto res = ::connect(_fd, (struct sockaddr*)&server , sizeof(server));
    if (res == -1 && errno != EINPROGRESS)
    {
        std::cerr << "failed to connect to " << uri << std::endl;
        return;
    }
}

void
EmscriptenWebSocketImpl::disconnect()
{
    _connected = false;

    pushCallback([=](std::weak_ptr<WebSocket> s) { disconnected()->execute(s); });
}

void
EmscriptenWebSocketImpl::sendMessage(const void* payload, size_t s)
{
    write(_fd, payload, s);
}

void
EmscriptenWebSocketImpl::openCallback(int fd, void* userData)
{
    auto impl = static_cast<EmscriptenWebSocketImpl*>(userData);

    impl->_connected = true;
    impl->pushCallback([=](std::weak_ptr<WebSocket> s) { impl->connected()->execute(s); });
}

void
EmscriptenWebSocketImpl::listenCallback(int fd, void* userData)
{
}

void
EmscriptenWebSocketImpl::connectionCallback(int fd, void* userData)
{
}

void
EmscriptenWebSocketImpl::messageCallback(int fd, void* userData)
{
    std::array<char, 1024> buffer;
    std::vector<uint8_t> payload;
    int len = 0;

    do
    {
        len = recv(fd, &buffer[0], sizeof(buffer), MSG_DONTWAIT);
        if (len > 0)
            std::copy(std::begin(buffer), std::begin(buffer) + len, std::back_inserter(payload));
    }
    while (len > 0);

    auto impl = static_cast<EmscriptenWebSocketImpl*>(userData);

    impl->pushCallback([=](std::weak_ptr<WebSocket> s) { impl->messageReceived()->execute(s, payload); });
}

void
EmscriptenWebSocketImpl::closeCallback(int fd, void* userData)
{
    EmscriptenWebSocketImpl* impl = static_cast<EmscriptenWebSocketImpl*>(userData);

    impl->disconnect();
}

void
EmscriptenWebSocketImpl::poll(std::weak_ptr<WebSocket> webSocket)
{
    for (auto& callback : _callbacks)
        callback(webSocket);
    _callbacks.clear();
}

void
EmscriptenWebSocketImpl::pushCallback(std::function<void(std::weak_ptr<WebSocket>)> callback)
{
    _callbacks.push_back(callback);
}


#endif // defined(EMSCRIPTEN)
