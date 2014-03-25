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
#include "minko/MinkoOverlay.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const std::string TEXTURE_FILENAME = "texture/box.png";

Signal<minko::dom::AbstractDOM::Ptr, std::string>::Slot onloadSlot;

Signal<minko::dom::AbstractDOMEvent::Ptr>::Slot onclickSlot;

dom::AbstractDOM::Ptr gameInterfaceDom;
dom::AbstractDOMElement::Ptr redScoreElement;
dom::AbstractDOMElement::Ptr blueScoreElement;

int redScore;
int blueScore;

void
updateRedScore();

void
updateBlueScore();

int main(int argc, char** argv)
{
	auto overlay = Overlay::create(argc, argv);

	redScore = 0;
	blueScore = 0;

	auto canvas = Canvas::create("Minko Example - Cube", 800, 600);

	overlay->initialize(canvas);

	auto sceneManager = SceneManager::create(canvas->context());
	
	// setup assets
	sceneManager->assets()->defaultOptions()->resizeSmoothly(true);
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->queue(TEXTURE_FILENAME)
		->queue("effect/Basic.effect")
		->queue("effect/Overlay.effect");

	sceneManager->assets()->context()->errorsEnabled(true);

	auto cubeGeometry = geometry::CubeGeometry::create(sceneManager->assets()->context());
	sceneManager->assets()->geometry("cubeGeometry", cubeGeometry);

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto mesh = scene::Node::create("mesh")
		->addComponent(Transform::create());
	root->addChild(mesh);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
		Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f))
		))
		->addComponent(PerspectiveCamera::create(800.f / 600.f, (float)PI * 0.25f, .1f, 1000.f));
	root->addChild(camera);

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
	});

	auto material = material::BasicMaterial::create()->diffuseColor(0xCCCCCCFF);

	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		root->addComponent(overlay);

		mesh->addComponent(Surface::create(
			assets->geometry("cubeGeometry"),
			material,
			assets->effect("effect/Basic.effect")
			));

		overlay->load("html/gameInterface.html");
	});

	onloadSlot = overlay->onload()->connect([=](minko::dom::AbstractDOM::Ptr dom, std::string page)
	{
		std::cout << "OnLoad: " << dom->fileName() << std::endl;

		if (!dom->isMain())
			return;

		if (dom->fileName() == "gameInterface.html")
		{
			std::cout << "before on click" << std::endl;
			onclickSlot = dom->document()->onclick()->connect([=](dom::AbstractDOMEvent::Ptr event)
			{
				dom->sendMessage("trololo");
			});
			std::cout << "after on click" << std::endl;
			gameInterfaceDom = dom;
			redScoreElement = gameInterfaceDom->getElementById("teamScoreRed");
			blueScoreElement = gameInterfaceDom->getElementById("teamScoreBlue");
		}
	});

	auto rightButtonDown = canvas->mouse()->rightButtonDown()->connect([&](input::Mouse::Ptr m)
	{
		updateRedScore();
	});


	auto leftButtonDown = canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr m)
	{
		updateBlueScore();
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, uint time, uint deltaTime)
	{
		mesh->component<Transform>()->matrix()->appendRotationY(.01f);

		sceneManager->nextFrame(time, deltaTime);
		//sceneManager->nextFrame();
	});

	sceneManager->assets()->load();
	canvas->run();

	overlay->clear();
	return 0;
}

void
updateRedScore()
{
	if (gameInterfaceDom != nullptr)
		redScoreElement->textContent(std::to_string(redScore++));
}

void
updateBlueScore()
{
	if (gameInterfaceDom != nullptr)
		blueScoreElement->textContent(std::to_string(blueScore++));
}