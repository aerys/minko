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
#include "minko/MinkoMk.hpp"
#include "minko/Any.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector2.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/MkStats.hpp"

#include "minko/serialize/TypeSerializer.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/geometry/SphereGeometry.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

void
unitTest(std::shared_ptr<file::AssetLibrary> assets, std::shared_ptr<render::AbstractContext> context)
{
	std::shared_ptr<mk::MkStats> mkStat = mk::MkStats::create();
	
	/*
	mkStat->compressionStat(
		math::Vector2::create(0, 0), 
		std::bind(&serialize::TypeSerializer::serializeVector2, std::placeholders::_1), 
		std::bind(&deserialize::TypeDeserializer::deserializeVector2, std::placeholders::_1),
		"Vector2 init");

	mkStat->compressionStat<math::Vector2::Ptr, msgpack::type::tuple<uint, std::string>>(
		math::Vector2::create((float)rand(), (float)rand()),
		std::bind(&serialize::TypeSerializer::serializeVector2, std::placeholders::_1), 
		std::bind(&deserialize::TypeDeserializer::deserializeVector2, std::placeholders::_1),
		"Vector2");

	mkStat->compressionStat<math::Vector3::Ptr, msgpack::type::tuple<uint, std::string>>(
		math::Vector3::create(), 
		std::bind(&serialize::TypeSerializer::serializeVector3, std::placeholders::_1), 
		std::bind(&deserialize::TypeDeserializer::deserializeVector3, std::placeholders::_1),
		"Vector3 init");

	mkStat->compressionStat<math::Vector3::Ptr, msgpack::type::tuple<uint, std::string>>(
		math::Vector3::create((float)rand(), (float)rand(), (float)rand()),
		std::bind(&serialize::TypeSerializer::serializeVector3, std::placeholders::_1), 
		std::bind(&deserialize::TypeDeserializer::deserializeVector3, std::placeholders::_1),
		"Vector3");

		mkStat->compressionStat<math::Vector4::Ptr, msgpack::type::tuple<uint, std::string>>(
		math::Vector4::create(), 
		std::bind(&serialize::TypeSerializer::serializeVector4, std::placeholders::_1), 
		std::bind(&deserialize::TypeDeserializer::deserializeVector4, std::placeholders::_1),
		"Vector4 init");

	mkStat->compressionStat<math::Vector4::Ptr, msgpack::type::tuple<uint, std::string>>(
		math::Vector4::create((float)rand(), (float)rand(), (float)rand(), (float)rand()),
		std::bind(&serialize::TypeSerializer::serializeVector4, std::placeholders::_1), 
		std::bind(&deserialize::TypeDeserializer::deserializeVector4, std::placeholders::_1),
		"Vector4");

	mkStat->compressionStat<math::Matrix4x4::Ptr, msgpack::type::tuple<uint, std::string>>(
		math::Matrix4x4::create(), 
		std::bind(&serialize::TypeSerializer::serializeMatrix4x4, std::placeholders::_1), 
		std::bind(&deserialize::TypeDeserializer::deserializeMatrix4x4, std::placeholders::_1),
		"Matrix4x4 init");

	mkStat->compressionStat<math::Matrix4x4::Ptr, msgpack::type::tuple<uint, std::string>>(
		math::Matrix4x4::create()->appendTranslation((float)rand(), (float)rand(), (float)rand()),
		std::bind(&serialize::TypeSerializer::serializeMatrix4x4, std::placeholders::_1), 
		std::bind(&deserialize::TypeDeserializer::deserializeMatrix4x4, std::placeholders::_1),
		"Matrix4x4 translation");

	mkStat->compressionStat<math::Matrix4x4::Ptr, msgpack::type::tuple<uint, std::string>>(
		math::Matrix4x4::create()->appendRotation((float)rand(), math::Vector3::create(1, 0, 0)),
		std::bind(&serialize::TypeSerializer::serializeMatrix4x4, std::placeholders::_1), 
		std::bind(&deserialize::TypeDeserializer::deserializeMatrix4x4, std::placeholders::_1),
		"Matrix4x4 rotation Xaxis");

	mkStat->compressionStat<math::Matrix4x4::Ptr, msgpack::type::tuple<uint, std::string>>(
		math::Matrix4x4::create()->appendScale(rand(), rand(), rand()),
		std::bind(&serialize::TypeSerializer::serializeMatrix4x4, std::placeholders::_1), 
		std::bind(&deserialize::TypeDeserializer::deserializeMatrix4x4, std::placeholders::_1),
		"Matrix4x4 scale");

	mkStat->compressionStat<math::Matrix4x4::Ptr, msgpack::type::tuple<uint, std::string>>(
		math::Matrix4x4::create()->appendScale(rand(), rand(), rand())->appendRotation(rand(), math::Vector3::create(1, 1, 1))->appendTranslation(rand(), rand(), rand()),
		std::bind(&serialize::TypeSerializer::serializeMatrix4x4, std::placeholders::_1), 
		std::bind(&deserialize::TypeDeserializer::deserializeMatrix4x4, std::placeholders::_1),
		"Matrix4x4 full transform");
	*/
		
		
/*	mkStat->assetStat<geometry::Geometry::Ptr, file::GeometryParser, file::GeometryWriter>(
		context,
		assets,
		"cube",
		assets->geometry("cube")
		);
	
	mkStat->assetStat<geometry::Geometry::Ptr, file::GeometryParser, file::GeometryWriter>(
		context,
		assets,
		"sphere",
		assets->geometry("sphere")
		);

	mkStat->assetStat<material::Material::Ptr, file::MaterialParser, file::MaterialWriter>(
		context,
		assets,
		"boxMaterial",
		assets->material("boxMaterial"));*/
}

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
		->registerParser<file::SceneParser>("scene")
		->queue("texture/box.png")
		//->queue("subScene.scene");
		->queue("effect/Basic.effect");

	sceneManager->assets()->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()));
	sceneManager->assets()->geometry("sphere", geometry::SphereGeometry::create(sceneManager->assets()->context(), 20, 20));
	
	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		if (loaded == 1)
			return;
		loaded = 1;
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

		auto root = scene::Node::create("root")
			->addComponent(sceneManager);

		root->addChild(mesh);
		root->addChild(mesh2);
		root->addChild(mesh3);
		
		mesh2->component<Transform>()->matrix()->appendTranslation(0, 1, 0);
		mesh3->component<Transform>()->matrix()->appendTranslation(0, -1, 0);
		
		unitTest(assets, canvas->context());
		
		auto camera = scene::Node::create("camera")
			->addComponent(Renderer::create(0x7f7f7fff))
			->addComponent(Transform::create(
				Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f))
			))
			->addComponent(PerspectiveCamera::create(800.f / 600.f, (float)PI * 0.25f, .1f, 1000.f));
		root->addChild(camera);

		auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
		{
			root->children()[0]->children()[0]->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
		});

		int frameId = 0;

		auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, uint time, uint deltaTime)
		{
			sceneManager->nextFrame();
			frameId++;

			if (frameId == 3)
			{
				serializeSceneExample(assets, root, sceneManager->assets()->context());
				//openSceneExample(assets, root);
			}
		});

		canvas->run();
	});

	sceneManager->assets()->load();

	return 0;
}

