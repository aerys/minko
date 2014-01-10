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
#include "minko/MinkoSerializer.hpp"
#include "minko/Any.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector2.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/MkStats.hpp"

#include "minko/serialize/TypeSerializer.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/geometry/SphereGeometry.hpp"

#define SERIALIZE // comment to test deserialization

using namespace minko;
using namespace minko::component;
using namespace minko::math;

void
serializeSceneExample(std::shared_ptr<file::AssetLibrary>		assets,
					  std::shared_ptr<scene::Node>				root,
					  std::shared_ptr<render::AbstractContext>	context)
{
	std::shared_ptr<file::SceneWriter> sceneWriter = file::SceneWriter::create();
	sceneWriter->data(root);
	sceneWriter->write("subScene.scene", assets, file::Options::create(context));
}

void
openSceneExample(std::shared_ptr<file::AssetLibrary>	assets, 
				 std::shared_ptr<scene::Node>			root)
{
	root->addChild(assets->symbol("subScene.scene"));
}

int loaded = 0;

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Cube", 800, 600);

	auto sceneManager = SceneManager::create(canvas->context());
	
	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
#ifdef SERIALIZE
		->queue("texture/box.png")
		->queue("effect/Basic.effect");
		
		sceneManager->assets()->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()));
		sceneManager->assets()->geometry("sphere", geometry::SphereGeometry::create(sceneManager->assets()->context(), 20, 20));
#else
		->registerParser<file::SceneParser>("scene")
		->queue("subScene.scene");
#endif

	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		if (loaded == 1)
			return;
		loaded = 1;

		auto root = scene::Node::create("root")
			->addComponent(sceneManager);

#ifdef SERIALIZE
		auto cubeMaterial = material::BasicMaterial::create()
			->diffuseMap(assets->texture("texture/box.png"))
			->diffuseColor(math::Vector4::create(1.f, 0.f, 0.f, 1.f))
			->blendMode(render::Blending::Mode::DEFAULT)
			->set<render::TriangleCulling>("triangleCulling", render::TriangleCulling::BACK);

		auto sphereMaterial = material::BasicMaterial::create()
			->diffuseMap(assets->texture("texture/box.png"))
			->diffuseColor(math::Vector4::create(0.f, 1.f, 0.f, 0.2f))
			->blendMode(render::Blending::Mode::ALPHA)
			->set<render::TriangleCulling>("triangleCulling", render::TriangleCulling::FRONT);

		assets->material("boxMaterial", cubeMaterial);
		assets->material("sphereMaterial", sphereMaterial);
		
		auto mesh = scene::Node::create("mesh")
			->addComponent(Transform::create())
			->addComponent(Surface::create(
				assets->geometry("sphere"),
				assets->material("sphereMaterial"),
				assets->effect("effect/Basic.effect")
			));

		auto mesh2 = scene::Node::create("mesh2")
			->addComponent(Transform::create())
			->addComponent(Surface::create(
				assets->geometry("cube"),
				assets->material("boxMaterial"),
				assets->effect("effect/Basic.effect")
			));

		auto mesh3 = scene::Node::create("mesh3")
			->addComponent(Transform::create())	
			->addComponent(Surface::create(
				assets->geometry("cube"),
				assets->material("boxMaterial"),
				assets->effect("effect/Basic.effect")
			));

		root->addChild(mesh);
		root->addChild(mesh2);
		root->addChild(mesh3);
		
		mesh2->component<Transform>()->matrix()->appendTranslation(0, 1, 0);
		mesh3->component<Transform>()->matrix()->appendTranslation(0, -1, 0);
		
		auto camera = scene::Node::create("camera")
			->addComponent(Renderer::create(0x7f7f7fff))
			->addComponent(Transform::create(
				Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f))
			))
			->addComponent(PerspectiveCamera::create(800.f / 600.f, (float)PI * 0.25f, .1f, 1000.f));
		root->addChild(camera);
#endif
		auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
		{
			root->children()[0]->children()[0]->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
		});

#ifdef SERIALIZE
		serializeSceneExample(assets, root, sceneManager->assets()->context());
#else
		openSceneExample(assets, root);
#endif
		auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, uint time, uint deltaTime)
		{
			sceneManager->nextFrame();
		});

		canvas->run();
	});
	
	sceneManager->assets()->load();

	return 0;
}

