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
#include "minko/MinkoBullet.hpp"
#include "minko/MinkoParticles.hpp"
#include "minko/MinkoSerializer.hpp"

std::string MODEL_FILENAME = "model/primitives/primitives.scene";

//#define SERIALIZE // comment to test deserialization
#define DEACTIVATE_PHYSICS

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;

static
std::ostream&
printNodeInfo(std::ostream&, Node::Ptr);

void
serializeSceneExample(std::shared_ptr<file::AssetLibrary>		assets,
					  std::shared_ptr<scene::Node>				root,
					  std::shared_ptr<render::AbstractContext>	context)
{
	std::shared_ptr<file::SceneWriter> sceneWriter = file::SceneWriter::create();
	sceneWriter->data(root);
	sceneWriter->write(MODEL_FILENAME, assets, file::Options::create(context));
}

Node::Ptr
createWorldFrame(float axisLength, file::AssetLibrary::Ptr);

void
moveScene(Node::Ptr, float& tx, float& ty, float& tz);

void
toogleParticlesEmittingState(Node::Ptr);

void
openSceneExample(std::shared_ptr<file::AssetLibrary>	assets,
				 std::shared_ptr<scene::Node>			root)
{
	auto sceneNode = assets->symbol(MODEL_FILENAME);

	if (!sceneNode->hasComponent<Transform>())
		sceneNode->addComponent(Transform::create());

	root->addChild(sceneNode);

	auto withColliders = NodeSet::create(sceneNode)
		->descendants(true)
		->where([](Node::Ptr n){ return n->hasComponent<component::bullet::Collider>(); });

	for (auto& n : withColliders->nodes())
		n->addComponent(bullet::ColliderDebug::create(assets));
}

int main(int argc, char** argv)
{
	auto canvas			= Canvas::create("Minko Example - Serializer/Deserializer", 800, 600);
	auto sceneManager	= SceneManager::create(canvas->context());

	extension::SerializerExtension::activeExtension<extension::PhysicsExtension>();
    extension::SerializerExtension::activeExtension<extension::ParticlesExtension>();

	// setup assets
	sceneManager->assets()
        ->load("effect/Basic.effect")
        ->load("effect/Phong.effect")
		->load("effect/Line.effect")
        ->load("effect/Particles.effect");

	sceneManager->assets()->defaultOptions()
		->generateMipmaps(true)
		->effect(sceneManager->assets()->effect("phong"));

	sceneManager->assets()
		->material("defaultMaterial", material::BasicMaterial::create()->diffuseColor(0xFFFFFFFF))
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->geometry("defaultGeometry", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->registerParser<file::PNGParser>("png")
#ifdef SERIALIZE
		->queue("texture/box.png");
		
		sceneManager->assets()->geometry("sphere", geometry::SphereGeometry::create(sceneManager->assets()->context(), 20, 20));
#else
		->registerParser<file::SceneParser>("scene")
		->queue(MODEL_FILENAME);
#endif

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

#ifndef DEACTIVATE_PHYSICS
	auto physicWorld = bullet::PhysicsWorld::create();

	physicWorld->setGravity(math::Vector3::create(0.f, -9.8f, 0.f));

	root->addComponent(physicWorld);
#endif // DEACTIVATE_PHYSICS

	root->data()->addProvider(canvas->data()); // FIXME

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
			Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 5.f))
		))
		->addComponent(PerspectiveCamera::create(800.f / 600.f, (float)PI * 0.25f, .1f, 1000.f));

	root->addChild(camera);

	auto mesh = scene::Node::create("mesh")
		->addComponent(Transform::create());
	auto mesh2 = scene::Node::create("mesh2")
		->addComponent(Transform::create());
	auto mesh3 = scene::Node::create("mesh3")
		->addComponent(Transform::create());

	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
#ifdef SERIALIZE
		auto cubeMaterial = material::BasicMaterial::create()
			->diffuseMap(assets->texture("texture/box.png"))
			->diffuseColor(math::Vector4::create(1.f, 0.f, 0.f, 1.f))
//			->blendMode(render::Blending::Mode::DEFAULT)
			->set<render::TriangleCulling>("triangleCulling", render::TriangleCulling::BACK);

		auto sphereMaterial = material::BasicMaterial::create()
			->diffuseMap(assets->texture("texture/box.png"))
			->diffuseColor(math::Vector4::create(0.f, 1.f, 0.f, 0.2f))
//			->blendMode(render::Blending::Mode::ALPHA)
			->set<render::TriangleCulling>("triangleCulling", render::TriangleCulling::FRONT);

		assets->material("boxMaterial", cubeMaterial);
		assets->material("sphereMaterial", sphereMaterial);

		mesh->addComponent(Surface::create(
				assets->geometry("sphere"),
				assets->material("sphereMaterial"),
				assets->effect("effect/Basic.effect")
			));

		mesh2->addComponent(Surface::create(
				assets->geometry("cube"),
				assets->material("boxMaterial"),
				assets->effect("effect/Basic.effect")
			));
		mesh3->addComponent(Surface::create(
				assets->geometry("cube"),
				assets->material("boxMaterial"),
				assets->effect("effect/Basic.effect")
			));

		root->addChild(mesh);
		root->addChild(mesh2);
		root->addChild(mesh3);

		mesh2->component<Transform>()->matrix()->appendTranslation(0, 1, 0);
		mesh3->component<Transform>()->matrix()->appendTranslation(0, -1, 0);
#endif

#ifdef SERIALIZE
		serializeSceneExample(assets, root, sceneManager->assets()->context());
#else
		openSceneExample(assets, root);

		root->addChild(createWorldFrame(5.0f, sceneManager->assets()));
