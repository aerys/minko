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
#include "minko/MinkoNodeJS.hpp"
#include "minko/MinkoHtmlOverlay.hpp"

#include "json/json.h"

using namespace minko;
using namespace minko::async;
using namespace minko::component;
using namespace minko::dom;
using namespace minko::file;
using namespace minko::net;

using namespace std;

async::Worker::Ptr worker;
Signal<Loader::Ptr>::Slot requestCompleteSlot;
Signal<Loader::Ptr, const Error&>::Slot requestErrorSlot;

const std::string EFFECT_FILENAME = "effect/Basic.effect";

void
callEndpoint(const string& url)
{
    auto loader = Loader::create();
    auto options = HTTPOptions::create();

    options
        ->verifyPeer(false)
        ->loadAsynchronously(false)
        ->parserFunction([](const string& extension){ return nullptr; })
        ->storeDataIfNotParsed(false)
        ->registerProtocol<net::HTTPProtocol>("http")
        ->registerProtocol<net::HTTPProtocol>("https");

    loader->options(options);

    requestErrorSlot = loader->error()->connect(
        [](Loader::Ptr loaderThis, const Error& error) -> void
    {
        LOG_ERROR(error.type() << ": " << error.what());
    });

    requestCompleteSlot = loader->complete()->connect(
        [url](Loader::Ptr loaderThis) -> void
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
                if (root["success"].asBool())
                    LOG_INFO("Call from C++ to " << url << ": success");
            }
        }
    });

    loader->queue(url)->load();
}

int
main(int argc, char** argv)
{
    auto overlay = HtmlOverlay::create(argc, argv);

    auto canvas = Canvas::create("Minko Example - NodeJS");
    canvas->registerWorker<NodeJSWorker>("node");
    canvas->registerWorker<HTTPWorker>("http");

    std::string extractDir = AndroidUnzip::extractFromAsset("server.zip");

    worker = AbstractCanvas::defaultCanvas()->getWorker("node");
    worker->start(vector<char>(extractDir.begin(), extractDir.end()));

    std::thread([]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        callEndpoint("http://127.0.0.1:3000/hello");
    }).detach();

    auto sceneManager = SceneManager::create(canvas);

    sceneManager->assets()->loader()
        ->queue(EFFECT_FILENAME);

    auto cubeGeometry = geometry::CubeGeometry::create(sceneManager->assets()->context());
    sceneManager->assets()->geometry("cubeGeometry", cubeGeometry);

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager)
        ->addComponent(overlay);

    auto mesh = scene::Node::create("mesh")
        ->addComponent(Transform::create());

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(
            math::inverse(
                math::lookAt(
                    math::vec3(0.f, 0.f, 3.f), math::vec3(), math::vec3(0, 1, 0)
                )
            )
        ))
        ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

    root->addChild(mesh);
    root->addChild(camera);

    auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        auto material = material::BasicMaterial::create();
        material->diffuseColor(0xff0000ff);

        mesh->addComponent(Surface::create(
            sceneManager->assets()->geometry("cubeGeometry"),
            material,
            sceneManager->assets()->effect(EFFECT_FILENAME)
        ));
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
    });

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime)
    {
        mesh->component<Transform>()->matrix(
            mesh->component<Transform>()->matrix() * math::rotate(0.01f, math::vec3(0, 1, 0))
        );

        sceneManager->nextFrame(time, deltaTime);
    });

    overlay->load("client.html");

    sceneManager->assets()->loader()->load();
    canvas->run();

    return 0;
}
