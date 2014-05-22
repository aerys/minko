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
#include "minko/MinkoFX.hpp"
#include "minko/MinkoSerializer.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const std::string MODEL_FILENAME	= "model/ironman.scene";

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Cube", 800, 600);

	auto sceneManager = SceneManager::create(canvas->context());

	// setup assets
	sceneManager->assets()->loader()->options()->resizeSmoothly(true);
	sceneManager->assets()->loader()->options()->generateMipmaps(true);
	sceneManager->assets()->loader()->options()
		->registerParser<file::SceneParser>("scene")
        ->registerParser<file::PNGParser>("png");

	auto fxLoader = file::Loader::create(sceneManager->assets()->loader())
		->queue("effect/Phong.effect")
		->queue("effect/FXAA/FXAA.effect")
		->queue("effect/Basic.effect")
		->queue("effect/VertexNormal.effect")
		->queue("texture/halftone.png")
		->queue("effect/Hologram/HologramVertexNormalFront.effect")
		->queue("effect/Hologram/HologramVertexNormalBack.effect")
		->queue("effect/Hologram/Hologram.effect");

	auto fxComplete = fxLoader->complete()->connect([&](file::Loader::Ptr l)
	{
		sceneManager->assets()->loader()->options()->effect(sceneManager->assets()->effect("effect/Hologram/Hologram.effect"));
		sceneManager->assets()->loader()->queue(MODEL_FILENAME);
		sceneManager->assets()->loader()->load();
	});

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto mesh = scene::Node::create("mesh")
		->addComponent(Transform::create());
	
	auto mesh2 = scene::Node::create("mesh2")
		->addComponent(Transform::create());

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
		Matrix4x4::create()->lookAt(Vector3::create(0.f, 0.8f, 0.f), Vector3::create(0.f, 2.0f, 3.5f))
		))
		->addComponent(PerspectiveCamera::create(800.f / 600.f, (float)PI * 0.25f, .1f, 1000.f));
	root->addChild(camera);

	// FXAA
	render::AbstractTexture::Ptr 	ppTarget = render::Texture::create(
		sceneManager->assets()->context(),
		math::clp2(canvas->width()),
		math::clp2(canvas->height()),
		false,
		true
		);

	ppTarget->upload();

	auto ppRenderer = Renderer::create(0x7f7f7fff);
	auto ppData = data::Provider::create()
		->set("backbuffer", ppTarget)
		->set("invBackbufferSize", Vector2::create(1.f / (float)ppTarget->width(), 1.f / (float)ppTarget->height()));

	auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
		auto cubeGeometry = geometry::CubeGeometry::create(sceneManager->assets()->context());
		auto sphereGeometry = geometry::SphereGeometry::create(sceneManager->assets()->context(), 30, 30);

		auto hologram = Hologram::create(
			sceneManager->assets()->effect("effect/Hologram/HologramVertexNormalFront.effect"),
			sceneManager->assets()->effect("effect/Hologram/HologramVertexNormalBack.effect"),
			canvas->context());

		sceneManager->assets()->geometry("cubeGeometry", cubeGeometry);
		sceneManager->assets()->geometry("sphereGeometry", sphereGeometry);

		mesh->addComponent(Surface::create(
            sceneManager->assets()->geometry("cubeGeometry"),
			material::Material::create()
				->set("diffuseColor", math::Vector4::create(102.f / 255.f, 205.f / 255.f, 50.f / 255.f, 1.f))
				->set("noiseMap", sceneManager->assets()->texture("texture/halftone.png"))
				->set("zSort", true)
				->set("blendMode", render::Blending::Mode::ALPHA),
			sceneManager->assets()->effect("effect/Hologram/Hologram.effect")
			));

		mesh2->addComponent(Surface::create(
			sceneManager->assets()->geometry("sphereGeometry"),
			material::Material::create()
				->set("diffuseColor", math::Vector4::create(240.f / 255.f, 95.f / 255.f, 120.f / 255.f, 1.f))
				->set("noiseMap", sceneManager->assets()->texture("texture/halftone.png"))
				->set("zSort", true)
				->set("blendMode", render::Blending::Mode::ALPHA),
			sceneManager->assets()->effect("effect/Hologram/Hologram.effect")
		));

		mesh->component<Transform>()->matrix()->appendTranslation(1.5f, 0.f, 0.f);
		mesh2->component<Transform>()->matrix()->appendTranslation(-1.5f, 0.f, 0.f);

		mesh->addComponent(hologram);
		mesh2->addComponent(hologram);

		root->addChild(mesh);
		root->addChild(mesh2);
		root->addChild(sceneManager->assets()->symbol(MODEL_FILENAME));
		//root->addChild(sceneManager->assets()->symbol(MODEL_FAKEHOLOGRAM));

		auto meshes = scene::NodeSet::create(sceneManager->assets()->symbol(MODEL_FILENAME))->descendants(false, false)->where([=](scene::Node::Ptr node)
		{
			return node->hasComponent<Surface>();
		});


		for (auto node : meshes->nodes())
		{
			node->addComponent(hologram);
			node->component<Surface>()->material()
				->set("blendMode", render::Blending::Mode::ALPHA)
				->set("noiseMap", sceneManager->assets()->texture("texture/halftone.png"))
				->set("diffuseColor", math::Vector4::create(173.f / 255.f, 216.f / 255.f, 230.f / 255.f, 1.f))
				->set("zSort", true);
		}

		// FXAA
		auto ppFx = sceneManager->assets()->effect("effect/FXAA/FXAA.effect");

		auto ppQuad = minko::scene::Node::create()->addComponent(Surface::create(
			geometry::QuadGeometry::create(sceneManager->assets()->context()),
			ppData,
			ppFx
			))->addComponent(Transform::create());

		ppRenderer->backgroundColor(0x7f7f7fff);

		auto ppScene = scene::Node::create();
		ppScene
			->addChild(ppQuad)
			->addComponent(ppRenderer);
	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
		auto width = math::clp2(w);
		auto height = math::clp2(h);

		ppTarget = render::Texture::create(sceneManager->assets()->context(), width, height, false, true);
		ppTarget->upload();
		ppData->set("backbuffer", ppTarget)
			->set("invBackbufferSize", Vector2::create(1.f / width, 1.f / height));
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
	{
        mesh->component<Transform>()->matrix()->prependRotationY(0.001f * deltaTime);
		mesh2->component<Transform>()->matrix()->prependRotationY(0.001f * deltaTime);
		sceneManager->assets()->symbol(MODEL_FILENAME)->component<Transform>()->matrix()->prependRotationY(0.001f * deltaTime);

		//sceneManager->nextFrame(time, deltaTime);
		sceneManager->nextFrame(time, deltaTime, ppTarget);
		ppRenderer->render(canvas->context());
	});

	fxLoader->load();

	canvas->run();

	return 0;
}


