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

using namespace minko;
using namespace minko::math;
using namespace minko::component;

const uint WINDOW_WIDTH = 800;
const uint WINDOW_HEIGHT = 600;

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Tutorial - Working with point lights", WINDOW_WIDTH, WINDOW_HEIGHT);
    auto sceneManager = component::SceneManager::create(canvas->context());

    // setup assets
    sceneManager->assets()->loader()->options()->generateMipmaps(true);
	sceneManager->assets()->loader()->options()->registerParser<file::PNGParser>("png");
	sceneManager->assets()->loader()
        ->queue("effect/Sprite.effect")
        ->queue("effect/Phong.effect")
        ->queue("texture/sprite-pointlight.png");

    auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {
        auto root = scene::Node::create("root")->addComponent(sceneManager);

        auto camera = scene::Node::create("camera")
            ->addComponent(Renderer::create(0x7f7f7fff))
            ->addComponent(Transform::create(
            Matrix4x4::create()->lookAt(Vector3::create(0.f, 1.f, 0.f), Vector3::create(0.f, 1.f, -3.f))
            ))
            ->addComponent(PerspectiveCamera::create(
            (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, float(M_PI) * 0.25f, .1f, 1000.f)
            );
        root->addChild(camera);

        // create a ground
        auto ground = scene::Node::create("ground")
            ->addComponent(Surface::create(
            geometry::QuadGeometry::create(canvas->context()),
            material::BasicMaterial::create()->diffuseColor(Vector4::create(0.5f, 0.5f, 0.5f, 1.f)),
			sceneManager->assets()->effect("effect/Phong.effect")
            ))
            ->addComponent(Transform::create(Matrix4x4::create()->appendScale(4.f)->appendRotationX(-(float(M_PI) /2))));
        root->addChild(ground);

        // create a left wall
        auto leftWall = scene::Node::create("leftWall")
            ->addComponent(Surface::create(
            geometry::QuadGeometry::create(canvas->context()),
            material::BasicMaterial::create()->diffuseColor(Vector4::create(0.5f, 0.5f, 0.5f, 1.f)),
			sceneManager->assets()->effect("effect/Phong.effect")
            ))
            ->addComponent(Transform::create(Matrix4x4::create()
            ->appendScale(4.f)
            ->appendRotationY(-(float(M_PI) / 2))
            ->appendTranslation(1.f, 1.f, 0.f)));
        root->addChild(leftWall);

        // create a right wall
        auto rightWall = scene::Node::create("rightWall")
            ->addComponent(Surface::create(
            geometry::QuadGeometry::create(canvas->context()),
            material::BasicMaterial::create()->diffuseColor(Vector4::create(0.5f, 0.5f, 0.5f, 1.f)),
			sceneManager->assets()->effect("effect/Phong.effect")
            ))
            ->addComponent(Transform::create(Matrix4x4::create()
            ->appendScale(4.f)
            ->appendRotationY((float(M_PI) / 2))
            ->appendTranslation(-1.f, 1.f, 0.f)));
        root->addChild(rightWall);

        // create a back wall
        auto backWall = scene::Node::create("backWall")
            ->addComponent(Surface::create(
            geometry::QuadGeometry::create(canvas->context()),
            material::BasicMaterial::create()->diffuseColor(Vector4::create(0.5f, 0.5f, 0.5f, 1.f)),
			sceneManager->assets()->effect("effect/Phong.effect")
            ))
            ->addComponent(Transform::create(Matrix4x4::create()
            ->appendScale(4.f)
            ->appendRotationX(float(M_PI))
            ->appendTranslation(0.f, 1.f, 1.f)));
        root->addChild(backWall);

        // create the point light node
        auto pointLightNode = scene::Node::create("pointLight")
            ->addComponent(Transform::create(Matrix4x4::create()->translation(0, 1.f, 0)));

        // add a sprite to have a light representation
        pointLightNode->addComponent(Surface::create(
            geometry::QuadGeometry::create(canvas->context()),
            material::Material::create()
			->set("diffuseMap", sceneManager->assets()->texture("texture/sprite-pointlight.png"))
            ->set("diffuseTint", Vector4::create(1.f, 1.f, 1.f, 1.f)),
			sceneManager->assets()->effect("effect/Sprite.effect")
            ));

        // create the point light component
        auto pointLight = PointLight::create();

        // update the point light component attributes
        pointLight->diffuse(0.5f);

        // add the component to the point light node
        pointLightNode->addComponent(pointLight);

        // add the Node to the root of the scene graph
        root->addChild(pointLightNode);

        auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
        {
            sceneManager->nextFrame(t, dt);
        });

        canvas->run();
    });

    sceneManager->assets()->loader()->load();

    return 0;
}