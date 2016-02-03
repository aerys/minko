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

const math::uint WINDOW_WIDTH = 800;
const math::uint WINDOW_HEIGHT = 600;

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Tutorial - Authoring uber-shaders", WINDOW_WIDTH, WINDOW_HEIGHT);
    
	auto sceneManager = component::SceneManager::create(canvas);

	sceneManager->assets()->loader()
        ->queue("effect/MyCustomUberEffect.effect")
		->queue("texture/box.png")
		->options()->registerParser<file::PNGParser>("png");

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()))
        ->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 0.f, 3.f), vec3(), vec3(0.f, 1.f, 0.f)))));
        
	root->addChild(camera);

	auto texturedCube = scene::Node::create("texturedCube");
	auto coloredCube = scene::Node::create("coloredCube");

    auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {

		auto texturedMaterial = material::Material::create();
		texturedMaterial->data()->set("diffuseMap", sceneManager->assets()->texture("texture/box.png")->sampler());

		texturedCube
			->addComponent(Transform::create(translate(vec3(-1.f, 0.f, -1.f))))
            ->addComponent(Surface::create(
				geometry::CubeGeometry::create(canvas->context()),
				texturedMaterial,
				sceneManager->assets()->effect("effect/MyCustomUberEffect.effect")));
        
		root->addChild(texturedCube);

		auto coloredMaterial = material::Material::create();
		coloredMaterial->data()->set("diffuseColor", vec4(0.f, 0.f, 1.f, 1.f));

        coloredCube
			->addComponent(Transform::create(translate(vec3(1.f, 0.f, -1.f))))
            ->addComponent(Surface::create(
				geometry::CubeGeometry::create(canvas->context()),
				coloredMaterial,
				sceneManager->assets()->effect("effect/MyCustomUberEffect.effect")
            ));

        root->addChild(coloredCube);

    });

    sceneManager->assets()->loader()->load();

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
	{
		auto texturedTransform = texturedCube->component<Transform>();
		texturedTransform->matrix(texturedTransform->matrix() * rotate(.01f, vec3(0.f, 1.f, 0.f)));

		auto coloredTransform = coloredCube->component<Transform>();
		coloredTransform->matrix(coloredTransform->matrix() * rotate(-.01f, vec3(0.f, 1.f, 0.f)));

		sceneManager->nextFrame(t, dt);
	});

	canvas->run();

    return 0;
}
