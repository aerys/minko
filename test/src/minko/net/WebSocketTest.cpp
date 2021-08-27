/*
Copyright(c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "WebSocketTest.hpp"

#include "minko/net/WebSocket.hpp"

using namespace minko;
using namespace minko::net;

TEST_F(WebSocketTest, Create)
{
    auto ws = std::make_shared<WebSocket>();
}

TEST_F(WebSocketTest, Connect)
{
    auto ws = std::make_shared<WebSocket>();
    bool connected = false;

    auto _ = ws->connected()->connect([&](WebSocket::WeakPtr s)
    {
        connected = true;
    });

    ws->connect("ws://localhost:8765");

    std::this_thread::sleep_for(std::chrono::seconds(3));
    ws->poll();

    ASSERT_TRUE(connected);
}

TEST_F(WebSocketTest, TLSConnect)
{
    auto ws = std::make_shared<WebSocket>();
    bool connected = false;

    auto _ = ws->connected()->connect([&](WebSocket::WeakPtr s)
    {
        connected = true;
    });

    ws->connect("wss://localhost:8766");

    std::this_thread::sleep_for(std::chrono::seconds(3));
    ws->poll();

    ASSERT_TRUE(connected);
}

TEST_F(WebSocketTest, Disconnect)
{
    auto ws = std::make_shared<WebSocket>();
    bool connected = false;
    bool disconnected = false;

    auto _ = ws->connected()->connect([&](WebSocket::WeakPtr s)
    {
        connected = true;
    });
    auto __ = ws->disconnected()->connect([&](WebSocket::WeakPtr s)
    {
        disconnected = true;
    });

    ws->connect("ws://localhost:8765");

    std::this_thread::sleep_for(std::chrono::seconds(3));
    ws->poll();

    ws->disconnect();

    std::this_thread::sleep_for(std::chrono::seconds(3));
    ws->poll();

    ASSERT_TRUE(connected);
    ASSERT_TRUE(disconnected);
}

TEST_F(WebSocketTest, SendMessage)
{
    auto ws = std::make_shared<WebSocket>();
    std::string sent = "hello world!";
    std::string received;

    auto _ = ws->connected()->connect([&](WebSocket::WeakPtr s)
    {
        ws->sendMessage(sent);
    });
    auto __ = ws->messageReceived()->connect([&](WebSocket::WeakPtr, const std::vector<uint8_t>& msg)
    {
        received = std::string((char*)&msg[0], msg.size());
    });

    ws->connect("ws://localhost:8765");

    std::this_thread::sleep_for(std::chrono::seconds(3));
    ws->poll();

    std::this_thread::sleep_for(std::chrono::seconds(3));
    ws->poll();

    ASSERT_EQ(received, sent);
}

TEST_F(WebSocketTest, TLSSendMessage)
{
    auto ws = std::make_shared<WebSocket>();
    std::string sent = "hello world!";
    std::string received;
    auto _ = ws->connected()->connect([&](WebSocket::WeakPtr s)
    {
        ws->sendMessage(sent);
    });
    auto __ = ws->messageReceived()->connect([&](WebSocket::WeakPtr, const std::vector<uint8_t>& msg)
    {
        received = std::string((char*)&msg[0], msg.size());
    });

    ws->connect("wss://localhost:8766");

    std::this_thread::sleep_for(std::chrono::seconds(3));
    ws->poll();

    std::this_thread::sleep_for(std::chrono::seconds(3));
    ws->poll();

    ASSERT_EQ(received, sent);
}