#endif
	});

	auto yaw = (float)PI * 0.5f;
	auto pitch = (float)PI * .5f;
	auto roll = 0.f;
	float minPitch = 0.f + float(1e-5);
	float maxPitch = (float)PI - float(1e-5);
	auto lookAt = Vector3::create(0.f, 0.f, 0.f);
	auto distance = 5.f;


	Signal<input::Mouse::Ptr, int, int>::Slot mouseMove;
	auto cameraRotationXSpeed = 0.f;
	auto cameraRotationYSpeed = 0.f;

	// handle mouse signals
	auto mouseWheel = canvas->mouse()->wheel()->connect([&](input::Mouse::Ptr m, int h, int v)
	{
		distance += (float)v / 5.f;
	});

	mouseMove = canvas->mouse()->move()->connect([&](input::Mouse::Ptr m, int dx, int dy)
	{
		if (m->leftButtonIsDown())
		{
			cameraRotationYSpeed = (float)dx * .01f;
			cameraRotationXSpeed = (float)dy * -.01f;
		}
	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		root->children()[0]->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
	});

	float tx = 0.0f;
	float ty = 0.0f;
	float tz = 0.0f;
	auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
	{
		if (k->keyIsDown(input::Keyboard::ScanCode::LEFT))
			tx -= 0.1f;
		else if (k->keyIsDown(input::Keyboard::ScanCode::RIGHT))
			tx += 0.1f;
		else if (k->keyIsDown(input::Keyboard::ScanCode::UP))
			tz += 0.1f;
		else if (k->keyIsDown(input::Keyboard::ScanCode::DOWN))
			tz -= 0.1f;
		
		// for particles
		if (k->keyIsDown(input::Keyboard::ScanCode::SPACE))
			toogleParticlesEmittingState(root);
	});

	auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime)
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

		// change the position of the serialized scene
		moveScene(sceneManager->assets()->symbol(MODEL_FILENAME), tx, ty, tz);

		sceneManager->nextFrame(time, deltaTime);
	});

	sceneManager->assets()->load();

	canvas->run();

	return 0;
}

void
toogleParticlesEmittingState(Node::Ptr root)
{
	if (root == nullptr)
		return;

	auto withParticles = NodeSet::create(root)
		->descendants(true)
		->where([](Node::Ptr n){ return n->hasComponent<component::ParticleSystem>(); });

	for (auto& n : withParticles->nodes())
	{
		auto particles = n->component<component::ParticleSystem>();

		particles->emitting(!particles->emitting());
	}
}

void
moveScene(Node::Ptr model, float& tx, float& ty, float& tz)
{
	if (model && model->hasComponent<Transform>())
	{
		model->component<Transform>()->matrix()->appendTranslation(tx, ty, tz);

		auto withColliders = scene::NodeSet::create(model)
			->descendants(true)
			->where([](scene::Node::Ptr n){ return n->hasComponent<bullet::Collider>(); });

		for (auto& n : withColliders->nodes())
			n->component<bullet::Collider>()->synchronizePhysicsWithGraphics();
	}

	tx = 0.0f;
	ty = 0.0f;
	tz = 0.0f;
}

Node::Ptr
createWorldFrame(float axisLength, file::AssetLibrary::Ptr assets)
{
	assert(assets->geometry("cube") && assets->effect("basic"));

	auto xAxis = Node::create("x-axis")
		->addComponent(Surface::create(
			assets->geometry("cube"),
			material::BasicMaterial::create()->diffuseColor(0xff0000ff),
			assets->effect("basic")
		))
		->addComponent(Transform::create(
			Matrix4x4::create()
				->appendTranslation(0.5f, 0.0f, 0.0f)
				->appendScale(axisLength, 0.01f, 0.01f)
		));

	auto yAxis = Node::create("y-axis")
		->addComponent(Surface::create(
			assets->geometry("cube"),
			material::BasicMaterial::create()->diffuseColor(0x00ff00ff),
			assets->effect("basic")
		))
		->addComponent(Transform::create(
			Matrix4x4::create()
				->appendTranslation(0.0f, 0.5f, 0.0f)
				->appendScale(0.01f, axisLength, 0.01f)
		));

	auto zAxis = Node::create("z-axis")
		->addComponent(Surface::create(
			assets->geometry("cube"),
			material::BasicMaterial::create()->diffuseColor(0x0000ffff),
			assets->effect("basic")
		))
		->addComponent(Transform::create(
			Matrix4x4::create()
				->appendTranslation(0.0f, 0.0f, 0.5f)
				->appendScale(0.01f, 0.01f, axisLength)
		));

	return Node::create("frame")
		->addChild(xAxis)
		->addChild(yAxis)
		->addChild(zAxis);
}

static
std::ostream&
printNodeInfo(std::ostream&	out, 
			  Node::Ptr		node)
{
	if (node == nullptr)
		return out;

	out << "\n'" << node->name() << "'" << (node->parent() ? "\t<- '" + node->parent()->name() + "'" : "" ) << std::endl;

	if (node->hasComponent<Transform>())
	{
		const auto& m = node->component<Transform>()->matrix()->data();

		out << "\t[" << m[0] << " " << m[1] << " " << m[2] << ";\n\t" 
			<< m[4] << " " << m[5] << " " << m[6] << ";\n\t" 
			<< m[8] << " " << m[9] << " " << m[10] << "]\n\t"
			<< "t = (" << m[3] << ", " << m[7] << ", " << m[11] << ")" 
			<< std::endl; 
	}

	for (auto& n : node->children())
		printNodeInfo(out, n);

	return out;
}
