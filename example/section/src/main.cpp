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
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoASSIMP.hpp"

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

std::string MODEL_FILENAME = "lights.scene";

int main(int argc, char** argv)
{
    auto inputFileName = std::string(MODEL_FILENAME);

    for (auto i = 1; i < argc; ++i)
    {
        const auto arg = std::string(argv[i]);
        if (arg == "-i")
            inputFileName = std::string(argv[++i]);
    }

    auto canvas = Canvas::create("Minko Example - Section", 1280, 720, Canvas::Flags::STENCIL);
	auto sceneManager = SceneManager::create(canvas);
	auto defaultLoader = sceneManager->assets()->loader();
	auto fxLoader = file::Loader::create(defaultLoader);

    defaultLoader->options()
		->resizeSmoothly(true)
        ->generateMipmaps(true)
        ->registerParser<file::PNGParser>("png")
        ->registerParser<file::JPEGParser>("jpg")
        ->registerParser<file::OBJParser>("obj")
        ->registerParser<file::ColladaParser>("dae")
        ->registerParser<file::FBXParser>("FBX");

	canvas->context()->errorsEnabled(true);

	fxLoader
		->queue("effect/Basic.effect")
		->queue("effect/CrossSectionDepth.effect")
		->queue("effect/CrossSectionStencil.effect")
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
        auto depthBufferRenderer = Renderer::create(
		    0x7f7f7fff, 
		    nullptr, 
		    sceneManager->assets()->effect("effect/CrossSectionDepth.effect"),
		    "default",
		    1042.f,
		    "Depth buffer renderer"
	    );

        auto stencilBufferRenderer = Renderer::create(
		    0x00000000, 
		    nullptr, 
		    sceneManager->assets()->effect("effect/CrossSectionStencil.effect"),
		    "default",
		    1000.f,
		    "Stencil buffer renderer"
	    );
    
        depthBufferRenderer->layoutMask(scene::BuiltinLayout::CLIPPING);
        stencilBufferRenderer->layoutMask(scene::BuiltinLayout::CLIPPED);
        // We want to keep depth buffer data
        stencilBufferRenderer->clearFlags(ClearFlags::COLOR | ClearFlags::STENCIL);

	    auto mainRenderer = Renderer::create(0x7f7f7fff);
        // We want to keep stencil buffer data
        mainRenderer->clearFlags(ClearFlags::COLOR | ClearFlags::DEPTH);

	    camera
            ->addComponent(depthBufferRenderer)
            ->addComponent(stencilBufferRenderer)
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
        defaultLoader->queue(inputFileName);
        defaultLoader->load();
	});

     auto _ = defaultLoader->complete()->connect([=](file::Loader::Ptr loader)
    {
        auto sceneNode = sceneManager->assets()->symbol(inputFileName);
        
        if (!sceneNode->hasComponent<Transform>())
            sceneNode->addComponent(Transform::create());
        
        auto surface = Surface::create(
			sceneManager->assets()->geometry("teapot"),
			material::BasicMaterial::create(),
			sceneManager->assets()->effect("effect/ClippingPlane.effect")
		);

		auto mesh = scene::Node::create("mesh");
		mesh->addComponent(surface);
		mesh->addComponent(Transform::create());

        mesh->layout(scene::BuiltinLayout::CLIPPED);
        sceneNode->layout(scene::BuiltinLayout::CLIPPED);

        auto nodeSet = scene::NodeSet::create(sceneNode)->descendants(true)->where([&](std::shared_ptr<scene::Node> n)
        {
            return n->hasComponent<Surface>();
        });

        for (auto node : nodeSet->nodes())
        {
            node->layout(scene::BuiltinLayout::CLIPPED);
        }

		auto transform = Transform::create();
		transform->matrix(math::scale(math::vec3(30)) * transform->matrix());
		transform->matrix(math::rotate((float)-M_PI_2, math::vec3(1.f, 0.f, 0.f)) * transform->matrix());
		
		auto clippingPlaneMeshMaterial = material::BasicMaterial::create();
		clippingPlaneMeshMaterial->data()->set("diffuseColor", math::vec4(1.0f, 1.0f, 0.f, 1.f));
        clippingPlaneMeshMaterial->data()->set("triangleCulling", minko::render::TriangleCulling::NONE);

        clippingPlaneMeshMaterial->data()
            ->set("diffuseColor", math::vec4(1.0f, 0.f, 0.f, 1.f))
            ->set("stencilFunction", CompareMode::NOT_EQUAL)
            ->set("stencilReference", 0)
            ->set("stencilMask", uint(1));

		auto clippingPlaneMeshSurface = Surface::create(
			geometry::QuadGeometry::create(sceneManager->assets()->context()),
			clippingPlaneMeshMaterial,
			sceneManager->assets()->effect("effect/Basic.effect")
		);

        //surface->material()->data()->set("triangleCulling", minko::render::TriangleCulling::NONE);

		clippingPlaneMesh->addComponent(transform);
		clippingPlaneMesh->addComponent(clippingPlaneMeshSurface);

        clippingPlaneMesh->layout(scene::BuiltinLayout::CLIPPING);

		root->addChild(clippingPlaneMesh);
		root->addChild(sceneNode);
		//root->addChild(mesh);
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
            auto clippingPlane = computeClippingPlane(transformMatrix[3].xyz(), normalVector);
			root->data().providers().front()->set("clippingPlane", clippingPlane);
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
