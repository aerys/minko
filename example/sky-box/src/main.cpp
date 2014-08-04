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
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;
using namespace minko::scene;

const std::string      CUBE_TEXTURE    = "texture/cubemap_sea.jpg";
const unsigned int     NUM_OBJECTS     = 15;

Node::Ptr
createTransparentObject(float, float rotationY, file::AssetLibrary::Ptr);

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Skybox");
    auto sceneManager = SceneManager::create(canvas->context());

    // setup assets
    auto loader = sceneManager->assets()->loader();

    loader->options()
        ->resizeSmoothly(true);
        ->generateMipmaps(true);
        ->registerParser<file::PNGParser>("png")
        ->registerParser<file::JPEGParser>("jpg");

    loader
        ->queue(CUBE_TEXTURE, file::Options::create(loader->options())->isCubeTexture(true))
        ->queue("effect/Basic.effect");

    sceneManager->assets()
        ->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
        ->geometry("sphere", geometry::SphereGeometry::create(sceneManager->assets()->context(), 16, 16));

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(
            Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f))
        ))
        ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

    auto sky = scene::Node::create("sky")
        ->addComponent(Transform::create(
            Matrix4x4::create()->appendScale(100.0f, 100.0f, 100.0f)
        ));

    auto objects = scene::Node::create("objects")
        ->addComponent(Transform::create(
            Matrix4x4::create()->appendRotationX(0.2f)
        ));

    auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        sky->addComponent(Surface::create(
                sceneManager->assets()->geometry("cube"),
                material::BasicMaterial::create()
                    ->diffuseCubeMap(sceneManager->assets()->cubeTexture(CUBE_TEXTURE))
                    ->triangleCulling(render::TriangleCulling::FRONT),
                sceneManager->assets()->effect("effect/Basic.effect")
            ));

        assert(NUM_OBJECTS > 0);

        const float scale = 1.25f * float(M_PI) / float(NUM_OBJECTS);
        const float dAngle = 2.0f * float(M_PI) / float(NUM_OBJECTS);

        for (unsigned int objId = 0; objId < NUM_OBJECTS; ++objId)
                  objects->addChild(createTransparentObject(scale, objId * dAngle, sceneManager->assets()));

        root
            ->addChild(camera)
            ->addChild(sky)
            ->addChild(objects);
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
    });

    auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
    {
        sky->component<Transform>()->matrix()->appendRotationY(.001f);
        objects->component<Transform>()->matrix()->prependRotationY(-.02f);

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

    auto        randomAxis = Vector3::create(rand(), rand(), rand())->normalize();
    const float randomAng  = 2.0f * float(M_PI) * rand() / float(RAND_MAX);

    return scene::Node::create()
        ->addComponent(Transform::create(
            Matrix4x4::create()
                ->appendRotation(randomAng, randomAxis)
                ->appendScale(scale)
                ->appendTranslation(1.0f)
                ->appendRotationY(rotationY)
        ))
        ->addComponent(Surface::create(
                        assets->geometry("cube"),
            material::BasicMaterial::create()
                ->diffuseColor(Color::hslaToRgba(0.5f * rotationY / float(M_PI), 1.0f, 0.5f, 0.5f))
                ->isTransparent(true, true)
                ->triangleCulling(render::TriangleCulling::BACK),
            assets->effect("effect/Basic.effect")
        ));
}
