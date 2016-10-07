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
#include <netdb.h>

using namespace minko::net;

std::list<std::pair<int, EmscriptenWebSocketImpl*>> EmscriptenWebSocketImpl::SocketCallbackBroker::_sockets;
bool EmscriptenWebSocketImpl::SocketCallbackBroker::_initialized = false;

EmscriptenWebSocketImpl::EmscriptenWebSocketImpl()
    : WebSocketImpl()
{
    _connected = false;
}

EmscriptenWebSocketImpl::~EmscriptenWebSocketImpl()
{
    disconnect();
}

int
EmscriptenWebSocketImpl::hostnameToIp(const char* hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ((he = gethostbyname( hostname ) ) == NULL)
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }

    return 1;
}

void
EmscriptenWebSocketImpl::connect(const std::string& uri)
{
    _connected = false;

    // From emscripten.h:
    // As well as being configurable at compile time via the "-s" option the WEBSOCKET_URL and WEBSOCKET_SUBPROTOCOL
    // settings may configured at run time via the Module object e.g.
    // Module['websocket'] = {subprotocol: 'base64, binary, text'};
    // Module['websocket'] = {url: 'wss://', subprotocol: 'base64'};
    // Run time configuration may be useful as it lets an application select multiple different services.
    emscripten_run_script(std::string("Module['websocket']['url'] = '" + uri + "'").c_str());

    _fd = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (_fd == -1)
    {
        std::cerr << "failed to create client socket" << std::endl;
        return;
    }

    fcntl(_fd, F_SETFL, O_NONBLOCK);

    SocketCallbackBroker::registerSocket(_fd, this);

    std::regex uriRegex("^(ws|wss)://(.*)(/.*)$");
    std::smatch uriMatch;

    if (!std::regex_search(uri, uriMatch, uriRegex))
        return;

    std::string protocol = uriMatch[1].str();
    std::string host = uriMatch[2].str();

    int port = protocol == "wss" ? 443 : 80;

    std::regex portRegex("^(.*):([0-9]+)$");
    std::smatch portMatch;

    if (std::regex_search(host, portMatch, portRegex))
    {
        host = portMatch[1];
        port = std::stoi(portMatch[2]);
    }

    std::regex ipRegex("^(?:[0-9]{1,3}\\.){3}[0-9]{1,3}$");

    if (!std::regex_match(host, ipRegex))
    {
        char ip[32];
        if (hostnameToIp(host.c_str(), ip) != 0)
        {
            std::cerr << "unable to resolve hostname \"" << host << "\"" << std::endl;
            return;
        }
        host = ip;
    }

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
    if (!_connected)
        return;

    close(_fd);
    SocketCallbackBroker::unregisterSocket(_fd, this);

    _fd = -1;
    _connected = false;

    pushCallback([=](std::weak_ptr<WebSocket> s) { disconnected()->execute(s); });
}

void
EmscriptenWebSocketImpl::sendMessage(const void* payload, size_t s)
{
    write(_fd, payload, s);
}

void
EmscriptenWebSocketImpl::openCallback(int fd)
{
    _connected = true;
    pushCallback([=](std::weak_ptr<WebSocket> s) { connected()->execute(s); });
}

void
EmscriptenWebSocketImpl::listenCallback(int fd)
{
}

void
EmscriptenWebSocketImpl::connectionCallback(int fd)
{
}

void
EmscriptenWebSocketImpl::messageCallback(int fd)
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

    pushCallback([=](std::weak_ptr<WebSocket> s) { messageReceived()->execute(s, payload); });
}

void
EmscriptenWebSocketImpl::closeCallback(int fd)
{
    disconnect();
}

bool
EmscriptenWebSocketImpl::poll(std::weak_ptr<WebSocket> webSocket)
{
    bool read = !_callbacks.empty();

    for (auto& callback : _callbacks)
        callback(webSocket);
    _callbacks.clear();

    return read;
}

void
EmscriptenWebSocketImpl::pushCallback(std::function<void(std::weak_ptr<WebSocket>)> callback)
{
    _callbacks.push_back(callback);
}

void
EmscriptenWebSocketImpl::SocketCallbackBroker::initializeHandlers()
{
    if (_initialized)
        return;

    emscripten_set_socket_open_callback(0, SocketCallbackBroker::openCallback);
    emscripten_set_socket_listen_callback(0, SocketCallbackBroker::listenCallback);
    emscripten_set_socket_connection_callback(0, SocketCallbackBroker::connectionCallback);
    emscripten_set_socket_message_callback(0, SocketCallbackBroker::messageCallback);
    emscripten_set_socket_close_callback(0, SocketCallbackBroker::closeCallback);

    _initialized = true;
}

void
EmscriptenWebSocketImpl::SocketCallbackBroker::registerSocket(int fd, EmscriptenWebSocketImpl* socket)
{
    initializeHandlers();
    _sockets.push_back(std::pair<int, EmscriptenWebSocketImpl*>(fd, socket));
}

void
EmscriptenWebSocketImpl::SocketCallbackBroker::unregisterSocket(int fd, EmscriptenWebSocketImpl* socket)
{
    auto it = std::find_if(
        _sockets.begin(),
        _sockets.end(),
        [=](std::pair<int, EmscriptenWebSocketImpl*> p)
        {
            return p.first == fd && p.second == socket;
        }
    );

    if (it != _sockets.end())
        _sockets.erase(it);
}

EmscriptenWebSocketImpl*
EmscriptenWebSocketImpl::SocketCallbackBroker::getSocketByFd(int fd)
{
    for (auto& fdToSocket : _sockets)
        if (fdToSocket.first == fd)
                return fdToSocket.second;

    return nullptr;
}

void
EmscriptenWebSocketImpl::SocketCallbackBroker::openCallback(int fd, void* userData)
{
    auto socket = getSocketByFd(fd);

    if (socket)
        socket->openCallback(fd);
}

void
EmscriptenWebSocketImpl::SocketCallbackBroker::listenCallback(int fd, void* userData)
{
    auto socket = getSocketByFd(fd);

    if (socket)
        socket->listenCallback(fd);
}

void
EmscriptenWebSocketImpl::SocketCallbackBroker::connectionCallback(int fd, void* userData)
{
    auto socket = getSocketByFd(fd);

    if (socket)
        socket->connectionCallback(fd);
}

void
EmscriptenWebSocketImpl::SocketCallbackBroker::messageCallback(int fd, void* userData)
{
    auto socket = getSocketByFd(fd);

    if (socket)
        socket->messageCallback(fd);
}

void
EmscriptenWebSocketImpl::SocketCallbackBroker::closeCallback(int fd, void* userData)
{
    auto socket = getSocketByFd(fd);

    if (socket)
        socket->closeCallback(fd);
}

#endif // defined(EMSCRIPTEN)
