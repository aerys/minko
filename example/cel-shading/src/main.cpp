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
#include "minko/MinkoASSIMP.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoPNG.hpp"

using namespace minko;
using namespace minko::component;

const uint WINDOW_WIDTH = 800;
const uint WINDOW_HEIGHT = 600;
const std::string MODEL_FILENAME = "pirate.dae";
const std::string TEXTURE_LIGHT = "texture_light.png";

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Cel Shading", WINDOW_WIDTH, WINDOW_HEIGHT);
    auto sceneManager = SceneManager::create(canvas);
    auto defaultOptions = sceneManager->assets()->loader()->options();

    canvas->context()->errorsEnabled(true);

	auto root = scene::Node::create("root")->addComponent(sceneManager);
	auto camera = scene::Node::create("camera");

    defaultOptions
        ->generateMipmaps(true)
        ->skinningFramerate(60)
        ->skinningMethod(SkinningMethod::HARDWARE)
        ->startAnimation(true)
        ->registerParser<file::ColladaParser>("dae")
		->registerParser<file::JPEGParser>("jpg")
		->registerParser<file::PNGParser>("png")
		;

    auto fxLoader = file::Loader::create(sceneManager->assets()->loader())
        ->queue("effect/Basic.effect")
		->queue("effect/Phong.effect")
		->queue("effect/VertexNormal.effect")
		//->queue("effect/Depth/Depth.effect")
        ->queue("effect/CelShading/CelShading.effect")
		->queue("effect/CelShading/Outline.effect")
		->queue("effect/CelShading/Sobel.effect")
		->queue("effect/FXAA/FXAA.effect")
		->queue(TEXTURE_LIGHT)
		;

	Renderer::Ptr mainRenderer = nullptr;
	Renderer::Ptr basicRenderer = nullptr;
	
	Renderer::Ptr normalRenderer = nullptr;
	auto renderTargetTexture = render::Texture::create(canvas->context(), math::clp2(WINDOW_WIDTH), math::clp2(WINDOW_HEIGHT), false, true);
	renderTargetTexture->upload();

	auto normalRenderTargetTexture = render::Texture::create(canvas->context(), math::clp2(WINDOW_WIDTH), math::clp2(WINDOW_HEIGHT), false, true);
	normalRenderTargetTexture->upload();

	auto enablePP = false;
	auto ppRenderer = Renderer::create();
	auto ppMaterial = material::BasicMaterial::create();
	render::Effect::Ptr ppEffect = nullptr;
	render::Effect::Ptr celShadingEffect = nullptr;
	
    auto fxComplete = fxLoader->complete()->connect([&](file::Loader::Ptr l)
    {
		celShadingEffect = sceneManager->assets()->effect("effect/CelShading/CelShading.effect");
		auto texture = sceneManager->assets()->texture(TEXTURE_LIGHT);
        celShadingEffect->data()->set("discretizedLightMap", texture->sampler());

		mainRenderer = Renderer::create(0x7f7f7fff, nullptr, celShadingEffect);
		basicRenderer = Renderer::create(0x7f7f7fff, nullptr, sceneManager->assets()->effect("effect/Basic.effect"));
		normalRenderer = Renderer::create(0xff0000ff, normalRenderTargetTexture, sceneManager->assets()->effect("effect/VertexNormal.effect"));

		mainRenderer->enabled(true);
		basicRenderer->enabled(false);

		ppMaterial->diffuseMap(renderTargetTexture);
		ppMaterial->data()->set("depthMap", normalRenderTargetTexture->sampler());
        ppMaterial->data()->set("borderThickness", 0.35f);
		ppMaterial->data()->set("borderColor", math::vec3(0.f, 0.f, 0.f));

		ppEffect = sceneManager->assets()->effect("effect/CelShading/Sobel.effect");
		ppEffect->data()->set("invertedDiffuseMapSize", math::vec2(1.f / float(renderTargetTexture->width()), 1.f / float(renderTargetTexture->height())));

		auto postProcessingScene = scene::Node::create()
			->addComponent(ppRenderer)
			->addComponent(
				Surface::create(
					geometry::QuadGeometry::create(sceneManager->assets()->context()),
					ppMaterial,
					ppEffect
				));
		
		camera
			->addComponent(mainRenderer)
			->addComponent(basicRenderer)
			->addComponent(normalRenderer)
			->addComponent(Transform::create(
				math::inverse(
					math::lookAt(
						math::vec3(0.25f, 0.75f, 2.5f),
						math::vec3(0.f, 0.75f, 0.f),
						math::vec3(0, 1, 0))
					)
				)
			)
			->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

        camera->data().set("discretizedLightMap", texture->sampler());

        auto rootProvider = data::Provider::create();
        rootProvider->set("borderThreshold", 0.33f);
        root->data().addProvider(rootProvider);

		root->addChild(camera);

        sceneManager->assets()->loader()->options()->effect(sceneManager->assets()->effect("effect/Basic.effect"));
		sceneManager->assets()->loader()
			->queue(MODEL_FILENAME)
			;
        sceneManager->assets()->loader()->load();
	});

    auto error = sceneManager->assets()->loader()->error()->connect([=](file::Loader::Ptr l, file::Error e)
    {
        std::cout << "error" << e.what() << std::endl;
    });

	auto model = scene::Node::create("model")
		->addComponent(Transform::create(math::translate(math::vec3(-0.5f, 0.f, 0.f))));
	auto mesh = scene::Node::create("mesh")
		->addComponent(Transform::create(math::translate(math::vec3(0.5f, 0.5f, 0.f)) * math::scale(math::vec3(0.1f))));
	auto directionalLightNode = scene::Node::create("dirLightNode")
		->addComponent(Transform::create(math::inverse(math::lookAt(math::vec3(0.f, 2.f, 2.f), math::vec3(0.f), math::vec3(0.f, 1.f, 0.f)))))
		->addComponent(DirectionalLight::create());

    auto _ = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {
		auto material = material::BasicMaterial::create();
		material->diffuseColor(math::vec4(0.f, 1.f, 0.f, 1.f));

		auto meshGeometry = geometry::TeapotGeometry::create(sceneManager->assets()->context());
		meshGeometry->computeNormals();
		mesh
			->addComponent(Surface::create(
				meshGeometry,
				material,
				sceneManager->assets()->effect("effect/Basic.effect")
			));

        auto symbol = sceneManager->assets()->symbol(MODEL_FILENAME);
		model->addChild(symbol);

		auto cubeNode = scene::Node::create("cubeNode")
			->addComponent(Transform::create(math::scale(math::vec3(0.1f))))
			->addComponent(Surface::create(
				geometry::CubeGeometry::create(canvas->context()),
				material,
				sceneManager->assets()->effect("effect/Basic.effect")
			));

		directionalLightNode->addChild(cubeNode);

		auto groundMaterial = material::BasicMaterial::create();
		groundMaterial->diffuseColor(0xff0000ff);
		auto groundNode = scene::Node::create("ground")
			->addComponent(Transform::create(math::scale(math::vec3(100.0f)) * math::rotate(-float(M_PI) * .5f, math::vec3(1.f, 0.f, 0.f))))
			->addComponent(Surface::create(
				geometry::QuadGeometry::create(sceneManager->assets()->context()),
				groundMaterial,
				sceneManager->assets()->effect("effect/Basic.effect")
			));

		auto quadMaterial = material::BasicMaterial::create();
		quadMaterial->diffuseMap(renderTargetTexture);
		auto quadNode = scene::Node::create("renderTarget")
			->addComponent(Transform::create(math::translate(math::vec3(0.f, 0.5f, -1.f))))
			->addComponent(Surface::create(
				geometry::QuadGeometry::create(sceneManager->assets()->context()),
				quadMaterial,
				sceneManager->assets()->effect("effect/Basic.effect")
			));

		root->addComponent(AmbientLight::create())
			->addChild(directionalLightNode)
			->addChild(model)
			->addChild(mesh)
			//->addChild(groundNode)
			//->addChild(quadNode);
		;
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr c, uint w, uint h)
    {
        camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));

		//renderTargetTexture = render::Texture::create(sceneManager->assets()->context(), math::clp2(w), math::clp2(h), false, true);
		//renderTargetTexture->upload();

		//ppMaterial->diffuseMap(renderTargetTexture);
		//ppEffect->data()->set("invertedDiffuseMapSize", math::vec2(1.f / float(renderTargetTexture->width()), 1.f / float(renderTargetTexture->height())));
    });

    // currently, keyUp events seem not to be fired at the individual key level
    auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
    {
		if (k->keyIsDown(input::Keyboard::SPACE))
		{
            enablePP = !enablePP;

			if (enablePP)
				std::cout << "Enable post processing" << std::endl;
			else
				std::cout << "Disable post processing" << std::endl;
		}
        else if (k->keyIsDown(input::Keyboard::A))
        {
            auto newValue = root->data().get<float>("borderThreshold") + 0.001f;
            root->data().set("borderThreshold", newValue);

            std::cout << "Border threshold: " << newValue << std::endl;
        }
        else if (k->keyIsDown(input::Keyboard::Z))
        {
            auto newValue = root->data().get<float>("borderThreshold") - 0.001f;
            root->data().set("borderThreshold", newValue);

            std::cout << "Border threshold: " << newValue << std::endl;
        }
		else if (k->keyIsDown(input::Keyboard::P))
		{
			auto newValue = ppMaterial->data()->get<float>("borderThickness") + 0.001f;
			ppMaterial->data()->set("borderThickness", newValue);

			std::cout << "Border thickness: " << newValue << std::endl;
		}
		else if (k->keyIsDown(input::Keyboard::L))
		{
			auto newValue = ppMaterial->data()->get<float>("borderThickness") - 0.001f;
			ppMaterial->data()->set("borderThickness", newValue);

			std::cout << "Border thickness: " << newValue << std::endl;
		}
		else if (k->keyIsDown(input::Keyboard::B))
		{
			mainRenderer->enabled(false);
			basicRenderer->enabled(true);
		}
		else if (k->keyIsDown(input::Keyboard::C))
		{
			basicRenderer->enabled(false);
			mainRenderer->enabled(true);
		}
    });
	
    auto keyUp = canvas->keyboard()->keyUp()->connect([&](input::Keyboard::Ptr k)
    {
    });

	Signal<input::Mouse::Ptr, int, int>::Slot mouseMove;
    float cameraRotationSpeed = 0.f;

    auto mouseDown = canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr mouse)
    {
        mouseMove = canvas->mouse()->move()->connect([&](input::Mouse::Ptr m, int dx, int dy)
        {
            cameraRotationSpeed = static_cast<float>(-dx) * .01f;
        });
    });

    auto mouseUp = canvas->mouse()->leftButtonUp()->connect([&](input::Mouse::Ptr mouse)
    {
        mouseMove = nullptr;
    });

    auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr c, float t, float dt)
    {
        camera->component<Transform>()->matrix(math::rotate(cameraRotationSpeed, math::vec3(0, 1, 0)) * camera->component<Transform>()->matrix());
        cameraRotationSpeed *= .99f;

		//mesh->component<Transform>()->matrix(
		//	mesh->component<Transform>()->matrix() * math::rotate(0.01f, math::vec3(0, 1, 0))
		//);

		//model->component<Transform>()->matrix(
		//	model->component<Transform>()->matrix() * math::rotate(0.01f, math::vec3(0, 1, 0))
		//);

		directionalLightNode->component<Transform>()->matrix(
			math::rotate(0.01f, math::vec3(0, 1, 0)) * directionalLightNode->component<Transform>()->matrix()
		);

		if (enablePP)
		{
			sceneManager->nextFrame(t, dt, renderTargetTexture);
			ppRenderer->render(canvas->context());
		}
		else
		{
			sceneManager->nextFrame(t, dt);
		}
    });

	fxLoader->load();
    canvas->run();

    return 0;
}