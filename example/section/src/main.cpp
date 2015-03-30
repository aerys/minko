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
#include "minko/MinkoSerializer.hpp"

using namespace minko;
using namespace minko::component;

math::vec4
computeClippingPlane(math::vec3 position, math::vec3 normal)
{
	auto clippingPlane = math::vec4();
	clippingPlane.x = normal.x;
	clippingPlane.y = normal.y;
	clippingPlane.z = normal.z;
	clippingPlane.w = (-(math::dot(normal, position)));

	return clippingPlane;
}

int main(int argc, char** argv)
{
	std::string inputFileName;

	for (auto i = 1; i < argc; ++i)
	{
		const auto arg = std::string(argv[i]);
		if (arg == "-i")
			inputFileName = std::string(argv[++i]);
	}

	auto canvas = Canvas::create("Minko Example - Section");
	auto sceneManager = SceneManager::create(canvas);
	auto defaultLoader = sceneManager->assets()->loader();
	auto fxLoader = file::Loader::create(defaultLoader);

	fxLoader->queue("effect/CrossSection.effect");

	defaultLoader->options()
		->generateMipmaps(true)
		->registerParser<file::SceneParser>("scene");

	auto fxComplete = fxLoader->complete()->connect([&](file::Loader::Ptr loader)
	{
		defaultLoader->options()->effect(sceneManager->assets()->effect("effect/CrossSection.effect"));
		defaultLoader->options()->disposeTextureAfterLoading(true);
		defaultLoader->queue(inputFileName);
		defaultLoader->load();

		//auto crossSectionEffect = sceneManager->assets()->effect("effect/CrossSection.effect");
	});

	auto fxError = defaultLoader->error()->connect([&](file::Loader::Ptr loader, const file::Error& error)
	{
		std::cout << "File loading error: " << error.what() << std::endl;
	});
	
	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto clippingPlanePosition = math::vec3();
	auto clippingPlaneNormal = math::vec3(1, 0, 0);
	auto rotationMatrix = math::mat4();

	auto clippingPlane = computeClippingPlane(clippingPlanePosition, clippingPlaneNormal);
	root->data().providers().front()->set("clippingPlane", clippingPlane);

	auto renderer = Renderer::create(0x7f7f7fff);

	auto camera = scene::Node::create("camera")
		->addComponent(renderer)
		->addComponent(Transform::create(
		math::inverse(math::lookAt(math::vec3(0.f, 0.f, 10.f), math::zero<math::vec3>(), math::vec3(0.f, 1.f, 0.f))
		)))
		->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

	root->addChild(camera);

	auto _ = defaultLoader->complete()->connect([&](file::Loader::Ptr loader)
	{
		auto sceneNode = sceneManager->assets()->symbol(inputFileName);
		
		root->addChild(sceneNode);

		if (!sceneNode->hasComponent<Transform>())
			sceneNode->addComponent(Transform::create());
	});

	auto yaw = float(M_PI) * 0.25f;
	auto pitch = float(M_PI) * .25f;
	auto roll = 0.f;
	float minPitch = 0.f + float(1e-5);
	float maxPitch = float(M_PI) - float(1e-5);
	auto lookAt = math::vec3(0.f, 0.f, 0.f);
	auto distance = 10.f;
	Signal<input::Mouse::Ptr, int, int>::Slot mouseMove;
	auto cameraRotationXSpeed = 0.f;
	auto cameraRotationYSpeed = 0.f;

	// handle mouse signals
	auto mouseWheel = canvas->mouse()->wheel()->connect([&](input::Mouse::Ptr m, int h, int v)
	{
		distance += float(v) * 1;
	});

	mouseMove = canvas->mouse()->move()->connect([&](input::Mouse::Ptr m, int dx, int dy)
	{
		if (m->leftButtonIsDown())
		{
			cameraRotationYSpeed = float(dx) * .01f;
			cameraRotationXSpeed = float(dy) * -.01f;
		}
	});

	// handle keyboard signals
	auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
	{
		// Change position
		if (k->keyIsDown(input::Keyboard::UP))
		{
			clippingPlanePosition.y += .01f;
		}
		else if (k->keyIsDown(input::Keyboard::DOWN))
		{
			clippingPlanePosition.y -= .01f;
		}
		else if (k->keyIsDown(input::Keyboard::LEFT))
		{
			clippingPlanePosition.x -= .01f;
		}
		else if (k->keyIsDown(input::Keyboard::RIGHT))
		{
			clippingPlanePosition.x += .01f;
		}
		// Change rotation
		else if (k->keyIsDown(input::Keyboard::PAGE_UP))
		{
			rotationMatrix = rotationMatrix * math::rotate(-0.001f, math::vec3(0.f, 0.f, 1.f));
			clippingPlaneNormal = math::vec3(rotationMatrix * math::vec4(clippingPlaneNormal, 1));
		}
		else if (k->keyIsDown(input::Keyboard::PAGE_DOWN))
		{
			rotationMatrix = rotationMatrix * math::rotate(0.001f, math::vec3(0.f, 0.f, 1.f));
			clippingPlaneNormal = math::vec3(rotationMatrix * math::vec4(clippingPlaneNormal, 1));
		}
		else if (k->keyIsDown(input::Keyboard::HOME))
		{
			rotationMatrix = rotationMatrix * math::rotate(-0.001f, math::vec3(0.f, 1.f, 0.f));
			clippingPlaneNormal = math::vec3(rotationMatrix * math::vec4(clippingPlaneNormal, 1));
		}
		else if (k->keyIsDown(input::Keyboard::END))
		{
			rotationMatrix = rotationMatrix * math::rotate(0.001f, math::vec3(0.f, 1.f, 0.f));
			clippingPlaneNormal = math::vec3(rotationMatrix * math::vec4(clippingPlaneNormal, 1));
		}
		
		clippingPlaneNormal = math::normalize(clippingPlaneNormal);
		auto clippingPlane = computeClippingPlane(clippingPlanePosition, clippingPlaneNormal);
		root->data().providers().front()->set("clippingPlane", clippingPlane);
	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, unsigned int w, unsigned int h)
	{
		root->children()[0]->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
	});

	auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr c, float time, float deltaTime)
	{
		yaw += cameraRotationYSpeed;
		cameraRotationYSpeed *= 0.9f;
		pitch += cameraRotationXSpeed;
		cameraRotationXSpeed *= 0.9f;

		if (pitch > maxPitch)
			pitch = maxPitch;
		else if (pitch < minPitch)
			pitch = minPitch;

		camera->component<Transform>()->matrix(math::inverse(math::lookAt((
			math::vec3(
			lookAt.x + distance * std::cos(yaw) * std::sin(pitch),
			lookAt.y + distance * std::cos(pitch),
			lookAt.z + distance * std::sin(yaw) * std::sin(pitch)
			)),
			lookAt,
			math::vec3(0.f, 1.f, 0.f)
			)));
		sceneManager->nextFrame(time, deltaTime);
	});

	fxLoader->load();
	canvas->run();
}
