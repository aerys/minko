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
#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::scene;

const unsigned int NUM_OBJECTS = 4;

Node::Ptr
createTransparentObject(float, float rotationY, file::AssetLibrary::Ptr);

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Blending");
    auto sceneManager = SceneManager::create(canvas);

    // setup assets
    auto loader = sceneManager->assets()->loader();

    loader
        ->queue("effect/Basic.effect");

    sceneManager->assets()
        ->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()));

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(
            math::inverse(
                math::lookAt(
                    math::vec3(0.f, 0.f, 3.f), 
                    math::vec3(), 
                    math::vec3(0, 1, 0)
                )
            )
        ))
        ->addComponent(Camera::create(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f)));

    auto objects = scene::Node::create("objects")
        ->addComponent(Transform::create(
            math::rotate(0.2f, math::vec3(1, 0, 0)) * math::mat4())
        );

    auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        assert(NUM_OBJECTS > 0);

        const float scale = 1.25f * float(M_PI) / float(NUM_OBJECTS);
        const float dAngle = 2.0f * float(M_PI) / float(NUM_OBJECTS);

        for (unsigned int objId = 0; objId < NUM_OBJECTS; ++objId)
            objects->addChild(createTransparentObject(scale, objId * dAngle, sceneManager->assets()));

        root->addChild(camera);
        root->addChild(objects);
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<Camera>()->projectionMatrix(math::perspective(.785f, float(w) / float(h), 0.1f, 1000.f));
    });

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime)
    {
        objects->component<Transform>()->matrix(objects->component<Transform>()->matrix() * math::rotate(-.02f, math::vec3(0, 1, 0)));

        sceneManager->nextFrame(time, deltaTime);
    });

    loader->load();
    canvas->run();
}

Node::Ptr
createTransparentObject(float scale, float rotationY, file::AssetLibrary::Ptr assets)
{
    assert(assets);
    assert(NUM_OBJECTS > 0);

    auto randomAxis = math::normalize(math::vec3(rand(), rand(), rand()));
    const float randomAng = 2.0f * float(M_PI) * rand() / float(RAND_MAX);

    auto matrix = math::mat4();
    matrix = math::rotate(randomAng, randomAxis) * matrix;
    matrix = math::scale(math::vec3(scale)) * matrix;
    matrix = math::translate(math::vec3(1.0f, 0, 0)) * matrix;
    matrix = math::rotate(rotationY, math::vec3(0, 1, 0)) * matrix;

    auto material = material::BasicMaterial::create();
    material->diffuseColor(math::vec4(math::rgbColor(math::vec3((rotationY / (2.f * (float)M_PI)) * 360, 1.0f, 0.5f)), 0.5f));
    material->blendingMode(render::Blending::Mode::ALPHA);
    material->zSorted(true);
    material->priority(render::Priority::TRANSPARENT);
    material->triangleCulling(render::TriangleCulling::BACK);

    return scene::Node::create()
        ->addComponent(Transform::create(matrix))
        ->addComponent(Surface::create(
                assets->geometry("cube"),
                material,
                assets->effect("effect/Basic.effect")
            )
        );
}