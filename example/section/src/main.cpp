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
using namespace minko::render;

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
	auto canvas = Canvas::create("Minko Example - Section");
	auto sceneManager = SceneManager::create(canvas);
	auto defaultLoader = sceneManager->assets()->loader();
	auto fxLoader = file::Loader::create(defaultLoader);

	canvas->context()->errorsEnabled(true);

	fxLoader
		->queue("effect/Basic.effect")
		->queue("effect/CrossSection.effect")
		->queue("effect/ClippingPlane.effect");

	auto fxError = defaultLoader->error()->connect([&](file::Loader::Ptr loader, const file::Error& error)
	{
		std::cout << "File loading error: " << error.what() << std::endl;
	});
	
	auto root = scene::Node::create("root")->addComponent(sceneManager);

	root->data().providers().front()->set("clippingPlane", math::vec4());

	sceneManager->assets()->geometry("teapot", geometry::TeapotGeometry::create(sceneManager->assets()->context()));

	auto clippingPlaneMesh = scene::Node::create("clippingPlane");

    auto camera = scene::Node::create("camera");

	auto fxComplete = fxLoader->complete()->connect([&](file::Loader::Ptr loader)
	{
        auto stencilBufferRenderer = Renderer::create(
		    0x00ffffff, 
		    nullptr, 
		    sceneManager->assets()->effect("effect/CrossSection.effect"),
		    "default",
		    1000.f,
		    "Stencil renderer"
	    );
    
	    auto mainRenderer = Renderer::create(0x7f7f7fff);
        mainRenderer->clearFlags(ClearFlags::DEPTH);

	    camera
		    ->addComponent(mainRenderer)
            ->addComponent(stencilBufferRenderer)
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
		    ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

	    root->addChild(camera);

		auto surface = Surface::create(
			sceneManager->assets()->geometry("teapot"),
			material::BasicMaterial::create(),
			sceneManager->assets()->effect("effect/ClippingPlane.effect")
		);

		auto mesh = scene::Node::create("mesh");
		mesh->addComponent(surface);
		mesh->addComponent(Transform::create());

		auto transform = Transform::create();
		transform->matrix(math::scale(math::vec3(30)) * transform->matrix());
		transform->matrix(math::rotate((float)-M_PI_2, math::vec3(1.f, 0.f, 0.f)) * transform->matrix());
		
		auto clippingPlaneMeshMaterial = material::BasicMaterial::create();
		clippingPlaneMeshMaterial->data()->set("diffuseColor", math::vec4(1.0f, 1.0f, 0.f, 1.f));
        clippingPlaneMeshMaterial->data()->set("triangleCulling", minko::render::TriangleCulling::FRONT);

        clippingPlaneMeshMaterial->data()
            ->set("diffuseColor", math::vec4(1.0f, 0.f, 0.f, 1.f))
            ->set("depthFunction", CompareMode::ALWAYS)
            ->set("stencilFunction", CompareMode::EQUAL)
            ->set("stencilReference", 1)
            ->set("stencilMask", uint(1))
            ->set("stencilFailOperation", StencilOperation::KEEP);

		auto clippingPlaneMeshSurface = Surface::create(
			geometry::QuadGeometry::create(sceneManager->assets()->context()),
			clippingPlaneMeshMaterial,
			sceneManager->assets()->effect("effect/Basic.effect")
		);

        //surface->material()->data()->set("triangleCulling", minko::render::TriangleCulling::NONE);

		clippingPlaneMesh->addComponent(transform);
		clippingPlaneMesh->addComponent(clippingPlaneMeshSurface);

		root->addChild(clippingPlaneMesh);
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
		if (clippingPlaneMesh->hasComponent<Transform>())
		{
			auto transform = clippingPlaneMesh->component<Transform>();
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

			auto surface = clippingPlaneMesh->component<Surface>();
			auto vertexBuffer = surface->geometry()->vertexBuffer("normal");
			auto normalAttribute = vertexBuffer->attribute("normal");
			auto normalVector = math::vec3(
				vertexBuffer->data()[normalAttribute.offset],
				vertexBuffer->data()[normalAttribute.offset + 1],
				vertexBuffer->data()[normalAttribute.offset + 2]
			);

			normalVector = math::normalize(math::mat3(transformMatrix) * normalVector);
			auto clippingPlane = computeClippingPlane(transformMatrix[3].xyz, normalVector);
			root->data().providers().front()->set("clippingPlane", clippingPlane);
		}
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

		sceneManager->nextFrame(time, deltaTime);
	});

	fxLoader->load();
	canvas->run();
}
