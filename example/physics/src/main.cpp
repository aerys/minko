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

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;

const std::string	TEXTURE_FILENAME	= "texture/box.png";
const float			GROUND_WIDTH		= 5.0f;
const float			GROUND_HEIGHT		= 0.25f;
const float			GROUND_DEPTH		= 5.0f;
const float			GROUND_THICK		= 0.05f;

const float			MIN_MASS			= 1.0f;
const float			MAX_MASS			= 5.0f;
const float			MIN_SCALE			= 0.2f;
const float			MAX_SCALE			= 1.0f;
const auto			MIN_DROP_POS		= Vector3::create(-GROUND_WIDTH * 0.5f + 0.5f, 5.0f, -GROUND_DEPTH * 0.5f + 0.5f);
const auto			MAX_DROP_POS		= Vector3::create( GROUND_WIDTH * 0.5f - 0.5f, 5.0f,  GROUND_DEPTH * 0.5f - 0.5f);

const unsigned int	MAX_NUM_OBJECTS		= 32;

Signal<input::Keyboard::Ptr>::Slot keyDown;
uint numObjects = 0;

Node::Ptr
createPhysicsObject(unsigned int id, file::AssetLibrary::Ptr, bool isCube);

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Physics", 800, 600);

	auto sceneManager	= SceneManager::create(canvas->context());

	// setup assets
	sceneManager->assets()->defaultOptions()
		->resizeSmoothly(true)
		->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->queue(TEXTURE_FILENAME)
		->queue("effect/Phong.effect")
		->geometry("sphere",	geometry::SphereGeometry::create(sceneManager->assets()->context(), 16, 16))
		->geometry("cube",		geometry::CubeGeometry::create(sceneManager->assets()->context()));
	
	std::cout << "[space]\tdrop an object onto the scene (up to " << MAX_NUM_OBJECTS << ")" << std::endl;


	auto root = scene::Node::create("root")
		->addComponent(sceneManager)
		->addComponent(bullet::PhysicsWorld::create());

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
		Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(5.0f, 1.5f, 5.0f))
		))
		->addComponent(PerspectiveCamera::create(800.f / 600.f, (float)PI * 0.25f, .1f, 1000.f));

	root->addChild(camera);

	auto groundNode = scene::Node::create("groundNode")->addComponent(Transform::create(
		Matrix4x4::create()->appendRotationZ(-(float)PI * 0.1f)
		));

	// set-up lighting environment
	auto ambientLightNode = scene::Node::create("ambientLight")
		->addComponent(AmbientLight::create());

	auto dirLightNode = scene::Node::create("dirLight")
		->addComponent(DirectionalLight::create())
		->addComponent(Transform::create(
		Matrix4x4::create()->lookAt(Vector4::zero(), Vector4::create(0.5f, 5.0f, 3.0f))
		));

	dirLightNode->component<DirectionalLight>()->specular(0.5f);

	root
		->addChild(ambientLightNode)
		->addChild(dirLightNode);

	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		auto groundNodeA = scene::Node::create("groundNodeA")
			->addComponent(Transform::create(
				Matrix4x4::create()->appendScale(GROUND_WIDTH, GROUND_THICK, GROUND_DEPTH)
			))
			->addComponent(Surface::create(
				assets->geometry("cube"),
				material::BasicMaterial::create()->diffuseMap(assets->texture(TEXTURE_FILENAME)),
				assets->effect("phong")
			))
			->addComponent(bullet::Collider::create(
					bullet::ColliderData::create(
						0.0f, // static object (no mass)
						bullet::BoxShape::create(GROUND_WIDTH * 0.5f, GROUND_THICK * 0.5f, GROUND_DEPTH * 0.5f)
					)
			));

		auto groundNodeB = scene::Node::create("groundNodeB")
			->addComponent(Transform::create(
				Matrix4x4::create()
					->appendScale(GROUND_THICK, GROUND_HEIGHT, GROUND_DEPTH)
					->appendTranslation(0.5f * (GROUND_WIDTH + GROUND_THICK), 0.5f * (GROUND_HEIGHT - GROUND_THICK), 0.0f)
			))
			->addComponent(Surface::create(
				assets->geometry("cube"),
				material::BasicMaterial::create()->diffuseColor(0x241f1cff),
				assets->effect("phong")
			))
			->addComponent(bullet::Collider::create(
				bullet::ColliderData::create(
					0.0f, // static object (no mass)
					bullet::BoxShape::create(GROUND_THICK * 0.5f, GROUND_HEIGHT * 0.5f, GROUND_DEPTH * 0.5f))
			));

		groundNode
			->addChild(groundNodeA)
			->addChild(groundNodeB);

		root->addChild(groundNode);

		keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
		{
			if (k->keyIsDown(input::Keyboard::SPACE))
			{
				if (numObjects < MAX_NUM_OBJECTS)
				{
					root->addChild(createPhysicsObject(numObjects, sceneManager->assets(), rand() / (float)RAND_MAX > 0.5f));
					++numObjects;

					std::cout << "object #" << numObjects << " dropped" << std::endl;
				}
				else
					std::cout << "You threw away all your possible objects. Try again!" << std::endl;
			}
		});
	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, uint time, float deltaTime)
	{
		sceneManager->nextFrame();
	});

	sceneManager->assets()->load();

	canvas->run();

	return 0;
}


Node::Ptr
createPhysicsObject(unsigned int id, file::AssetLibrary::Ptr assets, bool isCube)
{
	const float mass		= MIN_MASS  + (rand() / (float)RAND_MAX) * (MAX_MASS - MIN_MASS);
	const float size		= MIN_SCALE + (rand() / (float)RAND_MAX) * (MAX_SCALE - MIN_SCALE);

	const float startX		= MIN_DROP_POS->x() + (rand() / (float)RAND_MAX) * (MAX_DROP_POS->x() - MIN_DROP_POS->x());
	const float startY		= MIN_DROP_POS->y() + (rand() / (float)RAND_MAX) * (MAX_DROP_POS->y() - MIN_DROP_POS->y());
	const float startZ		= MIN_DROP_POS->z() + (rand() / (float)RAND_MAX) * (MAX_DROP_POS->z() - MIN_DROP_POS->z());

	const float halfSize	= 0.5f * size;
	const float hue			= (id % 10) * 0.1f;
	auto		diffColor	= Color::hslaToRgba(hue, 1.0f, 0.5f, 1.0f);
	auto		specColor	= Color::hslaToRgba(hue, 1.0f, 0.8f, 1.0f);
	const float shininess	= 2.0f * (rand() / (float)RAND_MAX) * 6.0f;

	bullet::Collider::Ptr collider = nullptr;

	if (isCube)
	{
		auto boxColliderData = bullet::ColliderData::create(
			mass,
			bullet::BoxShape::create(halfSize, halfSize, halfSize)
		);

		collider = bullet::Collider::create(boxColliderData);
	}
	else
	{
		auto sphColliderData = bullet::ColliderData::create(
			mass,
			bullet::SphereShape::create(halfSize) 
		);
		collider = bullet::Collider::create(sphColliderData);
	}

	return scene::Node::create("node_" + std::to_string(id))
		->addComponent(Transform::create(
			Matrix4x4::create()
				->appendScale(size)
				->appendTranslation(startX, startY, startZ)
		))
		->addComponent(Surface::create(
			assets->geometry(isCube ? "cube" : "sphere"),
			material::PhongMaterial::create()
				->specularColor(specColor)
				->shininess(shininess)
				->diffuseColor(diffColor),
			assets->effect("phong")
		))
		->addComponent(collider);
}
