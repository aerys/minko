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
#include "minko/MinkoSDL.hpp"
// STEP 0
#include "minko/MinkoBullet.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;

const uint WINDOW_WIDTH = 800;
const uint WINDOW_HEIGHT = 600;
const std::string TEXTURE_FILENAME = "texture/box.png";

int main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Tutorial - Hello falling cube!", WINDOW_WIDTH, WINDOW_HEIGHT);
    auto sceneManager = SceneManager::create(canvas->context());

	sceneManager->assets()->loader()
        ->queue(TEXTURE_FILENAME)
		->queue("effect/Basic.effect")
		->options()->registerParser<file::PNGParser>("png");

    auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {
        auto camera = scene::Node::create("camera")
            ->addComponent(Renderer::create(0x7f7f7fff))
            ->addComponent(Transform::create(
            Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.5f, 2.0f, 2.0f))
            ))
            ->addComponent(PerspectiveCamera::create(WINDOW_WIDTH / WINDOW_HEIGHT, float(M_PI) * 0.25f, .1f, 1000.f));

        // STEP 1: create and add your physics world to the scene
        auto root = scene::Node::create("root")
            ->addComponent(sceneManager)
            ->addComponent(bullet::PhysicsWorld::create());

        // STEP 2: create a box-shaped rigid body
        auto boxColliderData = bullet::ColliderData::create(
            5.0f, // strictly positive mass
            bullet::BoxShape::create(0.5f, 0.5f, 0.5f) // shape strictly matches the CubeGeometry
            );
        auto boxNode = scene::Node::create("boxNode")
            ->addComponent(bullet::Collider::create(boxColliderData))
            ->addComponent(Surface::create(
            geometry::CubeGeometry::create(canvas->context()),
			material::BasicMaterial::create()->diffuseMap(sceneManager->assets()->texture(TEXTURE_FILENAME)),
			sceneManager->assets()->effect("effect/Basic.effect")
            ));

        root->addChild(camera);
        // STEP 3: trigger the simulation by adding the physics object to the scene
        root->addChild(boxNode);

        auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
        {
            sceneManager->nextFrame(t, dt);
        });

        canvas->run();
    });

    sceneManager->assets()->loader()->load();

    return 0;
}

