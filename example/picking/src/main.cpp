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

using namespace minko;
using namespace minko::component;
using namespace minko::math;

Signal<scene::Node::Ptr>::Slot pickingMouseClick;
Signal<scene::Node::Ptr>::Slot pickingMouseRightClick;
Signal<scene::Node::Ptr>::Slot pickingMouseOver;
Signal<scene::Node::Ptr>::Slot pickingMouseOut;

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Picking", 800, 600);

	auto sceneManager = SceneManager::create(canvas->context());

	// setup assets
	sceneManager->assets()->loader()->options()->resizeSmoothly(true);
	sceneManager->assets()->loader()->options()->generateMipmaps(true);
	sceneManager->assets()->loader()->options()
                ->registerParser<file::PNGParser>("png");
        sceneManager->assets()->loader()
                ->queue("effect/Basic.effect")
		->queue("effect/Picking.effect");

	sceneManager->assets()
		->texture("renderTarget",			render::Texture::create(sceneManager->assets()->context(), 512, 512, false, true));

	sceneManager->assets()
		->material("redMaterial",			material::BasicMaterial::create()->diffuseColor(0xFF0000FF))
		->material("greenMaterial",			material::BasicMaterial::create()->diffuseColor(0xF0FF00FF))
		->material("blueMaterial",			material::BasicMaterial::create()->diffuseColor(0x0000FFFF))
		->material("backgroundMaterial",	material::BasicMaterial::create()->diffuseMap(sceneManager->assets()->texture("renderTarget")))
		->geometry("cube",					geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->geometry("sphere",				geometry::SphereGeometry::create(sceneManager->assets()->context()))
		->geometry("plane",					geometry::QuadGeometry::create(sceneManager->assets()->context()));

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Transform::create(
		Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 4.f))
		))
		->addComponent(PerspectiveCamera::create(800.f / 600.f, (float)PI * 0.25f, .1f, 1000.f));

	root->addChild(camera);

	auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
		auto cube = scene::Node::create("cubeNode")
			->addComponent(Surface::create(
				sceneManager->assets()->geometry("cube"),
				sceneManager->assets()->material("redMaterial"),
				sceneManager->assets()->effect("effect/Basic.effect")))
			->addComponent(Transform::create(Matrix4x4::create()->appendTranslation(Vector3::create(-1.4f))))
			->layouts(scene::Layout::Group::DEFAULT | scene::Layout::Group::PICKING);

		auto sphere = scene::Node::create("sphereNode")
			->addComponent(Surface::create(
				sceneManager->assets()->geometry("sphere"),
				sceneManager->assets()->material("greenMaterial"),
				sceneManager->assets()->effect("effect/Basic.effect")))
			->addComponent(Transform::create())
			->layouts(scene::Layout::Group::DEFAULT | scene::Layout::Group::PICKING);

		auto teapot = scene::Node::create("planeNode")
			->addComponent(Surface::create(
				sceneManager->assets()->geometry("plane"),
				sceneManager->assets()->material("blueMaterial"),
				sceneManager->assets()->effect("effect/Basic.effect")))
			->addComponent(Transform::create(Matrix4x4::create()->appendTranslation(Vector3::create(1.4f))))
			->layouts(scene::Layout::Group::DEFAULT | scene::Layout::Group::PICKING);

		root->addChild(cube)
			->addChild(sphere)
			->addChild(teapot);

		root->addComponent(Picking::create(sceneManager, canvas, camera, false));

		pickingMouseClick = root->component<Picking>()->mouseClick()->connect([&](scene::Node::Ptr node)
		{
			std::cout << "Click : " << node->name() << std::endl;
		});

		pickingMouseRightClick = root->component<Picking>()->mouseRightClick()->connect([&](scene::Node::Ptr node)
		{
			std::cout << "Right Click : " << node->name() << std::endl;
		});

		pickingMouseOver = root->component<Picking>()->mouseOver()->connect([&](scene::Node::Ptr node)
		{
			std::cout << "In : " << node->name() << std::endl;
		});

		pickingMouseOut = root->component<Picking>()->mouseOut()->connect([&](scene::Node::Ptr node)
		{
			std::cout << "Out : " << node->name() << std::endl;
		});
	});
	camera->addComponent(Renderer::create(0x7f7f7fff));

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
	{
		sceneManager->nextFrame(time, deltaTime);
	});

	sceneManager->assets()->loader()->load();

	canvas->run();

	return 0;
}


