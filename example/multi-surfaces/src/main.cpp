/*
Copyright (c) 2013 Aerys

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
#include "minko/geometry/CubeGeometryDown.hpp"
#include "minko/geometry/CubeGeometryUp.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

Signal<input::Keyboard::Ptr>::Slot keyDown;

Surface::Ptr surface1;
Surface::Ptr surface2;
Surface::Ptr surface3;

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Multi Surface", 800, 600);

	auto sceneManager = SceneManager::create(canvas->context());
	
	// setup assets
	sceneManager->assets()->defaultOptions()->resizeSmoothly(true);
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->queue("effect/Basic.effect");
	

	std::cout << "Press [Q] to add/remove the first surface." << std::endl;
	std::cout << "Press [W] to add/remove the second surface." << std::endl;
	std::cout << "Press [E] to add/remove the third surface." << std::endl;

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x00000000))
		->addComponent(Transform::create(
		Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 5.f))
		))
		->addComponent(PerspectiveCamera::create(800.f / 600.f, (float)PI * 0.25f, .1f, 1000.f));
	root->addChild(camera);

	auto cubeGeometry = geometry::CubeGeometry::create(sceneManager->assets()->context());
	auto cubeGeometryUp = geometry::CubeGeometryUp::create(sceneManager->assets()->context());
	auto cubeGeometryDown = geometry::CubeGeometryDown::create(sceneManager->assets()->context());
	auto redMaterial = material::BasicMaterial::create()->diffuseColor(math::Vector4::create(1.f));
	auto greenMaterial = material::BasicMaterial::create()->diffuseColor(math::Vector4::create(0.f, 1.f));
	auto blueMaterial = material::BasicMaterial::create()->diffuseColor(math::Vector4::create(0.f, 0.f, 1.f));

	sceneManager->assets()->geometry("cubeGeometry", cubeGeometry)
		->geometry("cubeGeometryDown", cubeGeometryDown)
		->geometry("cubeGeometryUp", cubeGeometryUp)
		->material("redMaterial", redMaterial)
		->material("greenMaterial", greenMaterial)
		->material("blueMaterial", blueMaterial);

	auto mesh = scene::Node::create("mesh")
		->addComponent(Transform::create());

	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		surface1 = Surface::create(
			assets->geometry("cubeGeometryDown"),
			assets->material("redMaterial"),
			assets->effect("effect/Basic.effect")
			);

		surface2 = Surface::create(
			assets->geometry("cubeGeometry"),
			assets->material("blueMaterial"),
			assets->effect("effect/Basic.effect")
			);

		surface3 = Surface::create(
			assets->geometry("cubeGeometryUp"),
			assets->material("greenMaterial"),
			assets->effect("effect/Basic.effect")
			);
		
		mesh->addComponent(surface1)
			->addComponent(surface2)
			->addComponent(surface3);

		root->addChild(mesh);

		keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
		{
			if (k->keyIsDown(input::Keyboard::Q))
			{
				if (mesh->hasComponent(surface1))
					mesh->removeComponent(surface1);
				else
					mesh->addComponent(surface1);
			}
			if (k->keyIsDown(input::Keyboard::W))
			{
				if (mesh->hasComponent(surface2))
					mesh->removeComponent(surface2);
				else
					mesh->addComponent(surface2);
			}
			if (k->keyIsDown(input::Keyboard::E))
			{
				if (mesh->hasComponent(surface3))
					mesh->removeComponent(surface3);
				else
					mesh->addComponent(surface3);
			}
		});
	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, uint time, float deltaTime)
	{
		//spotLight->component<Transform>()->matrix()->appendRotationY(0.01f);
		sceneManager->nextFrame();
	});

	sceneManager->assets()->load();
	canvas->run();

	return 0;
}


