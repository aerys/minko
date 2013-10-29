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

#include <minko/Minko.hpp>
#include <minko/MinkoSDL.hpp>

#include "minko/geometry/StarGeometry.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::geometry;
using namespace minko::component;
using namespace minko::render;

int
main(int argc, char** argv)
{
	MinkoSDL::initialize("Minko Example - Stencil", 800, 600);

	auto sceneManager = SceneManager::create(OpenGLES2Context::create());
	
	// setup assets
	sceneManager->assets()
		->geometry("star", StarGeometry::create(sceneManager->assets()->context(), 5, 0.5f, 0.25f))
		->queue("effect/Basic.effect");

	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		auto root = Node::create("root")
			->addComponent(sceneManager);

		auto canera	= Node::create("camera")
			->addComponent(Renderer::create(0x7f7f7fff))
			->addComponent(PerspectiveCamera::create(PI * 0.25f, ));

	});

	sceneManager->assets()->load();

	return 0;
}
