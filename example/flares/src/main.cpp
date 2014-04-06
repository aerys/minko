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

#define WINDOW_WIDTH  	800
#define WINDOW_HEIGHT 	600

scene::Node::Ptr camera = nullptr;

Signal<input::Keyboard::Ptr>::Slot keyDown;

int main(int argc, char** argv)
{
	auto canvas 			= Canvas::create("Minko Example - Light", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager		= SceneManager::create(canvas->context());
	auto defaultLoader		= sceneManager->assets()->loader();
	auto root				= scene::Node::create("root")->addComponent(sceneManager);
	auto sphereGeometry		= geometry::SphereGeometry::create(sceneManager->assets()->context(), 32, 32, true);
	auto lights				= scene::Node::create("lights")->addComponent(Transform::create());
	auto sphereMaterial		= material::PhongMaterial::create()
		->shininess(16.f)
		->specularColor(Vector4::create(1.0f, 1.0f, 1.0f, 1.0f))
		->diffuseColor(Vector4::create(1.f, 1.f, 1.f, 1.f));

	sphereGeometry->computeTangentSpace(false);

	// setup assets
    sceneManager->assets()
        ->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->geometry("quad", geometry::QuadGeometry::create(sceneManager->assets()->context()))
        ->geometry("sphere", sphereGeometry);

	defaultLoader->options()
		->generateMipmaps(true)
        ->registerParser<file::PNGParser>("png");
    defaultLoader
        ->queue("texture/normalmap-cells.png")
		->queue("texture/sprite-pointlight.png")
		->queue("effect/PseudoLensFlare/PseudoLensFlare.effect")
		->queue("effect/Basic.effect")
		->queue("effect/Sprite.effect")
		->queue("effect/Phong.effect");

	auto _ = defaultLoader->complete()->connect([=](file::Loader::Ptr loader)
	{
		// ground
		auto ground = scene::Node::create("ground")
			->addComponent(Surface::create(
				sceneManager->assets()->geometry("quad"),
				material::Material::create()
					->set("diffuseColor",	Vector4::create(1.f, 1.f, 1.f, 1.f)),
				sceneManager->assets()->effect("phong")
			))
			->addComponent(Transform::create(Matrix4x4::create()->appendScale(50.f)->appendRotationX(-1.57f)));
		root->addChild(ground);

		// sphere
		auto sphere = scene::Node::create("sphere")
			->addComponent(Surface::create(
				sceneManager->assets()->geometry("sphere"),
				sphereMaterial,
				sceneManager->assets()->effect("phong")
			))
			->addComponent(Transform::create(Matrix4x4::create()->appendTranslation(0.f, 2.f, 0.f)->prependScale(3.f)));
		root->addChild(sphere);

		// spotLight
		auto spotLight = scene::Node::create("spotLight")
			->addComponent(SpotLight::create(.15f, .4f))
			->addComponent(Transform::create(Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(15.f, 20.f, 0.f))));
		spotLight->component<SpotLight>()->diffuse(.4f);
		root->addChild(spotLight);

		root->addChild(lights);
	});

	// camera init
	camera = scene::Node::create("camera")
		->addComponent(Renderer::create())
		->addComponent(PerspectiveCamera::create((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT))
		->addComponent(Transform::create(
			Matrix4x4::create()->lookAt(Vector3::create(0.f, 2.f), Vector3::create(10.f, 10.f, 10.f))
		));
	root->addChild(camera);

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, unsigned int width, unsigned int height)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)width / (float)height);
	});

	auto yaw = 0.f;
	auto pitch = (float)PI * .5f;
	auto minPitch = 0.f + 1e-5;
	auto maxPitch = (float)PI - 1e-5;
	auto lookAt = Vector3::create(0.f, 2.f, 0.f);
	auto distance = 20.f;

	// handle mouse signals
	auto mouseWheel = canvas->mouse()->wheel()->connect([&](input::Mouse::Ptr m, int h, int v)
	{
		distance += (float)v / 10.f;
	});

	Signal<input::Mouse::Ptr, int, int>::Slot mouseMove;
	auto cameraRotationXSpeed = 0.f;
	auto cameraRotationYSpeed = 0.f;

	auto mouseDown = canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr m)
	{
		mouseMove = canvas->mouse()->move()->connect([&](input::Mouse::Ptr, int dx, int dy)
		{
			cameraRotationYSpeed = (float)dx * .01f;
			cameraRotationXSpeed = (float)dy * -.01f;
		});
	});

	auto mouseUp = canvas->mouse()->leftButtonUp()->connect([&](input::Mouse::Ptr m)
	{
		mouseMove = nullptr;
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
	{
		yaw += cameraRotationYSpeed;
		cameraRotationYSpeed *= 0.9f;

		pitch += cameraRotationXSpeed;
		cameraRotationXSpeed *= 0.9f;
		if (pitch > maxPitch)
			pitch = maxPitch;
		else if (pitch < minPitch)
			pitch = minPitch;

		camera->component<Transform>()->matrix()->lookAt(
			lookAt,
			Vector3::create(
				lookAt->x() + distance * cosf(yaw) * sinf(pitch),
				lookAt->y() + distance * cosf(pitch),
				lookAt->z() + distance * sinf(yaw) * sinf(pitch)
			)
		);

		lights->component<Transform>()->matrix()->appendRotationY(.005f);

		sceneManager->nextFrame(time, deltaTime);
	});

	sceneManager->assets()->loader()->load();

	canvas->run();

	exit(EXIT_SUCCESS);
}
