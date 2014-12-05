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
using namespace minko::component;
using namespace minko::math;

const std::string TEXTURE_FILENAME = "texture/box.png";

int
main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Cube", 800, 600);

    auto root = canvas->createScene();

    //auto sceneManager = SceneManager::create(canvas);

    auto sceneManager = root->component<SceneManager>();

	// setup assets
	sceneManager->assets()->loader()->options()
		->resizeSmoothly(true)
		->generateMipmaps(true)
		->registerParser<file::PNGParser>("png");

	sceneManager->assets()->loader()
		->queue(TEXTURE_FILENAME)
		->queue("effect/Basic.effect");

	sceneManager->assets()->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()));

	auto mesh = scene::Node::create("mesh")
		->addComponent(Transform::create());

    auto camerasNodeSet = scene::NodeSet::create(root)
            ->descendants(true)
            ->where([](scene::Node::Ptr n)
    {
        return n->hasComponent<PerspectiveCamera>();
    });

	auto camera = camerasNodeSet->nodes()[0];

	auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
		mesh->addComponent(Surface::create(
			sceneManager->assets()->geometry("cube"),
			material::BasicMaterial::create()->diffuseMap(
			sceneManager->assets()->texture(TEXTURE_FILENAME)
			),
			sceneManager->assets()->effect("effect/Basic.effect")
			));

		root->addChild(mesh);
	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
	{
		mesh->component<Transform>()->matrix()->appendRotationY(0.001f * deltaTime);

		sceneManager->nextFrame(time, deltaTime);
	});

	sceneManager->assets()->loader()->load();
	canvas->run();

	return 0;
}
