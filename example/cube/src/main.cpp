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

const std::string TEXTURE_FILENAME = "texture/box.png";

int
main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Cube", 800, 600);

    auto sceneManager = SceneManager::create(canvas);

	// setup assets
	sceneManager->assets()->loader()->options()
		->resizeSmoothly(true)
		->generateMipmaps(true)
		->registerParser<file::PNGParser>("png");

    sceneManager->assets()->loader()
        ->queue(TEXTURE_FILENAME)
		->queue("effect/Basic.effect");

	sceneManager->assets()->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()));
    sceneManager->assets()->material("material", material::BasicMaterial::create());

	auto mesh = scene::Node::create("mesh");
		//->addComponent(Transform::create());

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create())
		->addComponent(Transform::create(
			math::inverse(math::lookAt(math::vec3(0.f, 0.f, 150.f), math::vec3(0.f), math::vec3(0.f, 1.f, 0.f)))
		))
		->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

	root->addChild(camera);

    auto meshes = scene::Node::create();
    root->addChild(meshes);

	auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
        auto numFrames = 0;
        auto t = 0;
        auto p = 0;

        auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
        {
            camera->component<Transform>()->matrix(
                math::rotate(0.01f, math::vec3(0.f, 1.f, 0.f))
                * camera->component<Transform>()->matrix()
            );

            if (canvas->framerate() > 30.f)
                t++;
            else
                p++;

            if (t > 10)
            {
                t = 0;
                p = 0;
                for (auto i = 0; i < 100; ++i)
                    createRandomCube(
                        meshes,
                        sceneManager->assets()->geometry("cube"),
			material::BasicMaterial::create()->diffuseMap(
			sceneManager->assets()->texture(TEXTURE_FILENAME)
			),
                        sceneManager->assets()->effect("effect/Basic.effect")
                    );
            }

            if (p > 10 && meshes->children().size() > 0)
            {
                t = 0;
                p = 0;
                meshes->removeChild(meshes->children().back());
            }
            
            if (++numFrames % 100 == 0)
                std::cout << "num meshes = " << meshes->children().size()
                    << ", num draw calls = " << camera->component<Renderer>()->numDrawCalls()
                    << ", framerate = " << canvas->framerate() << std::endl;

            sceneManager->nextFrame(time, deltaTime);
        });

    	canvas->run();
	});

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
	});

	sceneManager->assets()->loader()->load();

	return 0;
}
