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
#include "minko/MinkoASSIMP.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoSerializer.hpp"

#include "ClippingPlane.hpp"
#include "ClippingPlaneLayout.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::render;

using namespace player::component;

int main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Section", 1268, 720, Canvas::Flags::STENCIL);
#ifdef DEBUG
    canvas->context()->errorsEnabled(true);
#endif
	auto sceneManager = SceneManager::create(canvas);
	auto defaultLoader = sceneManager->assets()->loader();
	auto fxLoader = file::Loader::create(defaultLoader);

    auto clippingPlanes = std::vector<ClippingPlane::Ptr>();

    defaultLoader->options()
		->resizeSmoothly(true)
        ->generateMipmaps(true)
        ->registerParser<file::PNGParser>("png")
        ->registerParser<file::JPEGParser>("jpg")
        ->registerParser<file::OBJParser>("obj")
        ->registerParser<file::ColladaParser>("dae")
        ->registerParser<file::FBXParser>("FBX")
        ->registerParser<file::SceneParser>("scene");

	canvas->context()->errorsEnabled(true);

	fxLoader
		->queue("effect/Basic.effect")
		->queue("effect/CrossSectionDepth.effect")
		->queue("effect/CrossSectionStencil.effect")
		->queue("effect/ClippingPlane.effect")
		->queue("effect/Stripes.effect");

	auto fxError = defaultLoader->error()->connect([&](file::Loader::Ptr loader, const file::Error& error)
	{
		std::cout << "File loading error: " << error.what() << std::endl;
	});

    auto root = scene::Node::create("root")->addComponent(sceneManager);
    auto camera = scene::Node::create("camera");

	sceneManager->assets()->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()));

	auto fxComplete = fxLoader->complete()->connect([&](file::Loader::Ptr loader)
	{
	    auto mainRenderer = Renderer::create(0x7f7f7fff);
        mainRenderer->layoutMask(scene::BuiltinLayout::DEFAULT | ClippingPlaneLayout::CLIPPING);
        // We want to keep stencil buffer data
        mainRenderer->clearFlags(ClearFlags::COLOR | ClearFlags::DEPTH);
		// mainRenderer->enabled(false);

	    camera
		    ->addComponent(mainRenderer)
		    ->addComponent(
		        Transform::create(
				    math::inverse(
					    math::lookAt(
						    math::vec3(0.f, 0.f, 10.f),
						    math::zero<math::vec3>(),
						    math::vec3(0.f, 1.f, 0.f)
					    )
				    )
			    )
		    )
		    ->addComponent(Camera::create(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f)));

	    root->addChild(camera);

        defaultLoader->options()->effect(sceneManager->assets()->effect("effect/ClippingPlane.effect"));
        defaultLoader->options()->disposeTextureAfterLoading(true);
        defaultLoader->load();
	});

    auto mesh = scene::Node::Ptr();
    auto _ = defaultLoader->complete()->connect([=, &mesh](file::Loader::Ptr loader)
    {
        auto surface = Surface::create(
			sceneManager->assets()->geometry("cube"),
			material::BasicMaterial::create(),
			sceneManager->assets()->effect("effect/ClippingPlane.effect")
		);

		mesh = scene::Node::create("mesh");
		mesh->addComponent(surface);
		mesh->addComponent(Transform::create());

        mesh->layout(scene::BuiltinLayout::DEFAULT | ClippingPlaneLayout::CLIPPED);

        surface->material()->data()->set("triangleCulling", minko::render::TriangleCulling::NONE);

        root->addChild(mesh);
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
		distance -= float(v) * 1;
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
        if (!clippingPlanes.empty())
        {
            const auto clippingPlane = clippingPlanes.back();
            auto transform = clippingPlane->originNode()->component<Transform>();
            auto transformMatrix = transform->matrix();

            // Change position
            if (k->keyIsDown(input::Keyboard::UP))
            {
                transformMatrix *= math::translate(math::vec3(0.f, 0.f, 0.01f));
            }
            else if (k->keyIsDown(input::Keyboard::DOWN))
            {
                transformMatrix *= math::translate(math::vec3(0.f, 0.f, -0.01f));
            }
            else if (k->keyIsDown(input::Keyboard::LEFT))
            {
                transformMatrix *= math::translate(math::vec3(0.f, -0.01f, 0.f));
            }
            else if (k->keyIsDown(input::Keyboard::RIGHT))
            {
                transformMatrix *= math::translate(math::vec3(0.f, 0.01f, 0.f));
            }

            // Change rotation
            else if (k->keyIsDown(input::Keyboard::PAGE_UP))
            {
                transformMatrix *= math::rotate(-0.1f, math::vec3(1.f, 0.f, 0.f));
            }
            else if (k->keyIsDown(input::Keyboard::PAGE_DOWN))
            {
                transformMatrix *= math::rotate(0.1f, math::vec3(1.f, 0.f, 0.f));
            }
            else if (k->keyIsDown(input::Keyboard::HOME))
            {
                transformMatrix *= math::rotate(-0.1f, math::vec3(0.f, 1.f, 0.f));
            }
            else if (k->keyIsDown(input::Keyboard::END))
            {
                transformMatrix *= math::rotate(0.1f, math::vec3(0.f, 1.f, 0.f));
            }
            else if (k->keyIsDown(input::Keyboard::INSERT))
            {
                transformMatrix *= math::rotate(-0.1f, math::vec3(0.f, 0.f, 1.f));
            }
            else if (k->keyIsDown(input::Keyboard::DEL))
            {
                transformMatrix *= math::rotate(0.1f, math::vec3(0.f, 0.f, 1.f));
            }

            transform->matrix(transformMatrix);
        }

        if (k->keyIsDown(input::Keyboard::SPACE))
        {
            const auto deltaRotation = math::rotate(math::radians(-90.f), math::vec3(0.f, 1.f, 0.f));
            auto rotation = math::mat4();

            for (auto i = 0; i < 2; ++i)
            {
                clippingPlanes.push_back(ClippingPlane::create());
                auto clippingPlane = clippingPlanes.back();
                clippingPlane->basePlaneTransformMatrix(math::scale(math::vec3(5.f)) * rotation);
                mesh->addComponent(clippingPlane);
                rotation *= deltaRotation;
            }
        }
	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, unsigned int w, unsigned int h)
	{
        camera->component<Camera>()->projectionMatrix(math::perspective(.785f, float(w) / float(h), 0.1f, 1000.f));
	});

	auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr c, float time, float deltaTime, bool shouldRender)
	{
		yaw += cameraRotationYSpeed;
		cameraRotationYSpeed *= 0.9f;
		pitch += cameraRotationXSpeed;
		cameraRotationXSpeed *= 0.9f;

		if (pitch > maxPitch)
			pitch = maxPitch;
		else if (pitch < minPitch)
			pitch = minPitch;

		camera->component<Transform>()->matrix(
			math::inverse(
				math::lookAt(
					math::vec3(
						lookAt.x + distance * std::cos(yaw) * std::sin(pitch),
						lookAt.y + distance * std::cos(pitch),
						lookAt.z + distance * std::sin(yaw) * std::sin(pitch)
					),
					lookAt,
					math::vec3(0.f, 1.f, 0.f)
				)
			)
		);

		sceneManager->nextFrame(time, deltaTime, shouldRender);
	});

	fxLoader->load();
	canvas->run();
}
