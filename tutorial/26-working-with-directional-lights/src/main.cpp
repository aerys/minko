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
  auto canvas = Canvas::create("Minko Tutorial - Working with directional lights", WINDOW_WIDTH, WINDOW_HEIGHT);
  auto sceneManager = component::SceneManager::create(canvas->context());
 
  sceneManager->assets()
	  ->queue("effect/Phong.effect");
  
  auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
  {
    auto root = scene::Node::create("root")
      ->addComponent(sceneManager);
 
    auto camera = scene::Node::create("camera")
      ->addComponent(Renderer::create(0x7f7f7fff))
      ->addComponent(PerspectiveCamera::create(
		(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)PI * 0.25f, .1f, 1000.f));
    root->addChild(camera);
 
	auto phongMaterial = material::PhongMaterial::create();

	phongMaterial->diffuseColor(0xFF0000FF);
	phongMaterial->specularColor(0xFFFFFFFF);
	phongMaterial->shininess(16.0f);

    auto sphere = scene::Node::create("sphere")
      ->addComponent(Transform::create(Matrix4x4::create()->translation(0.f, 0.f, -5.f)))
	  ->addComponent(Surface::create(
        geometry::SphereGeometry::create(assets->context()),
		phongMaterial,
        assets->effect("effect/Phong.effect")
      ));
	sphere->component<Transform>()->matrix()->prependRotationY(PI * 0.25f);
	root->addChild(sphere);

	auto ambientLight = scene::Node::create("ambientLight")
		->addComponent(AmbientLight::create(0.25f));
	ambientLight->component<AmbientLight>()->color(Vector4::create(1.0f, 1.0f, 1.0f, 1.0f));
	root->addChild(ambientLight);

	auto directionalLight = scene::Node::create("directionalLight")
		->addComponent(DirectionalLight::create()->diffuse(0.8f)->color(0xFFFFFFFF))
		->addComponent(Transform::create(Matrix4x4::create()->lookAt(Vector3::create(), Vector3::create(3.0f, 2.0f, 3.0f))));
	root->addChild(directionalLight);

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, uint t, float dt)
	{
		sceneManager->nextFrame();
	});

    canvas->run();
  });
 
  sceneManager->assets()->load();
 
  return 0;
}