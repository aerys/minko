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
#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const uint			WINDOW_WIDTH	= 800;
const uint			WINDOW_HEIGHT	= 600;

int 
main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - SDL binding", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = SceneManager::create(canvas->context());

	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
    sceneManager->assets()->load("effect/Basic.effect");

	sceneManager->assets()->defaultOptions()
		->skinningFramerate(60)
		->skinningMethod(SkinningMethod::HARDWARE)
		->startAnimation(true)
		->effect(sceneManager->assets()->effect("basic"));

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
			Matrix4x4::create()->lookAt(Vector3::create(0.f, 0.75f, 0.f), Vector3::create(0.25f, 0.75f, 2.5f))
		))
		->addComponent(PerspectiveCamera::create(
			(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)PI * 0.25f, .1f, 1000.f)
		);
	root->addChild(camera);

	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{

	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
	});

	// currently, keyUp events seem not to be fired at the individual key level
	auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
    {
		if (k->keyIsDown(input::Keyboard::ScanCode::UP))
		{
            std::cout << "[ScanCode]Key UP down !" << std::endl;
        }
		if (k->keyIsDown(input::Keyboard::ScanCode::DOWN))
		{
            std::cout << "[ScanCode]Key DOWN down !" << std::endl;
		}
		if (k->keyIsDown(input::Keyboard::ScanCode::LEFT))
		{
            std::cout << "[ScanCode]Key LEFT down !" << std::endl;
		}
		if (k->keyIsDown(input::Keyboard::ScanCode::RIGHT))
		{
            std::cout << "[ScanCode]Key RIGHT down !" << std::endl;
        }
		if (k->keyIsDown(input::Keyboard::ScanCode::SPACE))
		{
            std::cout << "[ScanCode]Key SPACE down !" << std::endl;
        }
		if (k->keyIsDown(input::Keyboard::ScanCode::END))
		{
            std::cout << "[ScanCode]Key END down !" << std::endl;
        }
		if (k->keyIsDown(input::Keyboard::ScanCode::_1))
		{
            std::cout << "[ScanCode]Key 1 down !" << std::endl;
		}
		if (k->keyIsDown(input::Keyboard::ScanCode::R))
		{
            std::cout << "[ScanCode]Key R down !" << std::endl;
		}
        if (k->keyIsDown(input::Keyboard::ScanCode::COMMA))
        {
            std::cout << "[ScanCode]Key COMMA down !" << std::endl;
        }
        if (k->keyIsDown(input::Keyboard::ScanCode::SEMICOLON))
        {
            std::cout << "[ScanCode]Key SEMICOLON down !" << std::endl;
        }
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, uint time, float deltaTime)
	{
		sceneManager->nextFrame();
	});

	sceneManager->assets()->load();
	canvas->run();

	return 0;
}