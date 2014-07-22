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
#include "minko/MinkoHtmlOverlay.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

Signal<minko::dom::AbstractDOM::Ptr, std::string>::Slot onloadSlot;

Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Slot onclickSlot;

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
	auto overlay = HtmlOverlay::create(argc, argv);

	redScore = 0;
	blueScore = 0;

	auto canvas = Canvas::create("Minko Example - Overlay", 800, 600);

	auto sceneManager = SceneManager::create(canvas->context());

	overlay->initialize(canvas, sceneManager);

	// setup assets
	sceneManager->assets()->loader()->options()
		->resizeSmoothly(true)
		->generateMipmaps(true);

	sceneManager->assets()->loader()
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
		->addComponent(PerspectiveCamera::create(800.f / 600.f, float(M_PI) * 0.25f, .1f, 1000.f));
	root->addChild(camera);

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
	});

	auto material = material::BasicMaterial::create()->diffuseColor(0xCCCCCCFF);

	auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
		root->addComponent(overlay);

		mesh->addComponent(Surface::create(
			sceneManager->assets()->geometry("cubeGeometry"),
			material,
			sceneManager->assets()->effect("effect/Basic.effect")
			));

		overlay->load("html/interface.html");
	});

	onloadSlot = overlay->onload()->connect([=](minko::dom::AbstractDOM::Ptr dom, std::string page)
	{
		if (!dom->isMain())
			return;

		if (dom->fileName() == "interface.html")
		{
			onclickSlot = dom->document()->onclick()->connect([=](dom::AbstractDOMMouseEvent::Ptr event)
			{
				dom->sendMessage("hello");
			});

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

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
	{
		mesh->component<Transform>()->matrix()->appendRotationY(.01f);

		sceneManager->nextFrame(time, deltaTime);
	});

	sceneManager->assets()->loader()->load();
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