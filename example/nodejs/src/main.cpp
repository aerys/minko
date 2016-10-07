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

#include "minko/Minko.hpp"
#include "minko/MinkoHTTP.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoNodeJSWorker.hpp"

#include "json/json.h"

using namespace minko;
using namespace minko::async;
using namespace minko::component;
using namespace minko::file;
using namespace minko::net;

using namespace std;

async::Worker::Ptr worker;
Signal<Loader::Ptr>::Slot requestCompleteSlot;
Signal<Loader::Ptr, const Error&>::Slot requestErrorSlot;

void
callEndpoint(const string& url)
{
    auto loader = Loader::create();
    auto options = HTTPOptions::create();

    options
        ->verifyPeer(false)
        ->loadAsynchronously(true)
        ->parserFunction([](const string& extension){ return nullptr; })
        ->storeDataIfNotParsed(false);

    loader->options(options);

    requestErrorSlot = loader->error()->connect(
        [](Loader::Ptr loaderThis, const Error& error) -> void
    {
        LOG_WARNING(error.type() << ": " << error.what());
    });

    requestCompleteSlot = loader->complete()->connect(
        [](Loader::Ptr loaderThis) -> void
    {
        auto& files = loaderThis->files();

        for (auto& p : files)
        {
            auto endpoint = p.first;
            auto result = p.second;
            auto& data = result->data();

            auto reader = Json::Reader();
            auto root = Json::Value();

            auto parsingError = false;

            if (reader.parse(reinterpret_cast<const char*>(data.data()),
                              reinterpret_cast<const char*>(data.data()) + data.size(),
                              root))
            {
                cout << "success: " << root["success"].asBool() << endl;
            }
        }
    });

    loader->queue(url)->load();
}

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("My Minko App", 960, 540);
    canvas->registerWorker<NodeJSWorker>("node");

    string path = "index.js";
    worker = AbstractCanvas::defaultCanvas()->getWorker("node");
    worker->start(vector<char>(path.begin(), path.end()));

    auto workerMessageSlot = worker->message()->connect([](Worker::Ptr, Worker::Message m) {
        if (m.type == "ready")
            callEndpoint("http://localhost:3000/hello");
    });

    canvas->run();

    return 0;
}
