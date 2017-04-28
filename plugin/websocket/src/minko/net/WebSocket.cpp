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

#include "minko/net/WebSocket.hpp"

#if defined(EMSCRIPTEN)
#include "minko/net/EmscriptenWebSocketImpl.hpp"
#else
#include "minko/net/NativeWebSocketImpl.hpp"
#endif // defined(EMSCRIPTEN)

using namespace minko;
using namespace minko::net;

WebSocket::WebSocket(WebSocketImpl* impl) :
#if defined(EMSCRIPTEN)
    _impl(impl ? impl : new EmscriptenWebSocketImpl()),
#else
    _impl(impl ? impl : new NativeWebSocketImpl()),
#endif
    _canvas(nullptr)
{}

WebSocket::WebSocket(AbstractCanvas::Ptr canvas, WebSocketImpl* impl) :
#if defined(EMSCRIPTEN)
    _impl(impl ? impl : new EmscriptenWebSocketImpl()),
#else
    _impl(impl ? impl : new NativeWebSocketImpl()),
#endif
    _canvas(canvas)
{}

WebSocket::~WebSocket()
{
    disconnect();
}

Signal<std::weak_ptr<WebSocket>>::Ptr
WebSocket::connected()
{
    return _impl->connected();
}

Signal<std::weak_ptr<WebSocket>>::Ptr
WebSocket::disconnected()
{
    return _impl->disconnected();
}

Signal<std::weak_ptr<WebSocket>, const std::vector<uint8_t>&>::Ptr
WebSocket::messageReceived()
{
    return _impl->messageReceived();
}

void
WebSocket::connect(const std::string& uri, const std::string& cookie)
{
    startPollingOnEnterFrame();

    _impl->connect(uri, cookie);
}

bool
WebSocket::poll()
{
    return _impl->poll(shared_from_this());
}

void
WebSocket::disconnect()
{
    stopPollingOnEnterFrame();
    _impl->disconnect();
}

void
WebSocket::sendMessage(const std::string& message)
{
    sendMessage(message.c_str(), message.size());
}

void
WebSocket::sendMessage(const std::vector<uint8_t>& payload)
{
    sendMessage(&payload[0], payload.size());
}

void
WebSocket::sendMessage(const void* payload, size_t s)
{
    _impl->sendMessage(payload, s);
}

void
WebSocket::startPollingOnEnterFrame()
{
    if (!!_canvas)
    {
        _enterFrameSlot = _canvas->enterFrame()->connect([&](AbstractCanvas::Ptr, float, float, bool)
        {
            poll();
        });
    }
}

void
WebSocket::stopPollingOnEnterFrame()
{
    _enterFrameSlot = nullptr;
}

bool
WebSocket::isConnected()
{
    return _impl->isConnected();
}
