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
using namespace minko::math;
using namespace minko::component;

const uint WINDOW_WIDTH = 800;
const uint WINDOW_HEIGHT = 600;

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Tutorial - Working with spot lights", WINDOW_WIDTH, WINDOW_HEIGHT);
    auto sceneManager = component::SceneManager::create(canvas->context());

    // setup assets
    sceneManager->assets()->defaultOptions()->generateMipmaps(true);
    sceneManager->assets()->queue("effect/Phong.effect");

    auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
    {
        auto root = scene::Node::create("root")->addComponent(sceneManager);

        auto camera = scene::Node::create("camera")
            ->addComponent(Renderer::create(0x7f7f7fff))
            ->addComponent(Transform::create(
            Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 3.f, -5.f))
            ))
            ->addComponent(PerspectiveCamera::create(
            (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, (float) PI * 0.25f, .1f, 1000.f)
            );
        root->addChild(camera);

        // create a ground
        auto ground = scene::Node::create("sphere")
            ->addComponent(Surface::create(
            geometry::QuadGeometry::create(assets->context()),
            material::BasicMaterial::create()->diffuseColor(Vector4::create(0.f, 0.f, 1.f, 1.f)),
            assets->effect("effect/Phong.effect")
            ))
            ->addComponent(Transform::create(Matrix4x4::create()->appendScale(3.f)->appendRotationX(-1.57f)));
        root->addChild(ground);

        // create the spot light node
        auto spotLightNode = scene::Node::create("spotLight");

        // change the spot light position
        spotLightNode->addComponent(Transform::create(Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.1f, 2.f, 0.f))));

        // create the point light component
        auto spotLight = SpotLight::create(.15f, .4f);

        // update the spot light component attributes
        spotLight->diffuse(0.5f);

        // add the component to the spot light node
        spotLightNode->addComponent(spotLight);

        // add the node to the root of the scene graph
        root->addChild(spotLightNode);

        auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, uint t, uint dt)
        {
            spotLightNode->component<Transform>()->matrix()->appendRotationX(0.005f);

            sceneManager->nextFrame();
        });

        canvas->run();
    });

    sceneManager->assets()->load();

    return 0;
}